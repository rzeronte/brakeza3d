#include "../../include/LUA/BrakezaLuaBridge.h"

void LUAIntegrationMeshes(sol::state &lua)
{
    lua.new_usertype<Mesh3D>("Mesh3D",
    sol::base_classes, sol::bases<Object3D>(),
        "AssimpLoadGeometryFromFile", &Mesh3D::AssimpLoadGeometryFromFile,
        "BuildGrid3D", &Mesh3D::BuildGrid3D,
        "BuildOctree", &Mesh3D::BuildOctree,
        "FillGrid3DFromGeometry", &Mesh3D::FillGrid3DFromGeometry,
        "getOctree", &Mesh3D::getOctree,
        "getGrid3D", &Mesh3D::getGrid3D,
        "isRenderPipelineDefault", &Mesh3D::isRenderPipelineDefault,
        "setRenderPipelineDefault", &Mesh3D::setRenderPipelineDefault,
        // Ruta del .fbx con el que se creó el objeto (válida nada más crearlo, antes de que termine
        // la carga asíncrona) -- p.ej. RiderAttachment.lua clona el modelo de una unidad montada.
        "getModelFile", [](Mesh3D& m) -> std::string { return m.getModelFile(); },
        "isMipmaps", &Mesh3D::isMipmaps,
        "setMipmaps", &Mesh3D::setMipmaps,
        "isEmissionEnabled", &Mesh3D::isEmissionEnabled,
        "setEmissionEnabled", &Mesh3D::setEmissionEnabled,
        "getEmissionIntensity", &Mesh3D::getEmissionIntensity,
        "setEmissionIntensity", &Mesh3D::setEmissionIntensity,
        // Emision por material: con alguno marcado, solo las submeshes de esos materiales emiten (si la emision del mesh esta activa).
        "setMaterialEmissive", &Mesh3D::setMaterialEmissive,
        "isMaterialEmissive", [](Mesh3D& m, int idx) -> bool { return m.getEmissiveMaterials().count(idx) > 0; },
        "setEmissiveByTexture", &Mesh3D::setMaterialsEmissiveByTexture,     // (patron, bool) -> n materiales marcados
        "clearEmissiveMaterials", [](Mesh3D& m) { for (int i : std::set<int>(m.getEmissiveMaterials())) m.setMaterialEmissive(i, false); },
        "getMeshCount", [](Mesh3D& m) -> int {
            return (int)m.getModelTextures().size();
        },
        "setDiffuseTexture", [](Mesh3D& m, int meshIdx, const std::string& path) {
            auto& textures = m.getModelTextures();
            if (meshIdx < 0 || meshIdx >= (int)textures.size()) return;
            textures[meshIdx] = imageCache.getOrLoad(FilePath::ImageFile(path));
        },
        "getDiffuseTextureFile", [](Mesh3D& m, int meshIdx) -> std::string {
            auto& textures = m.getModelTextures();
            if (meshIdx < 0 || meshIdx >= (int)textures.size() || textures[meshIdx] == nullptr) return "";
            return std::string(textures[meshIdx]->getFileName().c_str());
        }
    );

    lua.new_usertype<Mesh3DAnimation>("Mesh3DAnimation",
    sol::base_classes, sol::bases<Mesh3D, Object3D>(),
        "setIndexCurrentAnimation", &Mesh3DAnimation::setIndexCurrentAnimation,
        "setAnimationByName", &Mesh3DAnimation::setAnimationByName,
        "setAnimationSpeed", &Mesh3DAnimation::setAnimationSpeed,
        "isAnimationEnds", &Mesh3DAnimation::isAnimationEnds,
        "restartAnimation", &Mesh3DAnimation::restartAnimation,
        "getAnimationDuration", &Mesh3DAnimation::getAnimationDuration,
        "setLoop", &Mesh3DAnimation::setLoop,
        "isLoop", &Mesh3DAnimation::isLoop,
        "getNumAnimations", &Mesh3DAnimation::getNumAnimations,
        "getAnimationName", &Mesh3DAnimation::getAnimationName,
        "getBoneWorldPosition", &Mesh3DAnimation::getBoneWorldPosition,
        "getBoneWorldRotation", &Mesh3DAnimation::getBoneWorldRotation,
        "getBoneNames", &Mesh3DAnimation::getBoneNames,
        // Dibuja el esqueleto (líneas amarillas entre huesos) de todas las mallas animadas; es el mismo interruptor
        // que el menú "Draw Animation Bones"
        "setDrawBones", [](Mesh3DAnimation*, bool v) { Config::get()->DRAW_ANIMATION_BONES = v; },
        "isDrawBones",  [](Mesh3DAnimation*) { return Config::get()->DRAW_ANIMATION_BONES; }
    );

    lua.new_usertype<Projectile>("Projectile",
    sol::base_classes, sol::bases<Mesh3D, Object3D>(),
        "fire", [](Projectile* p, const Vertex3D& dir, float impulse, float accuracy, float shapeRadius, int collisionGroup, int collisionMask) {
            p->setCollisionsEnabled(true);
            float r = (shapeRadius > 0.001f) ? shapeRadius : 0.1f;
            p->setSimpleShapeSize(Vertex3D(r, r, r));
            p->setCollisionShape(SIMPLE_SHAPE);
            p->setCollisionMode(CollisionMode::BODY);
            p->setLinearDamping(0.0f);
            p->setAngularDamping(0.0f);
            p->setCcdMotionThreshold(0.05f);
            p->setCcdSweptSphereRadius(r);
            p->makeProjectileRigidBody(
                1.0f,
                dir,
                impulse,
                accuracy,
                Components::get()->Collisions()->getDynamicsWorld(),
                SIMPLE_SHAPE,
                collisionGroup,
                collisionMask
            );
        },
        "getDirection", &Projectile::getDirection,
        "setDirection", &Projectile::setDirection
    );

    lua.new_usertype<Image3DAnimation>("BillboardAnimation",
    sol::base_classes, sol::bases<Object3D>(),
        "CreateAnimation", &Image3DAnimation::CreateAnimation,
        "setAnimation", &Image3DAnimation::setAnimation,
        // pausa la animación y fija el frame (sincronizar un sprite con otra magnitud, p.ej. el giro de la baliza)
        "setFrame", &Image3DAnimation::setFrame,
        "UpdateBillboardSize", &Image3DAnimation::UpdateBillboardSize,
        "setTowardsCamera", &Image3DAnimation::setTowardsCamera,
        "setSize", &Image3DAnimation::setSize,
        "setRenderFB", &Image3DAnimation::setRenderFB,
        // true: el objeto se elimina solo al terminar UNA pasada de la animación (WeaponFX, sprites de un solo uso)
        "setAutoRemoveAfterAnimation", &Image3DAnimation::setAutoRemoveAfterAnimation
    );

    lua.new_usertype<Image3DAnimation360>("BillboardAnimation8Directions",
        sol::base_classes, sol::bases<Object3D>()
    );

    lua.new_usertype<ScriptLUATypeData>("ScriptLUATypeData",
    "name", &ScriptLUATypeData::name,
        "value", &ScriptLUATypeData::value
    );

    lua.new_usertype<Scene>("Scene",
        "getName",    &Scene::getName,
        "getFilePath",&Scene::getFilePath,
        "isActive",   &Scene::isActive,
        "setActive",  &Scene::setActive,
        "getObjects", [](Scene* s, sol::this_state st) -> sol::table {
            sol::state_view lua(st);
            sol::table result = lua.create_table();
            int i = 1;
            for (auto *obj : s->getObjects()) result[i++] = obj;
            return result;
        }
    );

    lua.new_usertype<SceneLoader>("SceneLoader",
        "clearWorld", &SceneLoader::ClearWorld,
        "cleanWorld", &SceneLoader::CleanWorld,
        "setSceneActive", [](SceneLoader*, std::string name, bool active) {
            SceneLoader::setSceneActive(name, active);
        },
        "setSceneHidden", [](SceneLoader*, std::string name, bool hidden) {
            SceneLoader::setSceneHidden(name, hidden);
        },
        "unloadScene", [](SceneLoader*, std::string name) {
            SceneLoader::UnloadScene(name);
        },
        "reloadScene", [](SceneLoader*, std::string name) {
            SceneLoader::ReloadScene(name);
        },
        "SaveScene", [](SceneLoader*, std::string path) {
            SceneLoader::SaveScene(FilePath::SceneFile(path));
        },
        "LoadScene", [](SceneLoader*, std::string path) {
            SceneLoader::LoadScene(FilePath::SceneFile(path));
        },
        "loadSceneAdditive", [](SceneLoader*, std::string path,
                                sol::optional<bool> scripts,
                                sol::optional<bool> shaders,
                                sol::optional<bool> camera,
                                sol::optional<bool> renderSettings,
                                sol::optional<bool> startScripts) {
            SceneLoader::LoadSceneAdditive(
                FilePath::SceneFile(path),
                scripts.value_or(false),
                shaders.value_or(false),
                camera.value_or(false),
                renderSettings.value_or(false),
                startScripts.value_or(false)
            );
        },
        "loadSceneADS", [](SceneLoader*, std::string name) {
            auto *scene = Components::get()->Scripting()->getSceneByName(name);
            if (!scene) return;
            const std::string &path = scene->getFilePath();
            FILE *f = fopen(path.c_str(), "rb");
            if (!f) return;
            fseek(f, 0, SEEK_END);
            long sz = ftell(f);
            rewind(f);
            std::string buf(sz, '\0');
            fread(buf.data(), 1, sz, f);
            fclose(f);
            cJSON *root = cJSON_Parse(buf.c_str());
            if (!root) return;
            SceneLoader::LoadADSSettings(root);
            cJSON_Delete(root);
        }
    );

    lua.new_usertype<Image2D>("Image2D",
    sol::base_classes, sol::bases<Object3D>(),
        "setScreenPosition",    &Image2D::setScreenPosition,
        "setWindowPosition",    &Image2D::setWindowPosition,
        "setSize",              &Image2D::setSize,
        "setFilePath",          &Image2D::setFilePath,
        "loadVideo",            &Image2D::loadVideo,
        "stopVideo",            &Image2D::stopVideo,
        "isVideoFinished",      &Image2D::isVideoFinished,
        "hasVideo",             &Image2D::hasVideo,
        "setDrawInBackground",  &Image2D::setDrawInBackground,
        "isDrawInBackground",   &Image2D::isDrawInBackground
    );

    lua.new_usertype<Image2DAnimation>("Image2DAnimation",
    sol::base_classes, sol::bases<Object3D>(),
        "updatePosition", &Image2DAnimation::updatePosition
    );

    lua.new_usertype<Image3D>("Image3D",
       sol::base_classes, sol::bases<Object3D>()
    );

    lua.new_usertype<LightPoint>("LightPoint3D",
    sol::base_classes, sol::bases<Object3D>(),
        "setConstant", &LightPoint::setConstant,
        "setLinear", &LightPoint::setLinear,
        "setCuadratic", &LightPoint::setCuadratic,
        "setColor", &LightPoint::setColor,
        "setColorSpecular", &LightPoint::setColorSpecular,
        "setAmbient", &LightPoint::setAmbient,
        "setFrustumCullingEnabled", &LightPoint::setFrustumCullingEnabled,
        "setFrustumCullingOffset",  &LightPoint::setFrustumCullingOffset,
        "getFrustumCullingEnabled", &LightPoint::getFrustumCullingEnabled,
        "getFrustumCullingOffset",  &LightPoint::getFrustumCullingOffset
    );

    lua.new_usertype<LightSpot>("SpotLight3D",
    sol::base_classes, sol::bases<LightPoint, Object3D>(),
        "setCutOff", &LightSpot::setCutOff,
        "setOuterCutOff", &LightSpot::setOuterCutOff,
        "setCastsShadow", &LightSpot::setCastsShadow,
        "getCastsShadow", &LightSpot::getCastsShadow
    );

}
