#ifndef BRAKEZA3D_GUITYPES_H
#define BRAKEZA3D_GUITYPES_H

// Minimal GUI types needed by engine-core classes (Object3D and subclasses).
// No ImGui dependency — safe to include from any engine layer.

#include "../SceneObjectTypes.h"

namespace GUIType {
    struct Sheet {
        int x = -1;
        int y = -1;
    };

    struct IconEntry {
        const char*   name  = nullptr;
        Sheet*        icon  = nullptr;
        Sheet         original;
    };
}

// ===== ICON OBJECT LIST =====
#define ICON_OBJECT_LIST(X) \
    X(OBJECT_3D, 0, 0) \
    X(MESH_3D, 0, 0) \
    X(MESH_3D_ANIMATION, 0, 0) \
    X(LIGHT_POINT, 0, 0) \
    X(LIGHT_SPOT, 0, 0) \
    X(PARTICLE_EMITTER, 0, 0) \
    X(IMAGE_3D_ANIMATION, 0, 0) \
    X(IMAGE_3D_ANIMATION_360, 0, 0) \
    X(IMAGE_2D_ANIMATION, 0, 0) \
    X(IMAGE_3D, 0, 0) \
    X(IMAGE_2D, 0, 0) \
    X(SWARM, 0, 0)

namespace IconObject {
    #define DECLARE_ICON(name, x, y) extern GUIType::Sheet name;
        ICON_OBJECT_LIST(DECLARE_ICON)
    #undef DECLARE_ICON

    #define COUNT_ICONS(...) + 1
        inline constexpr size_t ICON_COUNT = 0 ICON_OBJECT_LIST(COUNT_ICONS);
    #undef COUNT_ICONS

    extern GUIType::IconEntry ALL_ICONS_EDITOR[ICON_COUNT];
    void ResetToDefault();
}

namespace IconGUI {
    struct ObjectGUIFeatures {
        bool position = true;
        bool rotation = true;
        bool scale    = true;
        bool alpha    = true;
        bool shaders  = true;
        bool attached = true;
        bool collider = true;
        bool misc     = true;
    };
}

#endif //BRAKEZA3D_GUITYPES_H
