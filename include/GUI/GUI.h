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
        static constexpr ImVec2 ICONS_TOOLBAR = ImVec2(32, 32);
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
        FILES_SCRIPTS
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
        std::function<void()> functionCallBack;
    };
}

// Icons Objects
namespace IconObject {
    inline constexpr GUIType::Sheet OBJECT_3D = {0, 0};
    inline constexpr GUIType::Sheet MESH_3D = {0, 0};
    inline constexpr GUIType::Sheet MESH_3D_ANIMATION = {0, 0};
    inline constexpr GUIType::Sheet LIGHT_POINT = {0, 0};
    inline constexpr GUIType::Sheet LIGHT_SPOT = {0, 0};
    inline constexpr GUIType::Sheet PARTICLE_EMITTER = {0, 0};
    inline constexpr GUIType::Sheet IMAGE_3D_ANIMATION = {0, 0};
    inline constexpr GUIType::Sheet IMAGE_3D_ANIMATION_8DIR = {0, 0};
    inline constexpr GUIType::Sheet IMAGE_2D_ANIMATION = {0, 0};
    inline constexpr GUIType::Sheet IMAGE_3D = {0, 0};
    inline constexpr GUIType::Sheet IMAGE_2D = {0, 0};
    inline constexpr GUIType::Sheet SWARM = {0, 0};
}

// Icons GUI
namespace IconGUI {
    // Sheet coordinates mapping
    inline constexpr GUIType::Sheet FOLDER = {0, 0};
    inline constexpr GUIType::Sheet PLAY = {1, 0};
    inline constexpr GUIType::Sheet STOP = {2, 0};
    inline constexpr GUIType::Sheet RELOAD = {3, 0};
    inline constexpr GUIType::Sheet REMOVE = {4, 0};
    inline constexpr GUIType::Sheet PAUSE = {0, 0};
    inline constexpr GUIType::Sheet LOCK = {0, 0};
    inline constexpr GUIType::Sheet UNLOCK = {0, 0};
    inline constexpr GUIType::Sheet ADD = {4, 3};
    inline constexpr GUIType::Sheet SCENE = {0, 0};
    inline constexpr GUIType::Sheet SAVE = {0, 0};
    inline constexpr GUIType::Sheet GEAR = {0, 0};
    inline constexpr GUIType::Sheet GHOST = {0, 0};
    inline constexpr GUIType::Sheet SHADER = {0, 0};
    inline constexpr GUIType::Sheet SPOTLIGHT = {0, 0};
    inline constexpr GUIType::Sheet PARTICLES = {0, 0};
    inline constexpr GUIType::Sheet GRAVITY = {0, 0};
    inline constexpr GUIType::Sheet CLICK = {0, 0};
    inline constexpr GUIType::Sheet DRAW_COLLIDERS = {0, 0};
    inline constexpr GUIType::Sheet TARGET = {0, 0};
    inline constexpr GUIType::Sheet MOUSE_LOOK = {0, 0};
    inline constexpr GUIType::Sheet LAYOUT_DEFAULT = {0, 0};
    inline constexpr GUIType::Sheet LAYOUT_CODING = {0, 0};
    inline constexpr GUIType::Sheet LAYOUT_DESIGN = {0, 0};
    inline constexpr GUIType::Sheet PROJECT = {0, 0};
    inline constexpr GUIType::Sheet OPEN = {0, 0};
    inline constexpr GUIType::Sheet GUI = {0, 0};
    inline constexpr GUIType::Sheet TEXTURE = {0, 0};
    inline constexpr GUIType::Sheet TRANSLATE = {0, 0};
    inline constexpr GUIType::Sheet ROTATE = {0, 0};
    inline constexpr GUIType::Sheet SCALE = {0, 0};
    inline constexpr GUIType::Sheet SCRIPT = {0, 0};
    inline constexpr GUIType::Sheet PLAYER = {0, 0};
    inline constexpr GUIType::Sheet SPLASH = {0, 0};
    inline constexpr GUIType::Sheet LIGHT_SYSTEM = {0, 0};
    inline constexpr GUIType::Sheet SHADOW_MAPPING = {0, 0};
    inline constexpr GUIType::Sheet RENDER_TEXTURE = {0, 0};
    inline constexpr GUIType::Sheet RENDER_PIXELS = {0, 0};
    inline constexpr GUIType::Sheet RENDER_SHADING = {0, 0};
    inline constexpr GUIType::Sheet RENDER_WIRE = {0, 0};
    inline constexpr GUIType::Sheet SHOW_BONES = {0, 0};
    inline constexpr GUIType::Sheet PICKING_COLORS = {0, 0};
    inline constexpr GUIType::Sheet FPS = {0, 0};
    inline constexpr GUIType::Sheet PROFILER = {0, 0};
    inline constexpr GUIType::Sheet COLLISION_OBJECTS = {0, 0};
    inline constexpr GUIType::Sheet ABOUT_ME = {0, 0};
    inline constexpr GUIType::Sheet EXIT = {0, 3};

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