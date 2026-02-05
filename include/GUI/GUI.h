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
#include "../Misc/Timer.h"
#include "ImGuiColorTextEdit/TextEditor.h"

namespace GUIType
{
    struct DragDropTarget {
        inline static const char* SCRIPT_ITEM = "SCRIPT_ITEM";
        inline static const char* SHADER_ITEM = "SHADER_ITEM";
        inline static const char* IMAGE_ITEM = "IMAGE_ITEM";
        inline static const char* SCENE_ITEM = "SCENE_ITEM";
    };

    enum BrowserFileType {
        BROWSE_PROJECTS,
        BROWSE_SCENES,
        BROWSE_SHADERS,
        BROWSE_SCRIPTS,
    };

    struct Sheet {
        int x = -1;
        int y = -1;
    };

    enum ViewerObjectsMode {
        LIST,
        TREE
    };

    struct ViewerObjectType
    {
        std::string label;
        ObjectType type;
        Sheet icon;
        bool visible;

        void Toggle() { visible = !visible; }
    };

    struct BrowserCache {
        std::string currentFolder;
        std::vector<std::string> folderFiles;
        std::vector<std::string> folderFolders;
        std::string ext;
        std::function<void()> onChangeFolderCallback;
    };

    struct ButtonSize {
        static constexpr ImVec2 BUTTON_SMALL = ImVec2(-1, 20);
        static constexpr ImVec2 BUTTON_NORMAL = ImVec2(-1, 28);
        static constexpr ImVec2 BUTTON_BIG = ImVec2(-1, 36);
        static constexpr ImVec2 BUTTON_SMALL_AUTO = ImVec2(0, 20);
        static constexpr ImVec2 BUTTON_NORMAL_AUTO = ImVec2(0, 28);
        static constexpr ImVec2 BUTTON_BIG_AUTO = ImVec2(0, 36);
    };

    struct Sizes {
        static constexpr ImVec2 ICONS_TOOLBAR = ImVec2(22, 22);
        static constexpr ImVec2 ICONS_OBJECTS_ALLOWED = ImVec2(18, 18);
        static constexpr ImVec2 ICONS_BROWSERS = ImVec2(16, 16);
        static constexpr ImVec2 ICONS_CONSOLE = ImVec2(16, 16);
        static constexpr ImVec2 ICONS_LOG = ImVec2(16, 16);
        static constexpr ImVec2 ICON_SIZE_MENUS = ImVec2(16, 16);
        static constexpr ImVec2 ICON_SIZE_SMALL = ImVec2(14, 14);
        static constexpr ImVec2 ICON_LOCKS = ImVec2(14, 14);
        static constexpr ImVec2 DRAG_ACCEPT = ImVec2(32, 32);
        static constexpr ImVec2 ICON_BROWSER_TYPE = ImVec2(32, 32);
        static constexpr ImVec2 ICONS_CODE_EDITOR = ImVec2(24, 24);
    };

    struct Levels {
        static constexpr float WINDOW_ALPHA = 1.0f;
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
        static constexpr float DRAG_VOLUME_MIN = 0.f;
        static constexpr float DRAG_VOLUME_MAX = 128.f;
    };

    struct Colors {
        static constexpr ImVec4 BTN_OFF = ImVec4(0.20f, 0.22f, 0.24f, 1.0f);  // Gris oscuro neutro
        static constexpr ImVec4 BTN_ON = ImVec4(0.28f, 0.38f, 0.32f, 1.0f);   // Gris verdoso sutil 🟢
        static constexpr ImVec4 LUA_COLOR = ImVec4(0.25f, 0.35f, 0.30f, 1.0f); // Verde grisáceo (como tab inactiva)
        static constexpr ImVec4 PLAY_COLOR = ImVec4(0.35f, 0.55f, 0.42f, 1.0f); // Verde apagado profesional
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
        MEDIA_BROWSER,
        PROJECT_SETTINGS,
        DEBUG_ICONS,
        CODE_EDITOR,
        DOCUMENTATION,
        SCENE_INFO,
        SHADER_NODES_EDITOR,
        PROJECT_INFO,
        THREADS,
        BROWSER,
    };

    struct WindowGUI {
        std::string label;
        Window window;
        Sheet icon;
        bool isOpen = false;
        bool isInternal = false;
        bool isDockable = true;
        bool isObjectWindow = false;
        std::function<void()> functionCallBack;
        ImVec2 minSize = ImVec2(200, 150);
        ImVec2 maxSize = ImVec2(FLT_MAX, FLT_MAX);
        bool autoHideIfNotSelected = false;
    };

    struct LayoutWindowConfig {
        Window window;
        bool visible;
    };

    struct MenuItem {
        std::string label;
        Sheet icon;
        std::function<void()> cb;
    };

    struct IconEntry {
        const char* name = nullptr;
        Sheet* icon = nullptr;
        Sheet original;
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
    X(CLEAR_SCENE, 0, 0) \
    X(CREATE_FILE, 0, 0) \
    X(FOLDER, 0, 0) \
    X(REMOVE, 0, 0) \
    X(SAVE, 0, 0) \
    X(RESET, 0, 0) \
    X(TEXTURE, 0, 0) \
    X(DROP_ACCEPT, 0, 0) \
    X(SHADER_FILE, 0, 0) \
    X(SCRIPT_FILE, 0, 0) \
    X(SCRIPT_EDIT, 0, 0) \
    X(SCENE_FILE, 0, 0) \
    X(PROJECT_FILE, 0, 0) \
    X(SCENE_LOAD, 0, 0) \
    X(PROJECT_OPEN, 0, 0) \
    X(RENDER_TEXTURE, 0, 0) \
    X(RENDER_PIXELS, 0, 0) \
    X(RENDER_SHADING, 0, 0) \
    X(RENDER_WIRE, 0, 0) \
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
    X(LOGGING_MSG_OK, 0, 0) \
    X(LOGGING_MSG_ERROR, 0, 0) \
    X(LOGGING_MSG_WARNING, 0, 0) \
    X(LOGGING_OPTIONS, 0, 0) \
    X(VIDEO_VSYNC, 0, 0) \
    X(VIDEO_LIMIT_FRAMERATE, 0, 0) \
    X(VIDEO_DOF, 0, 0) \
    X(VIDEO_FOG, 0, 0) \
    X(VIDEO_OBJECT_AXIS, 0, 0) \
    X(VIDEO_AABB, 0, 0) \
    X(VIDEO_OCTREE, 0, 0) \
    X(VIDEO_GRID, 0, 0) \
    X(VIDEO_PICKING_COLORS, 0, 0) \
    X(VIDEO_FPS, 0, 0) \
    X(VIDEO_SHOW_BONES, 0, 0) \
    X(BRAKEZA_ABOUT_ME, 0, 0) \
    X(BRAKEZA_EXIT, 0, 0) \
    X(COLLIDERS_COLLISION_SYSTEM, 0, 0) \
    X(COLLIDERS_HANDLE_OBJECTS_COLLISIONS, 0, 0) \
    X(COLLIDERS_MAX_SUBSTEPS, 0, 0) \
    X(COLLIDERS_FIXED_TIMESTEP, 0, 0) \
    X(COLLIDERS_GRAVITY, 0, 0) \
    X(COLLIDERS_DEBUG_MODE, 0, 0) \
    X(COLLIDERS_DEMO_APP, 0, 0) \
    X(ILLUMINATION_ENABLE_SYSTEM, 0, 0) \
    X(ILLUMINATION_ENABLE_SHADOW_MAPPING, 0, 0) \
    X(ILLUMINATION_SUN_DIRECTION, 0, 0) \
    X(ILLUMINATION_COLORS, 0, 0) \
    X(ILLUMINATION_ENABLE_SUN_SHADOWS, 0, 0) \
    X(ILLUMINATION_SHADOW_MAPPING_DEBUG, 0, 0) \
    X(CAMERA_FOV, 0, 0) \
    X(CAMERA_POSITION, 0, 0) \
    X(CAMERA_ROTATION, 0, 0) \
    X(CAMERA_CURSOR_SPEED_WALKING, 0, 0) \
    X(CAMERA_CURSOR_SPEED_STRAFE, 0, 0) \
    X(CAMERA_CLICK_OBJECTS, 0, 0) \
    X(CAMERA_MOUSE_LOOK, 0, 0) \
    X(SOUND_ENABLE_SYSTEM, 0, 0) \
    X(SOUND_MUSIC_VOLUME, 0, 0) \
    X(SOUND_GLOBAL_CHANNEL_VOLUME, 0, 0) \
    X(LOGGING_OUTPUT_CONSOLE, 0, 0) \
    X(LOGGING_OUTPUT_STD, 0, 0) \
    X(LOGGING_COLLIDER, 0, 0) \
    X(LAYOUTS_ENABLE_GUI, 0, 0) \
    X(LAYOUTS_ENABLE_TOOLBAR, 0, 0) \
    X(LAYOUTS_LAYOUT_DEFAULT, 0, 0) \
    X(LAYOUTS_LAYOUT_DEVS, 0, 0) \
    X(LAYOUTS_LAYOUT_DESIGNERS, 0, 0) \
    X(LAYOUTS_SAVE_LAYOUT, 0, 0) \
    X(TOOLBAR_ENABLE_GUI, 0, 0) \
    X(TOOLBAR_LAYOUT_DEFAULT, 0, 0) \
    X(TOOLBAR_LAYOUT_CODING, 0, 0) \
    X(TOOLBAR_LAYOUT_DESIGN, 0, 0) \
    X(TOOLBAR_TRANSLATE, 0, 0) \
    X(TOOLBAR_ROTATE, 0, 0) \
    X(TOOLBAR_SCALE, 0, 0) \
    X(TOOLBAR_ENABLE_STEP_SIMULATION, 0, 0) \
    X(TOOLBAR_DRAW_COLLIDERS, 0, 0) \
    X(TOOLBAR_CLICK_OBJECTS, 0, 0) \
    X(TOOLBAR_MOUSE_LOOK, 0, 0) \
    X(TOOLBAR_ENABLE_LIGHT_SYSTEM, 0, 0) \
    X(TOOLBAR_ENABLE_SHADOW_MAPPING, 0, 0) \
    X(COLLIDER_BODY, 0, 0) \
    X(COLLIDER_KINEMATIC, 0, 0) \
    X(COLLIDER_GHOST, 0, 0) \
    X(BONE_LOCK, 0, 0) \
    X(BONE_UNLOCK, 0, 0) \
    X(LUA_PLAY, 0, 0) \
    X(LUA_STOP, 0, 0) \
    X(LUA_LOCK, 0, 0) \
    X(LUA_UNLOCK, 0, 0) \
    X(LUA_RELOAD, 0, 0) \
    X(LUA_REMOVE, 0, 0) \
    X(WINDOW_FULLSCREEN, 0, 0) \
    X(SHADER_LOAD, 0, 0) \
    X(SHADER_LOCK, 0, 0) \
    X(SHADER_UNLOCK, 0, 0) \
    X(SHADER_RELOAD, 0, 0) \
    X(SCRIPT_RELOAD, 0, 0) \
    X(PROJECT_REMOVE, 0, 0) \
    X(SCENE_REMOVE, 0, 0) \
    X(SCRIPT_REMOVE, 0, 0) \
    X(SHADER_REMOVE, 0, 0) \
    X(OBJECT_REMOVE_SCENE, 0, 0) \
    X(OBJECTS_VIEWER_LIST, 0, 0) \
    X(OBJECTS_VIEWER_TREE, 0, 0) \
    X(OBJECTS_VIEWER_CLEAR_FILTER, 0, 0) \
    X(LAYOUTS_RESET_CURRENT, 0, 0) \
    X(PROJECT_SETUP_GLOBAL_SCRIPTS, 0, 0) \
    X(PROJECT_SETUP_SCENE_SCRIPTS, 0, 0) \
    X(PROJECT_SETUP_SCENE_SHADERS, 0, 0) \
    X(SHADER_EDIT, 0, 0) \
    X(SCRIPT_CREATE_VARIABLE, 0, 0) \
    X(SHADER_CREATE_VARIABLE, 0, 0) \
    X(SHADER_SAVE, 0, 0) \
    X(SCRIPT_SAVE, 0, 0) \
    X(WARNING, 0, 0) \
    X(VIDEO_SHADER_GRID, 0, 0) \
    X(TOOLBAR_GRID_BACKGROUND, 0, 0) \
    X(SCRIPT_REMOVE_VARIABLE, 0, 0) \
    X(SHADER_REMOVE_VARIABLE, 0, 0) \
    X(SHADER_TYPE_MESH3D, 0, 0) \
    X(SHADER_TYPE_POSTPROCESSING, 0, 0) \
    X(CANCEL, 0, 0) \
    X(TOOLBAR_PICKING_COLORS, 0, 0) \
    X(VIDEO_TAKE_SCREENSHOT, 0, 0) \
    X(TOOLBAR_TAKE_SCREENSHOT, 0, 0) \
    X(MNU_WORKERS, 0, 0) \
    X(WIN_CODE_EDITOR, 0, 0) \
    X(WIN_SCRIPT_SETUP, 0, 0) \
    X(WIN_SHADER_SETUP, 0, 0) \
    X(SHADER_CODE_VS, 0, 0) \
    X(SHADER_CODE_FS, 0, 0) \
    X(EMPTY, 0, 0) \
    X(WIN_DOCUMENTATION, 0, 0) \
    X(SCENE_INFO, 0, 0) \
    X(CHECKED, 0, 0) \
    X(UNCHECKED, 0, 0) \
    X(FILE_BROKEN, 0, 0) \
    X(WIN_RESOLUTION, 0, 0) \
    X(WIN_SHADER_NODES, 0, 0) \
    X(NODE_EDITOR_AUTOFIT, 0, 0) \
    X(NODE_EDITOR_PIN_IN, 0, 0) \
    X(NODE_EDITOR_PIN_OUT, 0, 0) \
    X(NODE_TYPE_OUTPUT, 0, 0) \
    X(NODE_TYPE_DEFAULT, 0, 0) \
    X(NODE_EDITOR_POPUP, 0, 0) \
    X(NODE_EDITOR_TAB, 0, 0) \
    X(PROJECT_SETUP_SCENES, 0, 0) \
    X(IMAGE_FILE, 0, 0) \
    X(PROJECT_INFO, 0, 0) \
    X(MNU_RESOURCESHUB, 0, 0) \
    X(SESSION_ON, 0, 0) \
    X(SESSION_OFF, 0, 0) \
    X(LAYOUTS_ENABLE_STATUSBAR, 0, 0) \
    X(WIN_THREADS, 0, 0) \
    X(STAR_ON, 0, 0) \
    X(STAR_OFF, 0, 0) \
    X(LIGHT_ON, 0, 0) \
    X(LIGHT_OFF, 0, 0) \
    X(SESSION_CLOSE, 0, 0) \
    X(DOWNLOAD_RESOURCE, 0, 0) \
    X(HUB_URL_LINK, 0, 0) \
    X(SESSION_OPEN, 0, 0) \
    X(PROJECT_SAVE, 0, 0) \
    X(SCENE_SAVE, 0, 0) \
    X(WIN_BROWSER, 0, 0) \
    X(FOLDER_CURRENT, 0, 0) \
    X(SEARCH, 0, 0) \
    X(FOLDER_BACK, 0, 0) \
    X(BULLET, 0, 0) \
    X(TREE_BULLET_ON, 0, 0) \
    X(TREE_BULLET_OFF, 0, 0) \
    X(PROJECT_FILE_UNNAMED, 0, 0) \
    X(SCENE_FILE_UNNAMED, 0, 0) \
    X(PROJECT_CLOSE, 0, 0) \
    X(SCENE_ATTACH_PROJECT, 0, 0) \
    X(SCRIPT_LOAD, 0, 0) \
    X(WIN_OBJECT_AUTOHIDE_ON, 0, 0) \
    X(WIN_OBJECT_AUTOHIDE_OFF, 0, 0) \
    X(DOWNLOAD_ALL_RESOURCE, 0, 0) \
    X(HUB_ASSET_AUTHOR, 0, 0) \
    X(HUB_ASSET_SUBMIT_RATING, 0, 0) \
    X(CLEAR_FILTERS, 0, 0) \
    X(DOCKING_ON, 0, 0) \
    X(DOCKING_OFF, 0, 0) \
    X(CLEAN_SCENE, 0, 0) \
    X(COLLIDER_INFO, 0, 0) \
    X(COLLIDER_EDIT, 0, 0) \


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
    static void DrawButtonTransparent(const std::string &tooltip, GUIType::Sheet icon, ImVec2 size, bool active, const std::function<void()> &cb);
    static void ShowPopUp(const char* title, const char *message, const std::function<void()>& onConfirm);
    static void DrawButtonConfirm(const std::string &title, const std::string &question, GUIType::Sheet icon, ImVec2 size, const std::function<void()>& cb);
    static void Toggle(bool &value);
    static void ImGuiSetColors();
    static void CLIWelcomeMessage();
    static void ShowLoadTime(const std::string &text, const Timer &t);
    static bool ImageButtonSmall(GUIType::Sheet, const std::string &text, const std::function<void()> &onClick);
    static bool ImageButtonNormal(GUIType::Sheet, const std::string &text, const std::function<void()> &onClick);
    static bool ImageButtonBig(GUIType::Sheet, const std::string &text, const std::function<void()> &onClick);
    static void ImageButtonSmallConfirm(GUIType::Sheet texture, const std::string &text, const std::string &title, const std::string &question, const std::function<void()> &onConfirm);
    static void ImageButtonNormalConfirm(GUIType::Sheet texture, const std::string &text, const std::string &title, const std::string &question, const std::function<void()> &onConfirm);
    static void ImageButtonBigConfirm(GUIType::Sheet texture, const std::string &text, const std::string &title, const std::string &question, const std::function<void()> &onConfirm);

    static bool AutoImageButton(GUIType::Sheet, const char *label, const ImVec2 &imageSize, float height, float padding, const std::function<void()> &onClick);


    static GUIType::BrowserCache CreateBrowserCache(std::string folder, const std::string &extension, std::function<void()> functionCallBack);
};

#endif //BRAKEZA3D_GUI_H