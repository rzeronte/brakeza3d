#include "../../include/LUA/BrakezaLuaBridge.h"

void LUAIntegrationBasics(sol::state &lua)
{
    lua.new_usertype<Vertex3D>(
        "Vertex3D",
        sol::constructors<Vertex3D(), Vertex3D(float, float, float)>(),
        "x", &Vertex3D::x,
        "y", &Vertex3D::y,
        "z", &Vertex3D::z,
        "__add", sol::overload(&Vertex3D::operator+),
        "__sub", sol::overload(&Vertex3D::operator-),
        "getScaled", sol::overload(
            static_cast<Vertex3D (Vertex3D::*)(float) const>(&Vertex3D::getScaled),
            static_cast<Vertex3D (Vertex3D::*)(float, float, float) const>(&Vertex3D::getScaled)
        ),
        "getNormalize", &Vertex3D::getNormalize,
        "getModule", &Vertex3D::getModule
    );

    lua.new_usertype<M3>(
        "M3",
        sol::constructors<M3()>(),
        "__mul", sol::overload(
            sol::resolve<Vertex3D(Vertex3D)>(&M3::operator*),
            sol::resolve<M3(const M3&)>(&M3::operator*)
        ),
        "getMatrixRotationForEulerAngles", &M3::getMatrixRotationForEulerAngles,
        "RX", &M3::RX,
        "RY", &M3::RY,
        "RZ", &M3::RZ,
        "X", &M3::X,
        "Y", &M3::Y,
        "Z", &M3::Z,
        "getTranspose",  &M3::getTranspose,
        "setMatrixRotationForEulerAngles", &M3::setMatrixRotationForEulerAngles,
        "getMatrixIdentity", &M3::getMatrixIdentity
    );

    lua.new_usertype<Color>(
        "Color",
        sol::constructors<Color(),
            Color(float, float, float, float)>(),
        "setRed", &Color::setRed,
        "setGreen", &Color::setGreen,
        "setBlue", &Color::setBlue
    );

    lua.new_usertype<Object3D>(
        "Object3D",
        "addToPosition", &Object3D::AddToPosition,
        // getPosition/setPosition/setRotation blindados con isValidObjectPointer() por el mismo
        // motivo que setBelongToScene/setEnabled/etc. (ver comentario grande más abajo): un
        // script Lua que cachea un Object3D* (patrón eGetObj de PoliceAI.lua/FactionAI.lua) y lo
        // sigue leyendo/escribiendo cada frame en un bucle de patrulla/movimiento expone MUCHO
        // más una ventana de puntero colgante que una llamada puntual -- getPosition() se
        // convierte de referencia a valor (Vertex3D por copia) para poder devolver un Vertex3D()
        // por defecto sin desreferenciar nada cuando el puntero es inválido.
        "getPosition", [](Object3D* obj) -> Vertex3D {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] getPosition() llamado sobre un objeto invalido (Lua)"); return Vertex3D(); }
            return obj->getPosition();
        },
        "setPosition", [](Object3D* obj, const Vertex3D& p) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setPosition() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setPosition(p);
        },
        // Lo mismo con números sueltos (2026-10-10): sin crear un Vertex3D por llamada. Para bucles
        // que leen/mueven cientos de objetos por frame (unidades, tráfico) -- cada userdata creado
        // en Lua cuesta más que la propia lectura/escritura y alimenta al recolector de basura.
        "getPositionXYZ", [](Object3D* obj) -> std::tuple<float, float, float> {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] getPositionXYZ() llamado sobre un objeto invalido (Lua)"); return {0.0f, 0.0f, 0.0f}; }
            const Vertex3D& p = obj->getPosition();
            return {p.x, p.y, p.z};
        },
        "setPositionXYZ", [](Object3D* obj, float x, float y, float z) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setPositionXYZ() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setPosition(Vertex3D(x, y, z));
        },
        "setRotation", [](Object3D* obj, const M3& r) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setRotation() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setRotation(r);
        },
        "getTypeObject", &Object3D::getTypeObject,
        "getName", &Object3D::getName,
        "getId", &Object3D::getId,
        // Índice espacial (Spatial.*): máscara de bits de la lógica de juego, 0 = fuera del índice
        "setSpatialMask", [](Object3D* obj, double mask) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setSpatialMask() llamado sobre un objeto invalido (Lua)"); return; }
            Brakeza::get()->getSpatialIndex().setMask(obj, (uint32_t) mask);
        },
        "getSpatialMask", [](Object3D* obj) -> double {
            if (!Brakeza::get()->isValidObjectPointer(obj)) return 0.0;
            return (double) obj->getSpatialMask();
        },
        "getRotation", &Object3D::getRotation,
        // setBelongToScene/setEnabled/setRemoved/setSelectable/setAlphaEnabled/setEnableLights
        // van todos via lambda con Object3D* + Brakeza::isValidObjectPointer(), NO puntero a
        // miembro directo (&Object3D::metodo). Motivo: el binding por puntero a miembro no
        // valida el "this" en absoluto -- y un simple obj==nullptr NO basta, porque un script
        // Lua puede guardar un Object3D* que sobrevive a un reinicio de partida (removeAllObjects
        // + delete diferido): el puntero NO es null, apunta a memoria ya liberada (y quiza
        // reutilizada por otro objeto), así que == nullptr no lo detecta y ->isRemoved() sobre
        // el mismo puntero desreferencia esa memoria -> el propio crash que se intenta evitar.
        // isValidObjectPointer() solo compara el VALOR del puntero contra el registro de
        // objetos vivos (Object3D::Object3D()/~Object3D()), nunca lo desreferencia -- es la
        // UNICA comprobacion segura para un puntero que puede ser colgante. Ademas, TODOS los
        // metodos con la firma exacta void(Object3D::*)(bool) comparten la misma instanciacion
        // de plantilla de sol2 (sol::member_function_wrapper<void (Object3D::*)(bool),...>) y
        // el compilador los pliega en el mismo bloque de codigo (identical code folding) --
        // adr2line solo puede apuntar a UNO de ellos, así que hay que blindarlos TODOS o el
        // crash simplemente reaparece en el siguiente metodo sin blindar. Confirmado: probar
        // Object3D& (esperando que sol2 rechazase un self nulo en la conversion a referencia)
        // NO funciona para el "self" implicito de una llamada a metodo (obj:metodo()) -- sol2
        // no aplica esa validacion ahi. Solo un puntero + chequeo propio es fiable.
        "setBelongToScene", [](Object3D* obj, bool v) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setBelongToScene() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setBelongToScene(v);
        },
        "setEnabled", [](Object3D* obj, bool v) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setEnabled() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setEnabled(v);
        },
        "setRemoved", [](Object3D* obj, bool v) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setRemoved() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setRemoved(v);
        },
        // isRemoved/getLocalScriptVar/setLocalScriptVar iban por puntero a miembro directo (sin
        // pasar por isValidObjectPointer()) -- mismo problema que setBelongToScene/setEnabled/etc
        // de arriba: un Object3D* colgante (sobrevive a un reinicio, memoria liberada y quizá
        // reutilizada) NO es nullptr, así que ->isRemoved()/->getLocalScriptVar() desreferencian
        // esa memoria igualmente. Crash real reproducido: Object3D::getLocalScriptVar (Object3D.h
        // :123) leyendo luaEnvironment ya corrupto -> sol::basic_reference::push() -> access
        // violation en lua_rawgeti (ver debug-native-crashes.md, técnica addr2line + slide).
        "isRemoved", [](Object3D* obj) -> bool {
            if (!Brakeza::get()->isValidObjectPointer(obj)) return true;
            return obj->isRemoved();
        },
        // Oculto solo en render (niebla de guerra): sus scripts y colisiones siguen funcionando
        "setRenderHidden", [](Object3D* obj, bool v) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setRenderHidden() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setRenderHidden(v);
        },
        "isRenderHidden", [](Object3D* obj) -> bool {
            if (!Brakeza::get()->isValidObjectPointer(obj)) return false;
            return obj->isRenderHidden();
        },
        "setName", &Object3D::setName,
        "getScale", &Object3D::getScale,
        "setScale", &Object3D::setScale,
        "getScaleV", &Object3D::getScaleV,
        "setScaleV", &Object3D::setScaleV,
        "getModelMatrix", &Object3D::getModelMatrix,
        "getM3ModelMatrix", &Object3D::getM3ModelMatrix,
        "AxisForward", &Object3D::forward,
        "AxisUp", &Object3D::up,
        "AxisRight", &Object3D::right,
        "RemoveCollisionObject", [](Object3D* obj) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] RemoveCollisionObject() llamado sobre un objeto invalido (Lua)"); return; }
            obj->RemoveCollisionObject();
        },
        "SleepCollider", &Object3D::SleepCollider,
        "setCollisionsEnabled", [](Object3D* obj, bool v) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setCollisionsEnabled() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setCollisionsEnabled(v);
        },
        "setCollisionGroupMask", [](Object3D* o, int group, int mask) { o->setCollisionGroupMask(group, mask); },
        "DisableSimulationCollider", &Object3D::DisableSimulationCollider,
        "EnableSimulationCollider", &Object3D::EnableSimulationCollider,
        "DisableDeactivationCollider", &Object3D::DisableDeactivationCollider,
        "UpdateShapeCollider", &Object3D::UpdateShapeCollider,
        "setScalingCollider", &Object3D::setScalingCollider,
        "setSimpleShapeSize", [](Object3D* o, const Vertex3D& size) { o->setSimpleShapeSize(size); },
        "setCapsuleColliderSize", &Object3D::setCapsuleColliderSize,
        "moveCollider", &Object3D::moveCollider,
        "isCollisionsEnabled", [](Object3D* obj) -> bool {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] isCollisionsEnabled() llamado sobre un objeto invalido (Lua)"); return false; }
            return obj->isCollisionsEnabled();
        },
        "setDrawOffset", &Object3D::setDrawOffset,
        "setupGhostCollider", &Object3D::SetupGhostCollider,
        "SetupRigidBodyCollider", &Object3D::SetupRigidBodyCollider,
        "setColliderStatic", &Object3D::setColliderStatic,
        "setDebugDraw", &Object3D::setDebugDraw,
        "isDebugDraw", &Object3D::isDebugDraw,
        "resetColliderRotation", &Object3D::resetColliderRotation,
        "ApplyCentralForce", &Object3D::ApplyCentralForce,
        "ApplyCentralImpulse", &Object3D::ApplyCentralImpulse,
        "ApplyImpulse", &Object3D::ApplyImpulse,
        "setLinearVelocity", &Object3D::setLinearVelocity,
        "setAngularVelocity", &Object3D::setAngularVelocity,
        "setFriction", &Object3D::setFriction,
        "setRestitution", &Object3D::setRestitution,
        "setAngularDamping", &Object3D::setAngularDamping,
        "setLinearDamping", &Object3D::setLinearDamping,
        "setAngularFactor", &Object3D::setAngularFactor,
        "setLinearFactor",  &Object3D::setLinearFactor,
        "setMass", &Object3D::setMass,
        "setGravityCollider", &Object3D::setGravityCollider,
        "setWalkingDirection", &Object3D::setWalkingDirection,
        "Jump", &Object3D::Jump,
        "onGround", &Object3D::onGround,
        "getLocalScriptVar",  [](Object3D* obj, const char *varName) -> sol::object {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] getLocalScriptVar() llamado sobre un objeto invalido (Lua)"); return sol::lua_nil; }
            return obj->getLocalScriptVar(varName);
        },
        "setLocalScriptVar",  [](Object3D* obj, const char *varName, sol::object value) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setLocalScriptVar() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setLocalScriptVar(varName, value);
        },
        // Variables del script con tipo (Object3D.h): sin sol::object ni conversión posterior en Lua. nil si no existe o no es del tipo.
        "getScriptNumber", [](Object3D* obj, const char *varName) -> sol::optional<double> {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] getScriptNumber() llamado sobre un objeto invalido (Lua)"); return sol::nullopt; }
            return obj->getScriptNumber(varName);
        },
        "getScriptString", [](Object3D* obj, const char *varName) -> sol::optional<std::string> {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] getScriptString() llamado sobre un objeto invalido (Lua)"); return sol::nullopt; }
            return obj->getScriptString(varName);
        },
        "getScriptBool", [](Object3D* obj, const char *varName) -> sol::optional<bool> {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] getScriptBool() llamado sobre un objeto invalido (Lua)"); return sol::nullopt; }
            return obj->getScriptBool(varName);
        },
        "setScriptNumber", [](Object3D* obj, const char *varName, double value) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setScriptNumber() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setScriptNumber(varName, value);
        },
        "setScriptString", [](Object3D* obj, const char *varName, const std::string &value) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setScriptString() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setScriptString(varName, value);
        },
        "setScriptBool", [](Object3D* obj, const char *varName, bool value) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setScriptBool() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setScriptBool(varName, value);
        },
        "isSelectable",       &Object3D::isSelectable,
        "setSelectable", [](Object3D* obj, bool v) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setSelectable() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setSelectable(v);
        },
        "setHighlight",       &Object3D::setHighlight,
        "clearHighlight",     &Object3D::clearHighlight,
        "AttachScript", &Object3D::AttachScript,
        "AttachScriptAsync", [](Object3D* obj, const std::string& jsonPath) {
            Brakeza::get()->PoolCompute().enqueueWithMainThreadCallback(
                std::make_shared<ThreadJobReadObjectScript>(obj, jsonPath)
            );
        },
        "LookAt", sol::overload(
            static_cast<void(Object3D::*)(Object3D*)>(&Object3D::LookAt),
            static_cast<void(Object3D::*)(const Vertex3D&)>(&Object3D::LookAt)
        ),
        "ReloadScriptsEnvironment", &Object3D::ReloadScriptsEnvironment,
        "getLinearVelocity", &Object3D::getLinearVelocity,
        "setParent", &Object3D::setParent,
        "getParent", &Object3D::getParent,
        "AttachObject", &Object3D::AttachObject,
        "getAttached", &Object3D::getAttached,
        "getAlpha", &Object3D::getAlpha,
        "setAlpha", &Object3D::setAlpha,
        "isAlphaEnabled", &Object3D::isAlphaEnabled,
        "setAlphaEnabled", [](Object3D* obj, bool v) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setAlphaEnabled() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setAlphaEnabled(v);
        },
        "isEnableLights", &Object3D::isEnableLights,
        "setEnableLights", [](Object3D* obj, bool v) {
            if (!Brakeza::get()->isValidObjectPointer(obj)) { LOG_ERROR("[Object3D] setEnableLights() llamado sobre un objeto invalido (Lua)"); return; }
            obj->setEnableLights(v);
        },
        "isEnabled", &Object3D::isEnabled,
        "setCcdMotionThreshold", &Object3D::setCcdMotionThreshold,
        "setCcdSweptSphereRadius", &Object3D::setCcdSweptSphereRadius,
        "setShapeMargin", &Object3D::setShapeMargin,
        "setFrustumCulling", [](Object3D* o, bool v) { o->getRenderSettings().frustumCulling = v; },
        "setShadowMap",      [](Object3D* o, bool v) { o->getRenderSettings().shadowMap = v; },
        "isVisibleInFrustum", &Object3D::isVisibleInFrustum,
        "setScene", [](Object3D* o, const std::string& sceneName) {
            auto* scene = Components::get()->Scripting()->getSceneByName(sceneName);
            if (scene) {
                o->setScene(scene);
                scene->addObject(o);
            }
        },
        "getBoneWorldPosition", [](Object3D* o, const std::string& name) -> Vertex3D {
            auto* anim = dynamic_cast<Mesh3DAnimation*>(o);
            return anim ? anim->getBoneWorldPosition(name) : o->getPosition();
        },
        "getBoneWorldRotation", [](Object3D* o, const std::string& name) -> M3 {
            auto* anim = dynamic_cast<Mesh3DAnimation*>(o);
            return anim ? anim->getBoneWorldRotation(name) : o->getRotation();
        },
        "getBoneNames", [](Object3D* o) -> std::vector<std::string> {
            auto* anim = dynamic_cast<Mesh3DAnimation*>(o);
            return anim ? anim->getBoneNames() : std::vector<std::string>{};
        }
    );

}
