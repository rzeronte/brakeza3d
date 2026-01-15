#ifndef SDL2_3D_ENGINE_ENGINESETUP_H
#define SDL2_3D_ENGINE_ENGINESETUP_H

#include "3D/Vertex3D.h"
#include "Misc/ScriptLUA.h"
#include <GL/glew.h>

class Config {

public:
    static Config *get();

    static Config *instance;

    bool EXIT = false;

    // TITLE/VERSION
    std::string ENGINE_TITLE = "Brakeza3D - By Eduardo Rodriguez Alvarez <eduardo@brakeza.com>";
    std::string ENGINE_VERSION = "v.1.0.0";

    std::string ENGINE_WEBSITE = "https://www.brakeza.com";
    std::string ENGINE_SOURCE_WEBSITE = "https://github.com/rzeronte/brakeza3d";

    // DEFAULT APP ICONS
    std::string iconApplication = "application.png";
    std::string mouseIcon = "mouse.png";
    std::string LOGO_BRAKEZA = "logo.png";

    // FOLDERS
    std::string ROOT_FOLDER           = "../";
    std::string CONFIG_FOLDER         = "../config/";
    std::string SHADERS_FOLDER        = "../GLSL/";
    std::string ASSETS_FOLDER         = "../assets/";
    std::string DOCUMENTATION_FOLDER  = "../doc/GUI/";
    std::string SPRITES_FOLDER        = ASSETS_FOLDER + "sprites/";
    std::string MAPS_FOLDER           = ASSETS_FOLDER + "maps/";
    std::string IMAGES_FOLDER         = ASSETS_FOLDER + "images/";
    std::string VIDEOS_FOLDER         = ASSETS_FOLDER + "videos/";
    std::string SOUNDS_FOLDER         = ASSETS_FOLDER + "sounds/";
    std::string MODELS_FOLDER         = ASSETS_FOLDER + "models/";
    std::string ANIMATIONS_FOLDER     = ASSETS_FOLDER + "animations/";
    std::string TEMPLATES_FOLDER      = ASSETS_FOLDER + "templates/";
    std::string FONTS_FOLDER          = ASSETS_FOLDER + "fonts/";
    std::string TEXTURES_FOLDER       = ASSETS_FOLDER + "textures/";
    std::string GRIDS_FOLDER          = ASSETS_FOLDER + "grids/";
    std::string ICONS_FOLDER          = ASSETS_FOLDER + "icons/";
    std::string SCRIPTS_FOLDER        = ASSETS_FOLDER + "scripts/";
    std::string PROJECTS_FOLDER       = ASSETS_FOLDER + "projects/";
    std::string SCENES_FOLDER         = ASSETS_FOLDER + "scenes/";
    std::string CUSTOM_SHADERS_FOLDER = ASSETS_FOLDER + "shaders/";
    std::string SCREENSHOTS_FOLDER    = ASSETS_FOLDER + "screenshots/";
    std::string DEFAULT_SOUNDS_FILE   = "sounds.json";
    std::string DEFAULT_SCENE         = "brakeza.json";
    std::string DOCUMENTATION_CONFIG  = "documentation.json";
    std::string CAMERA_OBJECT_NAME    = "Camera";
    std::string GUI_ICON_SHEET        = "sheet.png";
    std::string SPLASH_FILENAME       = "splash.png";
    std::string ICONS_CONFIG          = "icons.json";

    std::string SCRIPTS_EXT     = "json";
    std::string SCENES_EXT      = "json";
    std::string SHADERS_EXT     = "json";
    std::string PROJECTS_EXT    = "json";
    std::string IMAGES_EXT      = "png";
    std::string MESH3D_EXT      = "fbx";

    // GLOBAL AXIS DIRECTIONS
    Vertex3D right = Vertex3D(1, 0, 0);
    Vertex3D up = Vertex3D(0, 1, 0);
    Vertex3D forward = Vertex3D(0, 0, 1);
    Vertex3D left = right.getInverse();
    Vertex3D down = up.getInverse();
    Vertex3D backward = forward.getInverse();

    // SCREEN RESOLUTION
    const int screenWidth = 1280;
    const int screenHeight = 720;

    const int renderWidth = 800;
    const int renderHeight = 600;

    // GRAVITY
    Vertex3D gravity = Vertex3D(0, -9.8f, 0);

    // Global ON/OFF Render settings
    bool ENABLE_SPLASH = true;
    bool ENABLE_FORWARD_RENDER = false;
    bool ENABLE_LIGHTS = true;
    bool ENABLE_SHADOW_MAPPING = true;
    bool ENABLE_DOF_BLUR = false;
    bool ENABLE_TRIANGLE_MODE_DEPTHMAP = false;
    bool ENABLE_GAMEPAD_CONTROLLER = true;
    bool ENABLE_FOG = false;
    bool ENABLE_BULLET_STEP_SIMULATION = false;
    bool ENABLE_SOUND = true;
    bool ENABLE_LOGGING_CONSOLE = true;
    bool ENABLE_LOGGING_STD = true;
    bool ENABLE_IMGUI = true;
    bool ENABLE_IMGUI_TOOLBAR = true;
    bool ENABLE_GRID_BACKGROUND = true;

    // Render options
    bool LIMIT_FRAMERATE = false;
    bool V_SYNC = true;
    int FRAMERATE = 60;

    float FRUSTUM_FARPLANE_DISTANCE = 1000;
    float HORIZONTAL_FOV = 90;

    // Draw axis
    bool RENDER_OBJECTS_AXIS = false;
    float OBJECT_AXIS_SIZE = 1.0f;

    // Render geometry modes
    bool TRIANGLE_MODE_PIXELS = false;
    bool TRIANGLE_MODE_WIREFRAME = false;
    bool TRIANGLE_MODE_SHADING = false;
    bool TRIANGLE_MODE_PICKING_COLORS = false;
    bool TRIANGLE_MODE_TEXTURIZED = true;

    // View options
    bool DRAW_ANIMATION_BONES = false;
    bool DRAW_MESH3D_AABB = false;
    bool DRAW_MESH3D_OCTREE = false;
    bool DRAW_MESH3D_GRID = false;
    bool DRAW_MESH3D_TEST_PASSED = true;
    bool DRAW_MESH3D_TEST_NOT_PASSED = false;
    bool DRAW_MESH3D_GRID_ASTAR = false;

    bool DRAW_LIGHTS_DIRECTION = false;
    bool FULLSCREEN = false;
    bool DRAW_FPS_RENDER = false;

    float FRUSTUM_CLIPPING_DISTANCE = 0.00075f;

    // ShadowMapping options
    bool SHADOW_MAPPING_DEBUG = false;
    float SHADOW_MAPPING_INTENSITY = 4.0f;
    bool SHADOW_MAPPING_ENABLE_DIRECTIONAL_LIGHT = true;
    float SHADOW_MAPPING_DEPTH_NEAR_PLANE = 1.0f;
    float SHADOW_MAPPING_DEPTH_FAR_PLANE = 200.0f;
    float SHADOW_MAPPING_FRUSTUM_SIZE = 30.0f;

    // SpriteDirectional3D Default size
    float BILLBOARD_WIDTH_DEFAULT = 100.f;
    float BILLBOARD_HEIGHT_DEFAULT = 100.f;

    // SOUND
    float SOUND_VOLUME_MUSIC = 40;
    float SOUND_VOLUME_FX = 50;

    // KEYBOARD
    float WALKING_SPEED = 0.25f;
    float STRAFE_SPEED = 0.25f;

    // MOUSE
    float MOUSE_SENSITIVITY = 0.075;
    bool MOUSE_LOOK = true;
    bool MOUSE_CLICK_SELECT_OBJECT3D = true;

    // COLLISION SYSTEM
    int BULLET_MAX_SUBSTEPS = 4;
    int BULLET_FIXED_TIME_STEPS = 60;
    bool BULLET_DEBUG_MODE = false;
    bool BULLET_CHECK_ALL_PAIRS = true;
    bool LOG_COLLISION_OBJECTS = false;

    float PLAYER_CAPSULE_RADIUS = 0.5f;
    float PLAYER_CAPSULE_HEIGHT = 1.8f;

    //LIGHTNING EFFECT
    float LIGHTNING_GENERATIONS = 5;
    float LIGHTNING_OFFSET_REDUCTION = 650.f;
    float LIGHTNING_PROBABILITY_BRANCH = 1.5f;
    float LIGHTNING_SEGMENT_SHIFT = 2.5f;
    float SPLASH_COUNTDOWN_TIME = 5.0f;
    bool SHOW_ABOUT_ME_MODAL = false;

    // CUSTOM SHADER TEMPLATES
    std::string TEMPLATE_SHADER_POSTPROCESSING_VS = TEMPLATES_FOLDER + "shader-postprocessing-template.vs";
    std::string TEMPLATE_SHADER_POSTPROCESSING_FS = TEMPLATES_FOLDER + "shader-postprocessing-template.fs";
    std::string TEMPLATE_SHADER_OBJECT_VS = TEMPLATES_FOLDER + "shader-object-template.vs";
    std::string TEMPLATE_SHADER_OBJECT_FS = TEMPLATES_FOLDER + "shader-object-template.fs";

    // SCRIPT TEMPLATE
    std::string TEMPLATE_SCRIPT = TEMPLATES_FOLDER + "script-template.lua";

    // GLOBAL ENUMS
    enum SoundChannels {
        SND_GLOBAL = -1,
    };

    typedef enum {
        LUA_STOP = 0,
        LUA_PLAY = 1,
    } LuaStateScripts;

    enum collisionGroups {
        Player = 1,
        Enemy = 2,
        Projectile = 4,
        ProjectileEnemy = 8,
        Health = 16,
        Weapon = 32,
        AllFilter = -1  //all bits sets: DefaultFilter | StaticFilter | KinematicFilter | DebrisFilter | SensorTrigger
    };

    struct DragDropCustomShaderData {
        const char* folder;
        const char* file;
    };

    enum ShaderSystemVariables {
        deltaTime = 0,
        executionTime = 1,
        diffuse = 2,
        specular = 3
    };

    enum CollisionSource {
        OBJECT_COLLIDER = 1,
        BONE_COLLIDER = 2
    };

    enum class ImGUIConfigs {
        DEFAULT,
        DESIGN,
        CODING
    };

    // DEV TESTING PURPOSE
    float TESTING_INT1 = 0.1f;
    float TESTING_INT2 = 0.03f;
    float TESTING_INT3 = 1;

    static int FindBlendModeIndex(GLenum mode) {
        for (int i = 0; i < 15; i++) {
            if (blend_values[i] == mode)
                return i;
        }
        return 0;
    }

    static inline const char* blend_modes[] = {
        "GL_ZERO",
        "GL_ONE",
        "GL_SRC_COLOR",
        "GL_ONE_MINUS_SRC_COLOR",
        "GL_DST_COLOR",
        "GL_ONE_MINUS_DST_COLOR",
        "GL_SRC_ALPHA",
        "GL_ONE_MINUS_SRC_ALPHA",
        "GL_DST_ALPHA",
        "GL_ONE_MINUS_DST_ALPHA",
        "GL_SRC_ALPHA_SATURATE",
        "GL_CONSTANT_COLOR",
        "GL_ONE_MINUS_CONSTANT_COLOR",
        "GL_CONSTANT_ALPHA",
        "GL_ONE_MINUS_CONSTANT_ALPHA"
    };

    static inline GLenum blend_values[] = {
        GL_ZERO,
        GL_ONE,
        GL_SRC_COLOR,
        GL_ONE_MINUS_SRC_COLOR,
        GL_DST_COLOR,
        GL_ONE_MINUS_DST_COLOR,
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_DST_ALPHA,
        GL_ONE_MINUS_DST_ALPHA,
        GL_SRC_ALPHA_SATURATE,
        GL_CONSTANT_COLOR,
        GL_ONE_MINUS_CONSTANT_COLOR,
        GL_CONSTANT_ALPHA,
        GL_ONE_MINUS_CONSTANT_ALPHA
    };

    struct LineCommandOptions {
        bool autoload = false;
        std::string project;
    };
};

#endif //SDL2_3D_ENGINE_ENGINESETUP_H