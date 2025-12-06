//
// Created by Eduardo on 04/12/2025.
//

#ifndef BRAKEZA3D_GUI_H
#define BRAKEZA3D_GUI_H

#include <functional>
#include <string>
#include <vector>
#include "imgui.h"
#include "../SceneObjectTypes.h"
#include "../Misc/ScriptLUA.h"
#include "../OpenGL/ShaderOGLCustom.h"

namespace GUIType
{
    struct Sheet {
        int x = -1;
        int y = -1;
    };

    struct AddonAllowedObjects
    {
        std::string label;
        TypeObject type;
        Sheet icon;
        bool visible;

        void Toggle() { visible = !visible; }
        [[nodiscard]] std::string id() const { return std::string("AddonAllowedObjects3D" + std::to_string((int)type)).c_str(); }
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
        // Botón apagado - gris oscuro de Blender
        static constexpr ImVec4 BTN_OFF = ImVec4(0.30f, 0.30f, 0.30f, 1.0f);
        // Botón encendido - naranja característico de Blender (en lugar de rojo)
        static constexpr ImVec4 BTN_ON = ImVec4(0.95f, 0.55f, 0.20f, 1.0f);
        // Lua - gris medio acorde al tema
        static constexpr ImVec4 LUA_COLOR = ImVec4(0.40f, 0.40f, 0.40f, 1.0f);
        // Play - verde más apagado estilo Blender
        static constexpr ImVec4 PLAY_COLOR = ImVec4(0.40f, 0.75f, 0.30f, 1.0f);
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
        Window window;
        Sheet icon;
        bool isOpen = false;
        std::function<void()> functionCallBack;
    };

    struct MenuItem {
        std::string label;
        Sheet icon;
        std::function<void()> cb;
    };

    struct IconEntry {
        const char* name;
        Sheet* icon;
        Sheet original;
    };

    struct ScriptEditableManager {
        std::string selectedScriptFilename;
        ScriptLUA *script = nullptr;
        char editableSource[1024 * 16];
    };

    struct ShaderEditableManager {
        bool loaded = false;
        std::string folder;
        std::string name;
        ShaderOGLCustom *shader = nullptr;
        char editableSourceVS[1024 * 16];
        char editableSourceFS[1024 * 16];
    };

}

// ===== DEFINICIÓN ÚNICA DE ICONOS OBJECT =====
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

// ===== DEFINICIÓN ÚNICA DE ICONOS GUI =====
#define ICON_GUI_LIST(X) \
    X(FOLDER, 0, 0) \
    X(PLAY, 0, 0) \
    X(STOP, 0, 0) \
    X(RELOAD, 0, 0) \
    X(REMOVE, 0, 0) \
    X(PAUSE, 0, 0) \
    X(LOCK, 0, 0) \
    X(UNLOCK, 0, 0) \
    X(ADD, 0, 0) \
    X(SCENE, 0, 0) \
    X(SAVE, 0, 0) \
    X(GEAR, 0, 0) \
    X(GHOST, 0, 0) \
    X(SHADER, 0, 0) \
    X(SPOTLIGHT, 0, 0) \
    X(PARTICLES, 0, 0) \
    X(GRAVITY, 0, 0) \
    X(CLICK, 0, 0) \
    X(DRAW_COLLIDERS, 0, 0) \
    X(TARGET, 0, 0) \
    X(MOUSE_LOOK, 0, 0) \
    X(LAYOUT_DEFAULT, 0, 0) \
    X(LAYOUT_CODING, 0, 0) \
    X(LAYOUT_DESIGN, 0, 0) \
    X(PROJECT, 0, 0) \
    X(OPEN, 0, 0) \
    X(GUI, 0, 0) \
    X(TEXTURE, 0, 0) \
    X(TRANSLATE, 0, 0) \
    X(ROTATE, 0, 0) \
    X(SCALE, 0, 0) \
    X(SCRIPT, 0, 0) \
    X(PLAYER, 0, 0) \
    X(SPLASH, 0, 0) \
    X(LIGHT_SYSTEM, 0, 0) \
    X(SHADOW_MAPPING, 0, 0) \
    X(RENDER_TEXTURE, 0, 0) \
    X(RENDER_PIXELS, 0, 0) \
    X(RENDER_SHADING, 0, 0) \
    X(RENDER_WIRE, 0, 0) \
    X(SHOW_BONES, 0, 0) \
    X(PICKING_COLORS, 0, 0) \
    X(FPS, 0, 0) \
    X(PROFILER, 0, 0) \
    X(COLLISION_OBJECTS, 0, 0) \
    X(ABOUT_ME, 0, 0) \
    X(EXIT, 0, 0) \
    X(MNU_BRAKEZA, 0, 0) \
    X(MNU_SCRIPT_CONTROLS, 0, 0) \
    X(MNU_ADD_OBJECT, 0, 0) \
    X(MNU_VIDEO, 0, 0) \
    X(MNU_COLLIDERS, 0, 0) \
    X(MNU_ILLUMINATION, 0, 0) \
    X(MNU_CAMERA, 0, 0) \
    X(MNU_SOUND, 0, 0) \
    X(MNU_LOGGING, 0, 0) \
    X(MNU_LAYOUTS, 0, 0) \
    X(MNU_WINDOWS, 0, 0) \
    X(WIN_PROJECT_SETTINGS, 0, 0) \
    X(WIN_SCENE_OBJECTS, 0, 0) \
    X(WIN_OBJECT_PROPS, 0, 0) \
    X(WIN_OBJECT_SHADERS, 0, 0) \
    X(WIN_OBJECT_SCRIPTS, 0, 0) \
    X(WIN_OBJECT_VARS, 0, 0) \
    X(WIN_GLOBAL_VARS, 0, 0) \
    X(WIN_KEYBOARD_MOUSE, 0, 0) \
    X(WIN_IMAGES, 0, 0) \
    X(WIN_FILES_PROJECTS, 0, 0) \
    X(WIN_FILES_SCENES, 0, 0) \
    X(WIN_FILES_SCRIPTS, 0, 0) \
    X(WIN_FILES_SHADERS, 0, 0) \
    X(WIN_LOGGING, 0, 0) \
    X(WIN_DEPTH_LIGHTS_MAPS, 0, 0) \
    X(WIN_PROFILER, 0, 0) \
    X(WIN_DEBUG_ICONS, 0, 0) \
    X(LOGGING_CLEAR, 0, 0) \
    X(LOGGING_OPTIONS, 0, 0)

// Icons Objects
namespace IconObject {
    // Declaraciones (editables en runtime)
    #define DECLARE_ICON(name, x, y) extern GUIType::Sheet name;
    ICON_OBJECT_LIST(DECLARE_ICON)
    #undef DECLARE_ICON

    // Contar iconos
    #define COUNT_ICONS(...) + 1
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
    #define COUNT_ICONS(...) + 1
    inline constexpr size_t ICON_COUNT = 0 ICON_GUI_LIST(COUNT_ICONS);
    #undef COUNT_ICONS

    extern GUIType::IconEntry ALL_ICONS_EDITOR[ICON_COUNT];

    void ResetToDefault();

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
    static void ImGuiSetColors();
    static GUIType::FolderBrowserCache CreateBrowserCache(std::string folder, const std::string &extension);
};

#endif //BRAKEZA3D_GUI_H