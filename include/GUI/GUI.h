//
// Created by Eduardo on 04/12/2025.
//

#ifndef BRAKEZA3D_GUI_H
#define BRAKEZA3D_GUI_H

#include <functional>
#include <string>
#include <vector>
#include "imgui.h"

namespace GUIType
{
    struct Sheet {
        int x = -1;
        int y = -1;
    };

    struct FolderBrowserCache {
        std::string currentFolder;
        std::vector<std::string> folderFiles;
        std::vector<std::string> folderFolders;
    };

    struct Sizes {
        static constexpr ImVec2 ICONS_TOOLBAR = ImVec2(24, 24);
        static constexpr ImVec2 ICONS_OBJECTS_ALLOWED = ImVec2(16, 16);
        static constexpr ImVec2 ICONS_BROWSERS = ImVec2(16, 16);
        static constexpr ImVec2 ICONS_CONSOLE = ImVec2(16, 16);
        static constexpr ImVec2 ICONS_LOG = ImVec2(16, 16);
        static constexpr ImVec2 ICON_SIZE_MENUS = ImVec2(16, 16);
        static constexpr ImVec2 ICON_SIZE_SMALL = ImVec2(14, 14);
        static constexpr ImVec2 ICON_LOCKS = ImVec2(14, 14);
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
        static constexpr ImVec4 BTN_OFF = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
        static constexpr ImVec4 BTN_ON = ImVec4(0.8f, 0.0f, 0.0f, 1.0f);
        static constexpr ImVec4 LUA_COLOR = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
        static constexpr ImVec4 PLAY_COLOR = ImVec4(0.3f, 0.9f, 0.3f, 1.0f);
    };

    enum GUIWindowsGroups {
        OBJECT_WINDOWS,
        FILE_SYSTEM,
        PROFILING,
        SCRIPTING
    };

    enum Window {
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
        FILES_SCRIPTS,
        DEBUG_ICONS
    };

    struct WindowData {
        std::string label;
        Sheet icon;
        Window window;
        bool isOpen = false;
        std::function<void()> functionCallBack;
    };

    struct MenuItem {
        std::string label;
        std::function<void()> cb;
    };

    struct IconEntry {
        const char* name;
        Sheet* icon;
        Sheet original;
    };
}

// ===== DEFINICIÓN ÚNICA DE ICONOS OBJECT =====
#define ICON_OBJECT_LIST(X) \
    X(OBJECT_3D, 10, 14) \
    X(MESH_3D, 14, 14) \
    X(MESH_3D_ANIMATION, 0, 0) \
    X(LIGHT_POINT, 1, 13) \
    X(LIGHT_SPOT, 0, 0) \
    X(PARTICLE_EMITTER, 0, 0) \
    X(IMAGE_3D_ANIMATION, 5, 27) \
    X(IMAGE_3D_ANIMATION_8DIR, 0, 0) \
    X(IMAGE_2D_ANIMATION, 0, 0) \
    X(IMAGE_3D, 0, 0) \
    X(IMAGE_2D, 0, 0) \
    X(SWARM, 0, 0)

// ===== DEFINICIÓN ÚNICA DE ICONOS GUI =====
#define ICON_GUI_LIST(X) \
    X(FOLDER, 3, 31) \
    X(PLAY, 2, 29) \
    X(STOP, 5, 3) \
    X(RELOAD, 15, 9) \
    X(REMOVE, 3, 3) \
    X(PAUSE, 4, 12) \
    X(LOCK, 5, 25) \
    X(UNLOCK, 4, 25) \
    X(ADD, 15, 37) \
    X(SCENE, 2, 37) \
    X(SAVE, 4, 33) \
    X(GEAR, 9, 8) \
    X(GHOST, 9, 29) \
    X(SHADER, 15, 31) \
    X(SPOTLIGHT, 3, 24) \
    X(PARTICLES, 11, 13) \
    X(GRAVITY, 15, 30) \
    X(CLICK, 9, 18) \
    X(DRAW_COLLIDERS, 3, 35) \
    X(TARGET, 13, 16) \
    X(MOUSE_LOOK, 10, 18) \
    X(LAYOUT_DEFAULT, 7, 1) \
    X(LAYOUT_CODING, 0, 0) \
    X(LAYOUT_DESIGN, 2, 27) \
    X(PROJECT, 15, 38) \
    X(OPEN, 15, 9) \
    X(GUI, 12, 17) \
    X(TEXTURE, 2, 4) \
    X(TRANSLATE, 3, 15) \
    X(ROTATE, 2, 15) \
    X(SCALE, 1, 15) \
    X(SCRIPT, 0, 0) \
    X(PLAYER, 14, 2) \
    X(SPLASH, 0, 0) \
    X(LIGHT_SYSTEM, 0, 0) \
    X(SHADOW_MAPPING, 0, 0) \
    X(RENDER_TEXTURE, 0, 0) \
    X(RENDER_PIXELS, 0, 0) \
    X(RENDER_SHADING, 0, 0) \
    X(RENDER_WIRE, 0, 0) \
    X(SHOW_BONES, 14, 38) \
    X(PICKING_COLORS, 11, 10) \
    X(FPS, 5, 4) \
    X(PROFILER, 10, 26) \
    X(COLLISION_OBJECTS, 8, 13) \
    X(ABOUT_ME, 15, 27) \
    X(EXIT, 0, 10)

// Icons Objects
namespace IconObject {
    // Declaraciones (editables en runtime)
    #define DECLARE_ICON(name, x, y) extern GUIType::Sheet name;
    ICON_OBJECT_LIST(DECLARE_ICON)
    #undef DECLARE_ICON

    // Contar iconos
    #define COUNT_ICONS(...) +1
    inline constexpr size_t ICON_COUNT = 0 ICON_OBJECT_LIST(COUNT_ICONS);
    #undef COUNT_ICONS

    // Array de metadatos para el editor
    extern GUIType::IconEntry ALL_ICONS_EDITOR[ICON_COUNT];

    // Función para resetear
    void ResetToDefault();
}

// Icons GUI
namespace IconGUI {
    // Declaraciones (editables en runtime)
    #define DECLARE_ICON(name, x, y) extern GUIType::Sheet name;
    ICON_GUI_LIST(DECLARE_ICON)
    #undef DECLARE_ICON

    // Contar iconos
    #define COUNT_ICONS(...) +1
    inline constexpr size_t ICON_COUNT = 0 ICON_GUI_LIST(COUNT_ICONS);
    #undef COUNT_ICONS

    // Array de metadatos para el editor
    extern GUIType::IconEntry ALL_ICONS_EDITOR[ICON_COUNT];

    // Función para resetear
    void ResetToDefault();

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

class GUI
{
public:
    static void DrawButton(const std::string &tooltip, GUIType::Sheet icon, ImVec2 size, bool active, const std::function<void()> &onClick);
    static void Toggle(bool &value);
    static GUIType::FolderBrowserCache CreateBrowserCache(std::string folder, const std::string &extension);
};

#endif //BRAKEZA3D_GUI_H