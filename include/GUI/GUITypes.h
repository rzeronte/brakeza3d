//
// Created by darkhead on 15/9/18.
//

#ifndef SDL2_3D_ENGINE_GUI_H
#define SDL2_3D_ENGINE_GUI_H

#include <functional>
#include <string>
#include "imgui.h"

struct GUISheet {
    int x = -1;
    int y = -1;
};


namespace GUITypes
{
    struct FolderBrowserCache {
        std::string currentFolder;
        std::vector<std::string> folderFiles;
        std::vector<std::string> folderFolders;
    };

    struct Sizes {
        static constexpr ImVec2 ICON_SIZE_MENUS = ImVec2(16, 16);
        static constexpr ImVec2 ICON_SIZE_SMALL = ImVec2(14, 14);
        static constexpr ImVec2 ICON_SIZE_MEDIUM = ImVec2(16, 16);
        static constexpr ImVec2 ICON_SIZE_LARGE = ImVec2(96, 96);
    };

    struct Levels {
        static constexpr float WINDOW_ALPHA = 0.9f;
        static constexpr int DEFAULT_WINDOW_WIDTH = 600;
        static constexpr int DEFAULT_WINDOW_HEIGHT = 600;
        static constexpr float DRAG_IMGUI_SENSIBILITY_LOW = 0.001f;
        static constexpr float DRAG_IMGUI_SENSIBILITY_MEDIUM = 0.01f;
        static constexpr int DRAG_IMGUI_SENSIBILITY_TOP = 1.0f;
        static constexpr int DRAG_FOV_MIN = 1.0f;
        static constexpr int DRAG_FOV_MAX = 160.0f;
        static constexpr int DRAG_FRAMERATE_MIN = 1.0f;
        static constexpr int DRAG_FRAMERATE_MAX = 1024.0f;
        static constexpr int DRAG_FAR_PLANE_MIN = 1.0f;
        static constexpr int DRAG_FAR_PLANE_MAX = 1024.0f;
        static constexpr int DRAG_UNIT_MIN = 0.f;
        static constexpr int DRAG_UNIT_MAX = 1.f;
        static constexpr int DRAG_VOLUME_MIN = 0.f;
        static constexpr int DRAG_VOLUME_MAX = 1.f;
    };

    struct Colors {
        static constexpr ImVec4 BTN_OFF = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);      // Gris oscuro
        static constexpr ImVec4 BTN_ON = ImVec4(0.36f, 0.48f, 0.82f, 0.40f);   // Gris claro
        static constexpr ImVec4 LUA_COLOR = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);      // Gris intermedio
        static constexpr ImVec4 PLAY_COLOR = ImVec4(0.3f, 0.9f, 0.3f, 1.0f);     // Gris intermedio
    };

    enum GUIWindowsGroups {
        OBJECT_WINDOWS,
        FILE_SYSTEM,
        PROFILING,
        SCRIPTING
    };

    enum GUIWindow {
        PROFILER,
        DEPTH_LIGHTS_MAPS,
        LOGGING,
        SCENE_OBJECTS,
        OBJECT_PROPS,
        OBJECT_SHADERS,
        OBJECT_SCRIPTS,
        OBJECT_VARS,
        GLOBAL_VARS,
        KEYBOARD_MOUSE,
        IMAGES,
        PROJECT_SETTINGS,
        FILES_SCENES,
        FILES_PROJECTS,
        FILES_SHADERS,
        FILES_SCRIPTS
    };

    struct GUIWindowData {
        std::string label;
        GUISheet icon;
        GUIWindow window;
        bool isOpen = false;
        std::function<void()> functionCallBack;
    };

    struct GUIMenuItem {
        std::string label;
        std::function<void()> functionCallBack;
    };
}

// Icons Objects
namespace IconObject {
    inline constexpr GUISheet OBJECT_3D = {0, 0};
    inline constexpr GUISheet MESH_3D = {0, 0};
    inline constexpr GUISheet MESH_3D_ANIMATION = {0, 0};
    inline constexpr GUISheet LIGHT_POINT = {0, 0};
    inline constexpr GUISheet LIGHT_SPOT = {0, 0};
    inline constexpr GUISheet PARTICLE_EMITTER = {0, 0};
    inline constexpr GUISheet IMAGE_3D_ANIMATION = {0, 0};
    inline constexpr GUISheet IMAGE_3D_ANIMATION_8DIR = {0, 0};
    inline constexpr GUISheet IMAGE_2D_ANIMATION = {0, 0};
    inline constexpr GUISheet IMAGE_3D = {0, 0};
    inline constexpr GUISheet IMAGE_2D = {0, 0};
    inline constexpr GUISheet SWARM = {0, 0};
}

// Icons GUI
namespace IconGUI {
    // Sheet coordinates mapping
    inline constexpr GUISheet FOLDER = {0, 0};
    inline constexpr GUISheet PLAY = {1, 0};
    inline constexpr GUISheet STOP = {2, 0};
    inline constexpr GUISheet RELOAD = {3, 0};
    inline constexpr GUISheet REMOVE = {4, 0};
    inline constexpr GUISheet PAUSE = {0, 0};
    inline constexpr GUISheet LOCK = {0, 0};
    inline constexpr GUISheet UNLOCK = {0, 0};
    inline constexpr GUISheet ADD = {4, 3};
    inline constexpr GUISheet SCENE = {0, 0};
    inline constexpr GUISheet SAVE = {0, 0};
    inline constexpr GUISheet GEAR = {0, 0};
    inline constexpr GUISheet GHOST = {0, 0};
    inline constexpr GUISheet SHADER = {0, 0};
    inline constexpr GUISheet SPOTLIGHT = {0, 0};
    inline constexpr GUISheet PARTICLES = {0, 0};
    inline constexpr GUISheet GRAVITY = {0, 0};
    inline constexpr GUISheet CLICK = {0, 0};
    inline constexpr GUISheet DRAW_COLLIDERS = {0, 0};
    inline constexpr GUISheet TARGET = {0, 0};
    inline constexpr GUISheet MOUSE_LOOK = {0, 0};
    inline constexpr GUISheet LAYOUT_DEFAULT = {0, 0};
    inline constexpr GUISheet LAYOUT_CODING = {0, 0};
    inline constexpr GUISheet LAYOUT_DESIGN = {0, 0};
    inline constexpr GUISheet PROJECT = {0, 0};
    inline constexpr GUISheet OPEN = {0, 0};
    inline constexpr GUISheet GUI = {0, 0};
    inline constexpr GUISheet TEXTURE = {0, 0};
    inline constexpr GUISheet TRANSLATE = {0, 0};
    inline constexpr GUISheet ROTATE = {0, 0};
    inline constexpr GUISheet SCALE = {0, 0};
    inline constexpr GUISheet SCRIPT = {0, 0};
    inline constexpr GUISheet PLAYER = {0, 0};
    inline constexpr GUISheet SPLASH = {0, 0};
    inline constexpr GUISheet LIGHT_SYSTEM = {0, 0};
    inline constexpr GUISheet SHADOW_MAPPING = {0, 0};
    inline constexpr GUISheet RENDER_TEXTURE = {0, 0};
    inline constexpr GUISheet RENDER_PIXELS = {0, 0};
    inline constexpr GUISheet RENDER_SHADING = {0, 0};
    inline constexpr GUISheet RENDER_WIRE = {0, 0};
    inline constexpr GUISheet SHOW_BONES = {0, 0};
    inline constexpr GUISheet PICKING_COLORS = {0, 0};
    inline constexpr GUISheet FPS = {0, 0};
    inline constexpr GUISheet PROFILER = {0, 0};
    inline constexpr GUISheet COLLISION_OBJECTS = {0, 0};
    inline constexpr GUISheet ABOUT_ME = {0, 0};
    inline constexpr GUISheet EXIT = {0, 3};

    // GUI Properties features for each object
    struct ObjectGUIFeatures {
        bool position = true;
        bool rotation = true;
        bool scale = true;
        bool alpha = true;
        bool shaders = true;
        bool attached = true;
        bool collider = true;
        bool misc = true;
    };
}

#endif //SDL2_3D_ENGINE_GUI_H
