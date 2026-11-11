//
// Undo/redo mínimo del editor (ver UndoManager.h)
//

#include "../../include/Render/UndoManager.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/3D/Object3D.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Render/JSONSerializerRegistry.h"
#include "../../include/Threads/ThreadJobLoadObject.h"
#include "../../include/Loaders/Scene.h"
#include "../../include/3D/Mesh3D.h"
#include "../../include/3D/LightSpot.h"
#include "../../include/3D/ParticleEmitter.h"
#include "../../include/3D/Sound3D.h"
#include "../../include/Render/ParticlesContext.h"
#include <SDL.h>
#include <algorithm>
#include <memory>
#include <imgui.h>

UndoManager &UndoManager::get()
{
    static UndoManager instance;
    return instance;
}

bool UndoManager::State::operator==(const State &o) const
{
    return std::equal(pos, pos + 3, o.pos) && std::equal(rot, rot + 9, o.rot)
        && std::equal(scale, scale + 3, o.scale) && alpha == o.alpha && enabled == o.enabled && extra == o.extra;
}

namespace
{
    struct Out
    {
        std::vector<float> &v;
        void f(float x) { v.push_back(x); }
        void b(bool x)  { v.push_back(x ? 1.0f : 0.0f); }
        void i(int x)   { v.push_back(static_cast<float>(x)); }
    };
    struct In
    {
        const std::vector<float> &v;
        size_t k = 0;
        float f()  { return k < v.size() ? v[k++] : 0.0f; }
        bool  b()  { return f() != 0.0f; }
        int   i()  { return static_cast<int>(f()); }
    };

    // Todos los campos numéricos del contexto del emisor, para detectar cambios. Un campo nuevo que no esté aquí
    // sigue restaurándose (se copia el contexto entero) pero su edición sola no crearía un paso de undo.
    void contextFloats(const ParticlesContext &c, Out &o)
    {
        o.f(c.GRAVITY); o.f(c.PARTICLES_BY_SECOND); o.f(c.PARTICLE_LIFESPAN); o.i(c.SMOKE_ANGLE_RANGE);
        o.f(c.MIN_VELOCITY); o.f(c.MAX_VELOCITY); o.i(c.MIN_ALPHA); o.i(c.MAX_ALPHA);
        o.f(c.POSITION_NOISE); o.f(c.VELOCITY_NOISE); o.f(c.DECELERATION_FACTOR);
        o.f(c.SIZE_START); o.f(c.SIZE_END); o.f(c.PARTICLE_SIZE_SCALE);
        o.b(c.ROTATION_RANDOM_START); o.f(c.ROTATION_SPEED_MIN); o.f(c.ROTATION_SPEED_MAX); o.b(c.ROTATION_RANDOM_DIR);
        o.f(c.SWIRL_SPEED); o.f(c.SWIRL_PULL); o.f(c.SPIRAL_RADIAL_SPEED); o.i(c.SPIRAL_ARMS); o.f(c.SPIRAL_SPIN);
        o.i(static_cast<int>(c.SHAPE)); o.f(c.SHAPE_RADIUS); o.f(c.SHAPE_HEIGHT); o.f(c.SHAPE_TUBE_RADIUS);
        o.f(c.SHAPE_SIZE_X); o.f(c.SHAPE_SIZE_Y); o.f(c.SHAPE_SIZE_Z); o.b(c.SHAPE_SURFACE_ONLY);
        o.i(static_cast<int>(c.SHAPE_DIRECTION));
        o.i(static_cast<int>(c.ORIENTATION));
        o.f(c.FLUTTER_ANGLE); o.f(c.FLUTTER_SPEED);
        o.f(static_cast<float>(std::hash<std::string>{}(c.SHAPE_MESH) % 1000003u));
    }
}

void UndoManager::captureExtra(Object3D *o, State &s)
{
    Out out{s.extra};

    // Comunes a todo objeto (sección Render del inspector)
    out.b(o->selectable); out.b(o->showAvatar); out.b(o->enableLights);
    const RenderSettings &rs = o->getRenderSettings();
    out.b(rs.blend); out.b(rs.culling); out.b(rs.depthTest); out.b(rs.writeDepth); out.b(rs.shadowMap); out.b(rs.frustumCulling);
    out.i(static_cast<int>(rs.mode_src)); out.i(static_cast<int>(rs.mode_dst));

    if (auto *m = dynamic_cast<Mesh3D *>(o)) {
        out.b(m->renderDefaultPipeline); out.b(m->frustumCullSubmeshes);
        out.b(m->emissionEnabled); out.f(m->emissionIntensity);
        out.i(static_cast<int>(m->emissiveMaterials.size()));
        for (int idx : m->emissiveMaterials) out.i(idx);
    }
    if (auto *lp = dynamic_cast<LightPoint *>(o)) {
        for (int c = 0; c < 4; c++) out.f(lp->ambient[c]);
        for (int c = 0; c < 4; c++) out.f(lp->diffuse[c]);
        for (int c = 0; c < 4; c++) out.f(lp->specular[c]);
        out.f(lp->constant); out.f(lp->linear); out.f(lp->quadratic);
        out.b(lp->frustumCullingEnabled); out.f(lp->frustumCullingOffset);
        if (auto *ls = dynamic_cast<LightSpot *>(lp)) {
            out.f(ls->cutOff); out.f(ls->outerCutOff); out.b(ls->showDebugCone); out.b(ls->castsShadow);
        }
    }
    if (auto *pe = dynamic_cast<ParticleEmitter *>(o)) {
        out.b(pe->stopAdd);
        out.f(pe->colorFrom.r); out.f(pe->colorFrom.g); out.f(pe->colorFrom.b); out.f(pe->colorFrom.a);
        out.f(pe->colorTo.r);   out.f(pe->colorTo.g);   out.f(pe->colorTo.b);   out.f(pe->colorTo.a);
        contextFloats(pe->context, out);
        s.pctx = std::make_shared<ParticlesContext>(pe->context);
    }
    if (auto *so = dynamic_cast<Sound3D *>(o)) {
        out.f(so->innerRadius); out.f(so->outerRadius); out.i(so->baseVolume); out.b(so->loop);
    }
}

void UndoManager::applyExtra(Object3D *o, const State &s)
{
    In in{s.extra};

    o->selectable = in.b(); o->showAvatar = in.b(); o->enableLights = in.b();
    RenderSettings &rs = o->getRenderSettings();
    rs.blend = in.b(); rs.culling = in.b(); rs.depthTest = in.b(); rs.writeDepth = in.b(); rs.shadowMap = in.b(); rs.frustumCulling = in.b();
    rs.mode_src = static_cast<GLenum>(in.i()); rs.mode_dst = static_cast<GLenum>(in.i());

    if (auto *m = dynamic_cast<Mesh3D *>(o)) {
        m->renderDefaultPipeline = in.b(); m->frustumCullSubmeshes = in.b();
        m->emissionEnabled = in.b(); m->emissionIntensity = in.f();
        m->emissiveMaterials.clear();
        for (int n = in.i(); n > 0; n--) m->emissiveMaterials.insert(in.i());
    }
    if (auto *lp = dynamic_cast<LightPoint *>(o)) {
        for (int c = 0; c < 4; c++) lp->ambient[c] = in.f();
        for (int c = 0; c < 4; c++) lp->diffuse[c] = in.f();
        for (int c = 0; c < 4; c++) lp->specular[c] = in.f();
        lp->constant = in.f(); lp->linear = in.f(); lp->quadratic = in.f();
        lp->frustumCullingEnabled = in.b(); lp->frustumCullingOffset = in.f();
        if (auto *ls = dynamic_cast<LightSpot *>(lp)) {
            ls->cutOff = in.f(); ls->outerCutOff = in.f(); ls->showDebugCone = in.b(); ls->castsShadow = in.b();
        }
    }
    if (auto *pe = dynamic_cast<ParticleEmitter *>(o)) {
        pe->stopAdd = in.b();
        const float fr = in.f(), fg = in.f(), fb = in.f(), fa = in.f();
        const float tr = in.f(), tg = in.f(), tb = in.f(), ta = in.f();
        pe->setColorFrom(Color(fr, fg, fb, fa));
        pe->setColorTo(Color(tr, tg, tb, ta));
        if (s.pctx) pe->context = *s.pctx;
    }
    if (auto *so = dynamic_cast<Sound3D *>(o)) {
        so->innerRadius = in.f(); so->outerRadius = in.f(); so->baseVolume = in.i(); so->loop = in.b();
    }
}

bool UndoManager::blocked()
{
    return Components::get()->Scripting()->isExecuting();
}

UndoManager::State UndoManager::capture(Object3D *o)
{
    State s;
    const Vertex3D p = o->getPosition();
    s.pos[0] = p.x; s.pos[1] = p.y; s.pos[2] = p.z;
    const M3 r = o->getRotation();
    std::copy(r.m, r.m + 9, s.rot);
    const Vertex3D sc = o->getScaleV();
    s.scale[0] = sc.x; s.scale[1] = sc.y; s.scale[2] = sc.z;
    s.alpha   = o->getAlpha();
    s.enabled = o->isEnabled();
    captureExtra(o, s);
    return s;
}

bool UndoManager::apply(const std::string &name, const State &s)
{
    Object3D *o = Brakeza::get()->getObjectByName(name);
    if (o == nullptr || o->isRemoved()) return false;

    o->setPosition(Vertex3D(s.pos[0], s.pos[1], s.pos[2]));
    M3 r;
    std::copy(s.rot, s.rot + 9, r.m);
    o->setRotation(r);
    o->setScaleV(Vertex3D(s.scale[0], s.scale[1], s.scale[2]));
    o->setAlpha(s.alpha);
    o->setEnabled(s.enabled);
    applyExtra(o, s);
    if (o->isCollisionsEnabled()) o->UpdateShapeCollider();
    return true;
}

void UndoManager::observe(Object3D *o, Source source, bool editing)
{
    if (o == nullptr || blocked()) {
        activeMask = 0;
        wasEditing = false;
        fieldActive = false;
        baselineName.clear();
        return;
    }

    if (editing) activeMask |= source; else activeMask &= ~static_cast<unsigned>(source);

    if (activeMask != 0) {                       // gesto en curso: el estado de partida no se toca
        if (baselineName != o->getName()) { activeMask = 0; wasEditing = false; return; }   // cambió el objeto a mitad
        wasEditing = true;
        return;
    }

    if (wasEditing && baselineName == o->getName()) {   // el gesto acaba de terminar
        const State after = capture(o);
        if (!(after == baseline)) {
            Entry e;
            e.name = baselineName;
            e.label = "Edit '" + baselineName + "'";
            e.before = baseline;
            e.after = after;
            push(e);
        }
    }
    wasEditing = false;
    baseline = capture(o);
    baselineName = o->getName();
}

void UndoManager::onFieldItem()
{
    if (ImGui::IsItemActivated()) fieldActive = true;
    if (ImGui::IsItemDeactivated()) fieldActive = false;
}

bool UndoManager::isBusy() const
{
    // El plazo evita que una recreación fallida bloquee el undo para siempre
    return !pendingRecreate.empty() && SDL_GetTicks64() - pendingSince <= 5000;
}

void UndoManager::push(const Entry &e)
{
    undoStack.push_back(e);
    redoStack.clear();
    trim();
}

void UndoManager::trim()
{
    auto &max = Config::get()->UNDO_MAX_STEPS;
    max = std::clamp(max, 1, 1000);
    const size_t limit = static_cast<size_t>(max);
    if (undoStack.size() > limit) undoStack.erase(undoStack.begin(), undoStack.end() - static_cast<long>(limit));
    if (redoStack.size() > limit) redoStack.erase(redoStack.begin(), redoStack.end() - static_cast<long>(limit));
}

UndoManager::Snap UndoManager::snapshot(Object3D *o)
{
    Snap s;
    s.name = o->getName();
    s.belongToScene = o->isBelongToScene();
    if (o->getScene() != nullptr) s.sceneName = o->getScene()->getName();
    if (cJSON *json = JSONSerializerRegistry::GetJsonByObject(o)) {
        char *text = cJSON_PrintUnformatted(json);
        if (text != nullptr) { s.json = text; free(text); }
        cJSON_Delete(json);
    }
    return s;
}

void UndoManager::recreate(const Snap &s)
{
    if (s.json.empty()) return;
    if (Object3D *o = Brakeza::get()->getObjectByName(s.name)) {
        if (!o->isRemoved()) return;   // ya está en la escena: no duplicar
    }
    cJSON *json = cJSON_Parse(s.json.c_str());
    if (json == nullptr) return;
    UndoManager &um = get();
    if (!um.isBusy()) um.pendingRecreate.clear();
    um.pendingRecreate.insert(s.name);
    um.pendingSince = SDL_GetTicks64();
    Scene *scene = nullptr;
    if (!s.sceneName.empty()) {
        for (auto *sc : Components::get()->Scripting()->getLoadedScenes()) {
            if (sc->getName() == s.sceneName) { scene = sc; break; }
        }
    }
    Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(
        std::make_shared<ThreadJobLoadObject>(json, scene != nullptr && s.belongToScene, scene)
    );
    cJSON_Delete(json);   // el job guarda su propia copia
}

void UndoManager::removeByName(const std::string &name)
{
    Object3D *o = Brakeza::get()->getObjectByName(name);
    if (o == nullptr || o->isRemoved()) return;
    Components::get()->Render()->clearSelection();
    o->setRemoved(true);
}

void UndoManager::removeObjects(const std::vector<Object3D *> &list)
{
    Entry e;
    e.presence = true;
    e.createdEntry = false;
    for (auto *o : list) {
        if (o == nullptr || o->isRemoved()) continue;
        if (!blocked()) e.objs.push_back(snapshot(o));
        o->setRemoved(true);
    }
    if (e.objs.empty()) return;
    e.label = e.objs.size() == 1 ? "Delete '" + e.objs[0].name + "'" : "Delete " + std::to_string(e.objs.size()) + " objects";
    push(e);
}

void UndoManager::expectCreation()
{
    awaitingCreation = true;
    awaitingSince = SDL_GetTicks64();
}

void UndoManager::onObjectAdded(Object3D *o)
{
    pendingRecreate.erase(o->getName());   // la recreación en curso (si era ésta) ya terminó
    if (!awaitingCreation) return;
    awaitingCreation = false;
    if (blocked() || SDL_GetTicks64() - awaitingSince > 15000) return;   // pasó demasiado tiempo: no es el alta pedida
    Entry e;
    e.presence = true;
    e.createdEntry = true;
    Snap s;
    s.name = o->getName();               // la foto se toma al deshacer, con el estado de ese momento
    e.objs.push_back(s);
    e.label = "Create '" + s.name + "'";
    push(e);
}

bool UndoManager::undo()
{
    if (undoStack.empty() || blocked() || isBusy()) return false;
    Entry e = undoStack.back();
    undoStack.pop_back();
    if (e.presence) {
        if (e.createdEntry) {                       // deshacer un alta: foto y eliminar
            for (auto &s : e.objs) {
                if (Object3D *o = Brakeza::get()->getObjectByName(s.name)) { if (!o->isRemoved()) s = snapshot(o); }
                removeByName(s.name);
            }
        } else {                                    // deshacer un borrado: recrear
            for (auto &s : e.objs) recreate(s);
        }
        redoStack.push_back(e);
    } else if (apply(e.name, e.before)) {
        redoStack.push_back(e);
    }
    baselineName.clear();                        // el próximo observe recoge el estado nuevo
    return true;
}

bool UndoManager::redo()
{
    if (redoStack.empty() || blocked() || isBusy()) return false;
    Entry e = redoStack.back();
    redoStack.pop_back();
    if (e.presence) {
        if (e.createdEntry) { for (auto &s : e.objs) recreate(s); }
        else                { for (auto &s : e.objs) removeByName(s.name); }
        undoStack.push_back(e);
    } else if (apply(e.name, e.after)) {
        undoStack.push_back(e);
    }
    baselineName.clear();
    return true;
}

void UndoManager::clear()
{
    undoStack.clear();
    redoStack.clear();
    baselineName.clear();
    activeMask = 0;
    wasEditing = false;
    fieldActive = false;
    awaitingCreation = false;
    pendingRecreate.clear();
}
