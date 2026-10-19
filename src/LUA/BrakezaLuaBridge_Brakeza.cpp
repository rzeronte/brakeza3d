#include "../../include/LUA/BrakezaLuaBridge.h"

void LUAIntegrationBrakeza(sol::state &lua)
{
    lua.new_usertype<Brakeza>("Brakeza3D",
        // Delta que ve el script: el del frame, o el acumulado desde su última ejecución si el
        // script tiene ticks_per_second > 0 (Brakeza::getScriptDeltaTime)
        "getDeltaTime", &Brakeza::getScriptDeltaTime,
        "getDeltaTimeMicro",  &Brakeza::getScriptDeltaTimeMicro,
        "getDeltaTimeMS",  &Brakeza::getScriptDeltaTimeMS,
        "getExecutionTime", &Brakeza::getExecutionTime,
        // Función estática: vale Brakeza.uniqueObjectLabel("x") y también Brakeza:uniqueObjectLabel("x"). Con los dos
        // puntos Lua pasa 'Brakeza' como primer argumento y el const char* quedaba NULL (strlen(NULL), crash)
        "uniqueObjectLabel", [](sol::variadic_args va) -> std::string {
            for (std::size_t i = 0; i < va.size(); ++i) {
                sol::object o = va.get<sol::object>(i);
                if (o.is<std::string>()) return Brakeza::UniqueObjectLabel(o.as<std::string>().c_str());
            }
            return Brakeza::UniqueObjectLabel("Object");
        },
        "Shutdown", &Brakeza::Shutdown,
        "requestExit", [](Brakeza& b, sol::object codeArg) {
            int code = (codeArg.valid() && codeArg.get_type() == sol::type::number) ? codeArg.as<int>() : 0;
            b.requestExit(code);
        },
        // Lee un parámetro declarado por el proyecto en su sección "cli_params" (assets/projects/*.json),
        // resuelto contra --set key=value. El motor no sabe qué significa 'key' -- solo tipo/default.
        "getCliParam", [](Brakeza& b, const std::string& key) -> sol::object {
            auto it = Config::get()->cliParams.find(key);
            if (it == Config::get()->cliParams.end()) return sol::nil;
            auto &lua = Components::get()->Scripting()->getLua();
            switch (it->second.type) {
                case Config::CliParamValue::Type::Bool:   return sol::make_object(lua, it->second.b);
                case Config::CliParamValue::Type::Number: return sol::make_object(lua, it->second.n);
                default:                                  return sol::make_object(lua, it->second.s);
            }
        },
        "AddObject3D",  &Brakeza::AddObject3D,
        "getObjectByName",    &Brakeza::getObjectByName,
        // Diagnóstico: todos los objetos de escena como tablas {name, type (ObjectType),
        // x, y, z, scripts (nº de scripts enganchados), removed, enabled}
        "getSceneObjectsInfo", [](Brakeza* b, sol::this_state st) -> sol::table {
            sol::state_view lua(st);
            sol::table out = lua.create_table();
            int i = 1;
            for (auto *o : b->copySceneObjects()) {
                if (o == nullptr) continue;
                sol::table t = lua.create_table();
                t["name"]    = o->getName();
                t["type"]    = static_cast<int>(o->getTypeObject());
                const Vertex3D p = o->getPosition();
                t["x"] = p.x; t["y"] = p.y; t["z"] = p.z;
                t["scripts"] = static_cast<int>(o->getScripts().size());
                t["removed"] = o->isRemoved();
                t["enabled"] = o->isEnabled();
                out[i++] = t;
            }
            return out;
        },
        "getObjectById",      &Brakeza::getObjectById,
        "getObjectAtScreen",  &Brakeza::getObjectAtScreen,
        "removeAllObjects",      &Brakeza::removeAllObjects,
        "getPendingJobsCount",   &Brakeza::getPendingJobsCount,
        "cancelPendingJobs",     &Brakeza::cancelPendingJobs,
        // Mesh3D por nombre (nil si no existe o no es una malla): permite setDiffuseTexture sobre objetos de una escena
        "getMesh3DByName", [](Brakeza* b, const std::string& name) -> Mesh3D* {
            return dynamic_cast<Mesh3D*>(b->getObjectByName(name));
        },
        "getMesh3DAnimationByName", [](Brakeza* b, const std::string& name) -> Mesh3DAnimation* {
            return dynamic_cast<Mesh3DAnimation*>(b->getObjectByName(name));
        },
        // "this" en un script de objeto se guarda con tipo estático Object3D* (ver
        // Object3D::ReloadScriptsEnvironment) -- Lua lo expone siempre con el usertype base, sin
        // los métodos propios de la subclase real (p.ej. Image3DAnimation::setSize), aunque el
        // objeto en memoria SÍ sea de ese tipo. Mismo patrón que getMesh3DAnimationByName: el
        // dynamic_cast explícito aquí da a sol2 el tipo estático correcto para exponer la API
        // completa de Image3DAnimation.
        "getImage3DAnimationByName", [](Brakeza* b, const std::string& name) -> Image3DAnimation* {
            return dynamic_cast<Image3DAnimation*>(b->getObjectByName(name));
        },
        "getProjectileByName", [](Brakeza* b, const std::string& name) -> Projectile* {
            return dynamic_cast<Projectile*>(b->getObjectByName(name));
        },
        // Emisor de partículas por nombre (nil si no existe o no es un ParticleEmitter): permite
        // setStopAdd/setContext sobre emisores cargados desde una escena
        "getParticleEmitterByName", [](Brakeza* b, const std::string& name) -> ParticleEmitter* {
            return dynamic_cast<ParticleEmitter*>(b->getObjectByName(name));
        },
        "loadLightsFromFBX", [](Brakeza* b, const std::string& path,
                                   float posX, float posY, float posZ,
                                   float rotX, float rotY, float rotZ,
                                   float scale,
                                   sol::optional<bool> enabledByDefault,
                                   sol::this_state s) -> sol::table {
            bool enabled = enabledByDefault.value_or(true);
            auto lights = FBXLightLoader::LoadLightsFromFile(path, posX, posY, posZ, rotX, rotY, rotZ, scale, enabled);
            sol::state_view lua(s);
            sol::table result = lua.create_table();
            int idx = 1;
            for (auto* light : lights) {
                b->AddObject3D(light, light->getName());
                if (light->getTypeObject() == ObjectType::LightSpot)
                    result[idx++] = static_cast<LightSpot*>(light);
                else
                    result[idx++] = static_cast<LightPoint*>(light);
            }
            return result;
        }
    );

    lua.new_usertype<Camera3D>("Camera3D",
       sol::base_classes, sol::bases<Object3D>(),
        "getM3ViewMatrix", &Camera3D::getM3ViewMatrix,
       "setFOV", &Camera3D::setFOV,
       "getFOV", [](Camera3D*) { return Config::get()->HORIZONTAL_FOV; },
       "getM3ProjectionMatrix", &Camera3D::getM3ProjectionMatrix,
       "setRotationFromEulerAngles", &Camera3D::setRotationFromEulerAngles,
       "getNearPlane", [](Camera3D*) { return Config::get()->FRUSTUM_NEARPLANE_DISTANCE; },
       "setNearPlane", [](Camera3D*, float v) { Config::get()->FRUSTUM_NEARPLANE_DISTANCE = v; },
       "getFarPlane",  [](Camera3D*) { return Config::get()->FRUSTUM_FARPLANE_DISTANCE; },
       "setFarPlane",  [](Camera3D*, float v) { Config::get()->FRUSTUM_FARPLANE_DISTANCE = v; }
    );

    lua.new_usertype<AABB3D>("AABB3D",
    sol::constructors<AABB3D()>(),
        "min", &AABB3D::min,
        "max", &AABB3D::max,
        "vertices", &AABB3D::vertices,
        "size", &AABB3D::size,
        "isColliding", &AABB3D::isColliding,
        "setScale", &AABB3D::setScale,
        "updateVertices", &AABB3D::updateVertices,
        "getPlanes", &AABB3D::getPlanes,
        "getCenter", &AABB3D::getCenter,
        "isPointInside", &AABB3D::isPointInside
    );

    lua.new_usertype<OctreeNode>("OctreeNode",
    "leaf", &OctreeNode::leaf,
        "bounds", &OctreeNode::bounds,
        "children", &OctreeNode::children, // Exponer el vector directamente
        "isLeaf", &OctreeNode::isLeaf
    );

    lua.new_usertype<Octree>("Octree",
    sol::constructors<Octree(AABB3D, int)>(),
        "BuildOctree", &Octree::BuildOctree,
        "FindNode", &Octree::FindNode,
        "isTriangleInsideAABB", &Octree::isTriangleInsideAABB,
        "root", &Octree::root
    );

    lua.new_usertype<CubeGrid3D>("CubeGrid3D",
        sol::constructors<CubeGrid3D()>(),
        "box", &CubeGrid3D::box,
        "posX", &CubeGrid3D::posX,
        "posY", &CubeGrid3D::posY,
        "posZ", &CubeGrid3D::posZ,
        "passed", &CubeGrid3D::passed
    );

    lua.new_usertype<Grid3D>("Grid3D",
        sol::constructors<Grid3D(AABB3D, int, int, int)>(),
        "isEmpty", &Grid3D::isEmpty,
        "getCubeFromPosition", &Grid3D::getCubeFromPosition,
        "getNumberCubesX", &Grid3D::getNumberCubesX,
        "getNumberCubesY", &Grid3D::getNumberCubesY,
        "getNumberCubesZ", &Grid3D::getNumberCubesZ,
        "getBoxes", &Grid3D::getBoxes,
        "setTravel", &Grid3D::setTravel,
        "MakeTravelCubesGrid", &Grid3D::MakeTravelCubesGrid,
        "computePath", &Grid3D::computePath,
        "fillGrid3DFromImage", sol::overload(
            [](Grid3D& g, const std::string& path, int threshold)                   { g.fillGrid3DFromImage(path, threshold); },
            [](Grid3D& g, const std::string& path, int threshold, bool fz, bool fx) { g.fillGrid3DFromImage(path, threshold, fz, fx); }
        ),
        "fillCostFromImage", sol::overload(
            [](Grid3D& g, const std::string& path, int threshold, float cost)                   { g.fillCostFromImage(path, threshold, cost); },
            [](Grid3D& g, const std::string& path, int threshold, float cost, bool fz, bool fx) { g.fillCostFromImage(path, threshold, cost, fz, fx); }
        ),
        // cells: tabla plana {x1, z1, x2, z2, ...} -- un único vector nuevo por llamada (copy-on-write).
        "setCellsCost", [](Grid3D& g, const sol::table& cells, float cost) {
            std::vector<std::pair<int,int>> list;
            const size_t n = cells.size();
            list.reserve(n / 2);
            for (size_t i = 1; i + 1 <= n; i += 2) {
                list.emplace_back(cells.get<int>(i), cells.get<int>(i + 1));
            }
            g.setCellsCost(list, cost);
        },
        "getCellCost", &Grid3D::getCellCost,
        "drawDebug", sol::overload(
            [](Grid3D& g)              { g.drawDebug(); },
            [](Grid3D& g, Color color) { g.drawDebug(color); }
        ),
        "isCellWalkable", &Grid3D::isCellWalkable,
        "snapToWalkable", &Grid3D::snapToWalkable,
        "requestPath", [](Grid3D& g, const std::string& unitName, int gx1, int gz1, int gx2, int gz2, int requestGen) {
            Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(
                std::make_shared<ThreadJobPathfinding>(&g, unitName, gx1, gz1, gx2, gz2, requestGen)
            );
        }
    );

}
