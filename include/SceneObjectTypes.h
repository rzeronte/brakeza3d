//
// Created by Eduardo on 25/11/2025.
//

#ifndef BRAKEZA3D_SCENEOBJECTTYPES_H
#define BRAKEZA3D_SCENEOBJECTTYPES_H

#include <cstring>

enum class SceneObjectLoaderMapping {
    Object3D,
    Mesh3D,
    Mesh3DAnimation,
    LightPoint3D,
    SpotLight3D,
    ParticleEmitter,
    BillboardAnimation,
    BillboardAnimation8Directions,
    Image2DAnimation,
    Image3D,
    Image2D
};

// IMPORTANT: The order must match EXACTLY the order of the enum
namespace SceneObjectTypes {
    inline constexpr const char* OBJECT_3D = "Object3D";
    inline constexpr const char* MESH_3D = "Mesh3D";
    inline constexpr const char* MESH_3D_ANIMATION = "Mesh3DAnimation";
    inline constexpr const char* LIGHT_POINT = "LightPoint3D";
    inline constexpr const char* LIGHT_SPOT = "SpotLight3D";
    inline constexpr const char* PARTICLE_EMITTER = "ParticleEmitter";
    inline constexpr const char* IMAGE_3D_ANIMATION = "BillboardAnimation";
    inline constexpr const char* IMAGE_3D_ANIMATION_8DIR = "BillboardAnimation8Directions";
    inline constexpr const char* IMAGE_2D_ANIMATION = "Image2DAnimation";
    inline constexpr const char* IMAGE_3D = "Image3D";
    inline constexpr const char* IMAGE_2D = "Image2D";
    inline constexpr const char* SWARM = "Swarm";
}

static constexpr const char* SceneObjectTypeNames[] = {
    SceneObjectTypes::OBJECT_3D,
    SceneObjectTypes::MESH_3D,
    SceneObjectTypes::MESH_3D_ANIMATION,
    SceneObjectTypes::LIGHT_POINT,
    SceneObjectTypes::LIGHT_SPOT,
    SceneObjectTypes::PARTICLE_EMITTER,
    SceneObjectTypes::IMAGE_3D_ANIMATION,
    SceneObjectTypes::IMAGE_3D_ANIMATION_8DIR,
    SceneObjectTypes::IMAGE_2D_ANIMATION,
    SceneObjectTypes::IMAGE_3D,
    SceneObjectTypes::IMAGE_2D,
    SceneObjectTypes::SWARM
};

namespace IconsByObject {
    inline constexpr const char* OBJECT_3D = "objectIcon";
    inline constexpr const char* MESH_3D = "meshIcon";
    inline constexpr const char* MESH_3D_ANIMATION = "Mesh3DAnimationIcon";
    inline constexpr const char* LIGHT_POINT = "lightIcon";
    inline constexpr const char* LIGHT_SPOT = "spotLightIcon";
    inline constexpr const char* PARTICLE_EMITTER = "particlesIcon";
    inline constexpr const char* IMAGE_3D_ANIMATION = "BillboardAnimationIcon";
    inline constexpr const char* IMAGE_3D_ANIMATION_8DIR = "BillboardAnimation8DirectionsIcon";
    inline constexpr const char* IMAGE_2D_ANIMATION = "Image2DAnimation";
    inline constexpr const char* IMAGE_3D = "Image3D";
    inline constexpr const char* IMAGE_2D = "Image2DIcon";
    inline constexpr const char* SWARM = "swarmIcon";

    struct IconMapping {
        const char* id;
        const char* filename;
    };

    inline constexpr IconMapping ICON_FILES[] = {
        {OBJECT_3D, "object.png"},
        {MESH_3D, "mesh.png"},
        {MESH_3D_ANIMATION, "Mesh3DAnimationIcon.png"},
        {LIGHT_POINT, "light.png"},
        {LIGHT_SPOT, "spotlight.png"},
        {PARTICLE_EMITTER, "particles.png"},
        {IMAGE_3D_ANIMATION, "BillboardAnimationIcon.png"},
        {IMAGE_3D_ANIMATION_8DIR, "BillboardAnimation8DirectionsIcon.png"},
        {IMAGE_2D_ANIMATION, "Image2DAnimationIcon.png"},
        {IMAGE_3D, "Image3DIcon.png"},
        {IMAGE_2D, "Image2DIcon.png"},
        {SWARM, "swarm.png"}
    };

    inline const char* Filename(const char* iconId) {
        for (const auto& icon : ICON_FILES) {
            if (std::strcmp(icon.id, iconId) == 0) {
                return icon.filename;
            }
        }
        return nullptr;
    }
}

inline const char* getSceneObjectTypeName(SceneObjectLoaderMapping type) {
    return SceneObjectTypeNames[static_cast<size_t>(type)];
}

namespace IconsByGUI {
    inline constexpr const char* FOLDER = "folder";
    inline constexpr const char* PLAY = "play";
    inline constexpr const char* STOP = "stop";
    inline constexpr const char* RELOAD = "reload";
    inline constexpr const char* REMOVE = "remove";
    inline constexpr const char* PAUSE = "pause";
    inline constexpr const char* LOCK = "lock";
    inline constexpr const char* UNLOCK = "unlock";
    inline constexpr const char* ADD = "add";
    inline constexpr const char* SCENE = "scene";
    inline constexpr const char* SAVE = "save";
    inline constexpr const char* GEAR = "gear";
    inline constexpr const char* GHOST = "ghost";
    inline constexpr const char* SHADER = "shader";
    inline constexpr const char* SPOTLIGHT = "spotlight";
    inline constexpr const char* PARTICLES = "particles";
    inline constexpr const char* GRAVITY = "gravity";
    inline constexpr const char* CLICK = "click";
    inline constexpr const char* DRAW_COLLIDERS = "drawColliders";
    inline constexpr const char* TARGET = "target";
    inline constexpr const char* MOUSE_LOOK = "mouseLook";
    inline constexpr const char* LAYOUT_DEFAULT = "layoutDefault";
    inline constexpr const char* LAYOUT_CODING = "layoutCoding";
    inline constexpr const char* LAYOUT_DESIGN = "layoutDesign";
    inline constexpr const char* PROJECT = "project";
    inline constexpr const char* OPEN = "open";
    inline constexpr const char* GUI = "gui";
    inline constexpr const char* TEXTURE = "texture";
    inline constexpr const char* TRANSLATE = "translate";
    inline constexpr const char* ROTATE = "rotate";
    inline constexpr const char* SCALE = "scale";
    inline constexpr const char* SCRIPT = "script";
    inline constexpr const char* PLAYER = "player";
    inline constexpr const char* REBUILD = "rebuild";
    inline constexpr const char* SPLASH = "splash";
    inline constexpr const char* LIGHT_SYSTEM = "lightSystem";
    inline constexpr const char* SHADOW_MAPPING = "shadowMapping";
    inline constexpr const char* RENDER_TEXTURE = "renderTexture";
    inline constexpr const char* RENDER_PIXELS = "renderPixels";
    inline constexpr const char* RENDER_SHADING = "renderShading";
    inline constexpr const char* RENDER_WIRE = "renderWire";

    struct IconMapping {
        const char* id;
        const char* filename;
    };

    inline constexpr IconMapping ICON_FILES[] = {
        {FOLDER, "folder.png"},
        {PLAY, "play.png"},
        {STOP, "stop.png"},
        {RELOAD, "reload.png"},
        {REMOVE, "remove.png"},
        {PAUSE, "pause.png"},
        {LOCK, "lock.png"},
        {UNLOCK, "unlock.png"},
        {ADD, "add.png"},
        {SCENE, "scene.png"},
        {SAVE, "save.png"},
        {GEAR, "gear.png"},
        {GHOST, "ghost.png"},
        {SHADER, "shader.png"},
        {SPOTLIGHT, "spotlight.png"},
        {PARTICLES, "particles.png"},
        {GRAVITY, "gravity.png"},
        {CLICK, "click.png"},
        {DRAW_COLLIDERS, "DrawColliders.png"},
        {TARGET, "target.png"},
        {MOUSE_LOOK, "mouseIcon.png"},
        {LAYOUT_DEFAULT, "layoutDefault.png"},
        {LAYOUT_CODING, "layoutCoding.png"},
        {LAYOUT_DESIGN, "layoutDesign.png"},
        {PROJECT, "project.png"},
        {OPEN, "open.png"},
        {GUI, "gui.png"},
        {TEXTURE, "texture.png"},
        {TRANSLATE, "translate.png"},
        {ROTATE, "rotate.png"},
        {SCALE, "scale.png"},
        {SCRIPT, "script.png"},
        {PLAYER, "player.png"},
        {REBUILD, "rebuild.png"},
        {SPLASH, "splash.png"},
        {LIGHT_SYSTEM, "sun.png"},
        {RENDER_TEXTURE, "render_texture.png"},
        {SHADOW_MAPPING, "shadow_mapping.png"},
        {RENDER_PIXELS, "render_pixels.png"},
        {RENDER_SHADING, "render_shading.png"},
        {RENDER_WIRE, "render_wire.png"},
    };

    inline const char* Filename(const char* iconId) {
        for (const auto& icon : ICON_FILES) {
            if (std::strcmp(icon.id, iconId) == 0) {
                return icon.filename;
            }
        }
        return nullptr;
    }
}


#endif //BRAKEZA3D_SCENEOBJECTTYPES_H