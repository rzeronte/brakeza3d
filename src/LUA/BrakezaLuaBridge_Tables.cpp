#include "../../include/LUA/BrakezaLuaBridge.h"

// Obj.*: acceso a objetos POR ID NUMERICO, como lua_CFunction crudas (sin sol, sin crear userdata).
// Para los bucles calientes de las IAs (cientos de unidades por frame): el id se resuelve en el motor, asi que no
// existe puntero colgante (si el objeto ya no esta, la funcion lo dice) y no hace falta isValidObjectPointer().
// Un id que no esta en el indice puede ser un objeto ELIMINADO o uno recien creado cuyo AddObject3D aun no se ha
// ejecutado (ver feedback_async_object_cache_wipe): quien llama debe tratar "nil" como "ignorar este frame" y no
// como "borrar mi estado"; para saber si esta eliminado de verdad, Obj.removed(id) solo devuelve true si el
// objeto sigue indexado y marcado como eliminado.
//   local x, y, z = Obj.pos(id)          -- nil si el id no esta
//   Obj.setPos(id, x, y, z)              -- devuelve true si lo movio
//   Obj.alive(id)                        -- true si esta indexado y no eliminado
//   Obj.removed(id)                      -- true si esta indexado y marcado como eliminado
//   Obj.enabled(id)                      -- true si esta indexado, no eliminado y habilitado
static inline Object3D *objById(lua_State *L, int idx)
{
    return Brakeza::get()->getObjectById((unsigned int) lua_tonumber(L, idx));
}
static int luaObjPos(lua_State *L)
{
    Object3D *o = objById(L, 1);
    if (o == nullptr) { lua_pushnil(L); return 1; }
    const Vertex3D &p = o->getPosition();
    lua_pushnumber(L, p.x); lua_pushnumber(L, p.y); lua_pushnumber(L, p.z);
    return 3;
}
static int luaObjSetPos(lua_State *L)
{
    Object3D *o = objById(L, 1);
    if (o == nullptr || o->isRemoved()) { lua_pushboolean(L, 0); return 1; }
    o->setPosition(Vertex3D((float) lua_tonumber(L, 2), (float) lua_tonumber(L, 3), (float) lua_tonumber(L, 4)));
    lua_pushboolean(L, 1);
    return 1;
}
static int luaObjAlive(lua_State *L)
{
    Object3D *o = objById(L, 1);
    lua_pushboolean(L, o != nullptr && !o->isRemoved());
    return 1;
}
static int luaObjRemoved(lua_State *L)
{
    Object3D *o = objById(L, 1);
    lua_pushboolean(L, o != nullptr && o->isRemoved());
    return 1;
}
static int luaObjEnabled(lua_State *L)
{
    Object3D *o = objById(L, 1);
    lua_pushboolean(L, o != nullptr && !o->isRemoved() && o->isEnabled());
    return 1;
}

void LUAIntegrationTables(sol::state &lua)
{
    // Lectura del profiler desde Lua (diagnóstico: Tests/RenderPerfProbe.lua). snapshot() devuelve
    // { cpu = {medida = ms del último frame}, gpu = {pasada = ms} } con las medidas por componente
    // (<Componente>_pre/_update/_post y pasadas de render) y las de GPU (con setGpuTiming(true)).
    lua["ProfilerLua"] = lua.create_table();
    // Diagnostico: ProfilerLua.setJumpTrace(umbral, filtro) escribe quien mueve de golpe (> umbral u en XZ) a objetos cuyo nombre
    // contiene filtro (ver Object3D::reportPositionJump). umbral 0 = apagado.
    lua["ProfilerLua"]["setJumpTrace"] = [](float threshold, const std::string &filter) {
        Object3D::debugJumpFilter = filter;
        Object3D::debugJumpThreshold = threshold;
    };
    // Interruptor general del profiler (el mismo que el de la ventana Profiler): apagado no mide nada
    lua["ProfilerLua"]["setEnabled"] = [](bool v) { Profiler::get()->setEnabled(v); };
    lua["ProfilerLua"]["setGpuTiming"] = [](bool v) { Profiler::get()->setGpuTimingEnabled(v); };
    lua["ProfilerLua"]["setScriptDetail"] = [](bool v) { Profiler::get()->setScriptDetailEnabled(v); };
    // Cuenta las llamadas glDraw* del frame (snapshot().drawCalls = las del frame anterior)
    lua["ProfilerLua"]["setCountDrawCalls"] = [](bool v) { Profiler::get()->setCountDrawCalls(v); };
    // Draw calls del último frame por "etapa | sitio" (requiere setCountDrawCalls(true))
    lua["ProfilerLua"]["drawCallsBySource"] = [](sol::this_state st) -> sol::table {
        sol::state_view lv(st);
        sol::table out = lv.create_table();
        for (const auto& [k, v] : Profiler::get()->getLastDrawCallsBySource()) out[k] = v;
        return out;
    };
    // Reloj de alta resolución (segundos, steady_clock) para cronometrar secciones de un script:
    //   local t0 = ProfilerLua.now() ... local ms = (ProfilerLua.now() - t0) * 1000
    // Lua aquí no tiene la librería os (ni os.clock), y getExecutionTime solo cambia una vez por frame.
    lua["ProfilerLua"]["now"] = []() -> double {
        return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
    };
    lua["ProfilerLua"]["snapshot"] = [](sol::this_state st) -> sol::table {
        sol::state_view lua(st);
        sol::table out = lua.create_table();
        sol::table cpu = lua.create_table();
        for (auto& [name, m] : Profiler::get()->getComponentMeasures()) cpu[name] = m.diffTime;
        sol::table gpu = lua.create_table();
        for (auto& [name, m] : Profiler::get()->getGpuMeasures()) gpu[name] = m.lastGpuMs;
        sol::table scripts = lua.create_table();   // "[G] Nombre_update", "[O] objeto/script_update"...
        for (auto& [name, m] : Profiler::get()->getScriptMeasures()) scripts[name] = m.diffTime;
        out["cpu"] = cpu;
        out["gpu"] = gpu;
        out["scripts"] = scripts;
        out["drawCalls"] = Profiler::get()->getLastDrawCalls();
        return out;
    };

    // Índice espacial bajo demanda (ver SpatialIndex.h). Los objetos entran con obj:setSpatialMask(bits);
    // la primera consulta de cada frame lo reconstruye. Resultados como números (id, x, y, z, d2), sin
    // crear userdata por resultado: el objeto se pide aparte (Brakeza:getObjectById) solo si hace falta.
    //   local id, x, y, z, d2 = Spatial.nearest(x, z, r, mask [, excludeId] [, enabledOnly])  -- nil si no hay
    //   local n = Spatial.queryInto(out, x, z, r, mask [, maxN] [, sorted] [, enabledOnly])
    //          out[(i-1)*5 + 1..5] = id, x, y, z, d2 para i = 1..n (la tabla se reutiliza; lo que sobra no se borra)
    //   local n = Spatial.count(x, z, r, mask [, enabledOnly])
    {
        lua["Obj"] = lua.create_table();
        lua_State *L = lua.lua_state();
        lua_getglobal(L, "Obj");
        lua_pushcfunction(L, luaObjPos);     lua_setfield(L, -2, "pos");
        lua_pushcfunction(L, luaObjSetPos);  lua_setfield(L, -2, "setPos");
        lua_pushcfunction(L, luaObjAlive);   lua_setfield(L, -2, "alive");
        lua_pushcfunction(L, luaObjRemoved); lua_setfield(L, -2, "removed");
        lua_pushcfunction(L, luaObjEnabled); lua_setfield(L, -2, "enabled");
        lua_pop(L, 1);
    }

    lua["Spatial"] = lua.create_table();
    lua["Spatial"]["setCellSize"] = [](float size) { Brakeza::get()->getSpatialIndex().setCellSize(size); };
    lua["Spatial"]["getCellSize"] = []() -> float { return Brakeza::get()->getSpatialIndex().getCellSize(); };
    lua["Spatial"]["size"] = []() -> int { return Brakeza::get()->getSpatialIndex().size(); };
    lua["Spatial"]["nearest"] = [](sol::this_state st, float x, float z, float r, double mask,
                                   sol::optional<unsigned int> excludeId, sol::optional<bool> enabledOnly) -> sol::variadic_results {
        sol::variadic_results res;
        SpatialIndex::Hit h{};
        if (Brakeza::get()->getSpatialIndex().nearest(x, z, r, (uint32_t) mask, excludeId.value_or(0), enabledOnly.value_or(false), h)) {
            res.push_back(sol::make_object(st, h.id));
            res.push_back(sol::make_object(st, h.x));
            res.push_back(sol::make_object(st, h.y));
            res.push_back(sol::make_object(st, h.z));
            res.push_back(sol::make_object(st, h.d2));
        } else {
            res.push_back(sol::make_object(st, sol::lua_nil));
        }
        return res;
    };
    lua["Spatial"]["queryInto"] = [](sol::table out, float x, float z, float r, double mask,
                                     sol::optional<int> maxN, sol::optional<bool> sorted, sol::optional<bool> enabledOnly) -> int {
        const auto &hits = Brakeza::get()->getSpatialIndex().query(x, z, r, (uint32_t) mask, maxN.value_or(0), sorted.value_or(false), enabledOnly.value_or(false));
        int k = 1;
        for (const auto &h : hits) {
            out.raw_set(k, h.id, k + 1, h.x, k + 2, h.y, k + 3, h.z, k + 4, h.d2);
            k += 5;
        }
        return (int) hits.size();
    };
    lua["Spatial"]["count"] = [](float x, float z, float r, double mask, sol::optional<bool> enabledOnly) -> int {
        return Brakeza::get()->getSpatialIndex().count(x, z, r, (uint32_t) mask, enabledOnly.value_or(false));
    };
    lua["Spatial"]["drawDebug"] = [](float r, float g, float b, float a) {
        Brakeza::get()->getSpatialIndex().drawDebug(r, g, b, a);
    };

    // Resolución de solapes entre agentes (ver CrowdSolver.h). Genérico: radio + prioridad + capas.
    //   Crowd.add(obj, radius [, priority=-1 (auto: parado resiste, en marcha cede)] [, layer=1] [, collidesWith=0xFFFFFFFF])
    //   Crowd.remove(obj)   Crowd.setPriority(obj, p)   Crowd.setRadius(obj, r)   Crowd.has(obj)   Crowd.clear()
    //   Crowd.configure{ iterations=, maxPushSpeed=, stiffness=, margin=, enabled=, autoStillSpeed=, autoStillDelay=,
    //                    autoMovingPriority=, autoStillPriority= }
    //   Crowd.setWalkable(function(x, z) return bool end)  -- o nil: revierte/desliza correcciones a sitios prohibidos
    //   Crowd.stats() -> { agents, pairs, moved, reverted, maxPush, stepMs }
    // Geometría de puestos (listas planas { x1, z1, x2, z2, ... }, índices 1-based en assign):
    //   Crowd.ringSlots(cx, cz, radius, count [, startAngle])
    //   Crowd.packedSlots(cx, cz, count, spacing [, dirX, dirZ])
    //   Crowd.arcSlots(tx, tz, fromX, fromZ, dist, spacing, count)
    //   Crowd.assignSlots(fromFlat, slotsFlat) -> { slotIndex por punto (0 = sin puesto) }
    {
        auto slotsToTable = [](sol::this_state st, const std::vector<CrowdSolver::Slot> &v) {
            sol::state_view lv(st);
            sol::table t = lv.create_table((int) v.size() * 2, 0);
            int k = 1;
            for (const auto &p : v) { t.raw_set(k, p.x, k + 1, p.z); k += 2; }
            return t;
        };
        auto flatToSlots = [](sol::table t) {
            std::vector<CrowdSolver::Slot> v;
            const int n = (int) t.size() / 2;
            v.reserve(n);
            for (int i = 0; i < n; ++i) v.push_back({ t.get<float>(2 * i + 1), t.get<float>(2 * i + 2) });
            return v;
        };
        lua["Crowd"] = lua.create_table();
        lua["Crowd"]["add"] = [](Object3D *obj, float radius, sol::optional<int> priority, sol::optional<double> layer, sol::optional<double> collidesWith) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Crowd] add() sobre un objeto invalido (Lua)"); return; }
            Brakeza::get()->getCrowd().add(obj, radius, priority.value_or(-1), (uint32_t) layer.value_or(1.0),
                                           (uint32_t) collidesWith.value_or(4294967295.0));
        };
        lua["Crowd"]["remove"] = [](Object3D *obj) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) return;
            Brakeza::get()->getCrowd().remove(obj);
        };
        lua["Crowd"]["setPriority"] = [](Object3D *obj, int p) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) return;
            Brakeza::get()->getCrowd().setPriority(obj, p);
        };
        lua["Crowd"]["setRadius"] = [](Object3D *obj, float r) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) return;
            Brakeza::get()->getCrowd().setRadius(obj, r);
        };
        lua["Crowd"]["has"] = [](Object3D *obj) -> bool {
            if (!Brakeza::get()->isValidObjectPointer(obj)) return false;
            return Brakeza::get()->getCrowd().has(obj);
        };
        lua["Crowd"]["clear"] = []() { Brakeza::get()->getCrowd().clear(); };
        lua["Crowd"]["size"] = []() -> int { return Brakeza::get()->getCrowd().size(); };
        lua["Crowd"]["configure"] = [](sol::table t) {
            auto &c = Brakeza::get()->getCrowd().config();
            if (auto v = t.get<sol::optional<int>>("iterations"))     c.iterations = *v;
            if (auto v = t.get<sol::optional<float>>("maxPushSpeed")) c.maxPushSpeed = *v;
            if (auto v = t.get<sol::optional<float>>("stiffness"))    c.stiffness = *v;
            if (auto v = t.get<sol::optional<float>>("margin"))       c.margin = *v;
            if (auto v = t.get<sol::optional<bool>>("enabled"))       c.enabled = *v;
            if (auto v = t.get<sol::optional<float>>("autoStillSpeed"))     c.autoStillSpeed = *v;
            if (auto v = t.get<sol::optional<float>>("autoStillDelay"))     c.autoStillDelay = *v;
            if (auto v = t.get<sol::optional<int>>("autoMovingPriority"))   c.autoMovingPriority = *v;
            if (auto v = t.get<sol::optional<int>>("autoStillPriority"))    c.autoStillPriority = *v;
        };
        lua["Crowd"]["setWalkable"] = [](sol::object fn) {
            if (fn.is<sol::function>()) {
                sol::function f = fn.as<sol::function>();
                Brakeza::get()->getCrowd().setWalkable([f](float x, float z) -> bool {
                    sol::protected_function_result r = f(x, z);
                    if (!r.valid()) return true;      // un error del script no debe inmovilizar a todos
                    return r.get<bool>();
                });
            } else {
                Brakeza::get()->getCrowd().setWalkable(nullptr);
            }
        };
        lua["Crowd"]["stats"] = [](sol::this_state st) {
            sol::state_view lv(st);
            const auto &s = Brakeza::get()->getCrowd().getStats();
            sol::table t = lv.create_table();
            t["agents"] = s.agents; t["pairs"] = s.pairs; t["moved"] = s.moved;
            t["reverted"] = s.reverted; t["maxPush"] = s.maxPush; t["stepMs"] = s.stepMs;
            return t;
        };
        lua["Crowd"]["ringSlots"] = [slotsToTable](sol::this_state st, float cx, float cz, float radius, int count, sol::optional<float> startAngle) {
            return slotsToTable(st, CrowdSolver::ringSlots(cx, cz, radius, count, startAngle.value_or(0.0f)));
        };
        lua["Crowd"]["packedSlots"] = [slotsToTable](sol::this_state st, float cx, float cz, int count, float spacing, sol::optional<float> dirX, sol::optional<float> dirZ) {
            return slotsToTable(st, CrowdSolver::packedSlots(cx, cz, count, spacing, dirX.value_or(0.0f), dirZ.value_or(1.0f)));
        };
        lua["Crowd"]["arcSlots"] = [slotsToTable](sol::this_state st, float tx, float tz, float fromX, float fromZ, float dist, float spacing, int count) {
            return slotsToTable(st, CrowdSolver::arcSlots(tx, tz, fromX, fromZ, dist, spacing, count));
        };
        lua["Crowd"]["assignSlots"] = [flatToSlots](sol::this_state st, sol::table from, sol::table slots) {
            sol::state_view lv(st);
            const std::vector<int> res = CrowdSolver::assignSlots(flatToSlots(from), flatToSlots(slots));
            sol::table t = lv.create_table((int) res.size(), 0);
            for (size_t i = 0; i < res.size(); ++i) t[i + 1] = res[i] + 1;   // 1-based; 0 = sin puesto
            return t;
        };
    }

    lua["ShadowConfig"] = lua.create_table();
    lua["ShadowConfig"]["setFrustumSize"]   = [](float v) { Config::get()->SHADOW_MAPPING_FRUSTUM_SIZE = v; };
    lua["ShadowConfig"]["getFrustumSize"]   = []() -> float { return Config::get()->SHADOW_MAPPING_FRUSTUM_SIZE; };
    lua["ShadowConfig"]["setFarPlane"]      = [](float v) { Config::get()->SHADOW_MAPPING_DEPTH_FAR_PLANE = v; };
    lua["ShadowConfig"]["getFarPlane"]      = []() -> float { return Config::get()->SHADOW_MAPPING_DEPTH_FAR_PLANE; };
    lua["ShadowConfig"]["setIntensity"]     = [](float v) { Config::get()->SHADOW_MAPPING_INTENSITY = v; };
    lua["ShadowConfig"]["getIntensity"]     = []() -> float { return Config::get()->SHADOW_MAPPING_INTENSITY; };
    lua["ShadowConfig"]["setShadowEnabled"] = [](bool v)  { Config::get()->ENABLE_SHADOW_MAPPING = v; };
    lua["ShadowConfig"]["isShadowEnabled"]  = []() -> bool { return Config::get()->ENABLE_SHADOW_MAPPING; };
    lua["ShadowConfig"]["setResolution"]    = [](int res) {
        res = std::max(256, std::min(res, 8192));
        Config::get()->SHADOW_MAP_RESOLUTION = res;
        Components::get()->Render()->getShaders()->shaderShadowPass->ResetFramebuffers();
    };
    lua["ShadowConfig"]["getResolution"]    = []() -> int { return Config::get()->SHADOW_MAP_RESOLUTION; };
    lua["ShadowConfig"]["setPCFKernelSize"] = [](int size) {
        Config::get()->SHADOW_MAPPING_PCF_KERNEL_SIZE = std::max(0, size);
    };
    lua["ShadowConfig"]["getPCFKernelSize"] = []() -> int { return Config::get()->SHADOW_MAPPING_PCF_KERNEL_SIZE; };
    lua["ShadowConfig"]["setFocus"] = [](float x, float y, float z) {
        Config::get()->SHADOW_MAPPING_FOCUS = glm::vec3(x, y, z);
    };
}
