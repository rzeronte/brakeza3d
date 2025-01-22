#ifndef SDL2_3D_ENGINE_ENGINESETUP_H
#define SDL2_3D_ENGINE_ENGINESETUP_H

#include "Misc/Color.h"
#include "Objects/Vertex3D.h"
#include "Misc/Timer.h"
#include "../imgui/imgui.h"
#include "Objects/ScriptLUA.h"
#include <map>

class EngineSetup {

public:
    static EngineSetup *get();

    static EngineSetup *instance;

    bool LOGGING = false;

    bool IMGUI_ENABLED = true;

    std::string ENGINE_VERSION = "v.1.0.0";

    std::string ENGINE_TITLE = "Brakeza3D Application";

    std::string cameraNameIdentifier = "Camera";
    std::string cameraTriggerNameIdentifier = "triggerCamera";

    const std::string iconApplication = "application.png";

    Vertex3D right = Vertex3D(1, 0, 0);
    Vertex3D up = Vertex3D(0, 1, 0);
    Vertex3D forward = Vertex3D(0, 0, 1);

    Vertex3D left = right.getInverse();
    Vertex3D down = up.getInverse();
    Vertex3D backward = forward.getInverse();

    Vertex3D gravity = Vertex3D(0, 9.8f, 0);

    // Screen dimension constants
    const int screenWidth = 1280;
    const int screenHeight = 720;

    bool LIMIT_FRAMERATE = true;
    bool V_SYNC = true;
    int FRAMERATE = 60;

    bool ENABLE_FOG = true;
    float FRUSTUM_FARPLANE_DISTANCE = 1000;
    float HORIZONTAL_FOV = 90;

    // Draw axis
    bool RENDER_MAIN_AXIS = false;
    bool RENDER_OBJECTS_AXIS = false;
    float OBJECT_AXIS_SIZE = 1;

    // Fill Triangle modes
    bool TRIANGLE_MODE_PIXELS = false;
    bool TRIANGLE_MODE_WIREFRAME = false;
    bool TRIANGLE_MODE_COLOR_SOLID = false;
    bool TRIANGLE_MODE_TEXTURIZED = true;

    bool DRAW_ANIMATION_BONES = false;

    bool DRAW_MESH3D_AABB = false;
    bool DRAW_MESH3D_OCTREE = false;
    bool DRAW_MESH3D_GRID = false;
    bool DRAW_MESH3D_GRID_POINTS = true;
    bool DRAW_MESH3D_GRID_CUBES = false;
    bool DRAW_MESH3D_GRID_EMPTY = true;
    bool DRAW_MESH3D_GRID_NO_EMPTY = true;

    // Z BUFFER
    bool ENABLE_LIGHTS = true;
    bool DRAW_LIGHTS_DIRECTION = false;
    float LIGHTS_DIRECTION_SIZE = 2500;
    bool FULLSCREEN = false;
    bool DRAW_FPS = false;

    // CONFIG VALUES
    float FRUSTUM_CLIPPING_DISTANCE = 0.00075f;

    std::string ROOT_FOLDER = "../";
    std::string ASSETS_FOLDER = "../assets/";

    std::string SPRITES_FOLDER = ASSETS_FOLDER + "sprites/";
    std::string MAPS_FOLDER = ASSETS_FOLDER + "maps/";
    std::string IMAGES_FOLDER = ASSETS_FOLDER + "images/";
    std::string VIDEOS_FOLDER = ASSETS_FOLDER + "videos/";
    std::string SOUNDS_FOLDER = ASSETS_FOLDER + "sounds/";
    std::string MODELS_FOLDER = ASSETS_FOLDER + "models/";
    std::string ANIMATIONS_FOLDER = ASSETS_FOLDER + "animations/";

    std::string FONTS_FOLDER = ASSETS_FOLDER + "fonts/";
    std::string TEXTURES_FOLDER = ASSETS_FOLDER + "textures/";
    std::string GRIDS_FOLDER = ASSETS_FOLDER + "grids/";
    std::string ICONS_FOLDER = ASSETS_FOLDER + "icons/";
    std::string SCRIPTS_FOLDER = ASSETS_FOLDER + "scripts/";
    std::string SCENES_FOLDER = ASSETS_FOLDER + "scenes/";
    std::string CUSTOM_SHADERS_FOLDER = ASSETS_FOLDER + "shaders/";

    std::string SHADERS_FOLDER = "../GLSL/";

    bool SOUND_ENABLED = true;
    float SOUND_VOLUME_MUSIC = 40;
    float SOUND_CHANNEL_GLOBAL = 50;

    // SpriteDirectional3D Default size
    float BILLBOARD_WIDTH_DEFAULT = 100.f;
    float BILLBOARD_HEIGHT_DEFAULT = 100.f;

    // KEYBOARD
    float WALKING_SPEED = 1.0f;
    float TURN_SPEED = 0.5f;
    float PITCH_SPEED = 0.5f;
    float STRAFE_SPEED = 1.0f;

    // MOUSE
    float MOUSE_SENSITIVITY = 0.250;

    // COLLISION SYSTEM
    bool BULLET_STEP_SIMULATION = true;
    bool BULLET_DEBUG_MODE = false;
    bool BULLET_CHECK_ALL_PAIRS = true;

    float TESTING_INT1 = 0.1f;
    float TESTING_INT2 = 0.03f;
    float TESTING_INT3 = 1;
    float TESTING_INT4 = 0.1f;

    bool ENABLE_DEPTH_OF_FIELD = true;
    bool SHOW_DEPTH_OF_FIELD = false;

    //LIGHTNING EFFECT
    float LIGHTNING_GENERATIONS = 5;
    float LIGHTNING_OFFSET_REDUCTION = 650.f;
    float LIGHTNING_PROBABILITY_BRANCH = 1.5f;
    float LIGHTNING_SEGMENT_SHIFT = 2.5f;

    bool LOG_COLLISION_OBJECTS = false;

    float PLAYER_CAPSULE_RADIUS = 16.00f;
    float PLAYER_CAPSULE_HEIGHT = 32.00f;

    float PROJECTILE_DEMO_IMPULSE = 500;
    float PROJECTILE_DEMO_ACCURACY = 100;
    float PROJECTILE_DEMO_MASS = 100;
    bool PROJECTILE_SIMPLE_MESH = true;

    bool CLICK_SELECT_OBJECT3D = false;
    bool GAMEPAD_CONTROLLER_ENABLED = true;

    std::string CONFIG_FOLDER = "../config/";
    std::string CFG_SOUNDS = "sounds.json";

    std::string LOGO_BRAKEZA = "logo.png";

    enum SoundChannels {
        SND_GLOBAL = -1,
    };

    typedef enum {
        LUA_STOP = 0,
        LUA_PLAY = 1,
    } LuaStateScripts;

    enum class LUADataType {
        INT,
        FLOAT,
        VERTEX3D,
        STRING
    };

    std::map<std::string, LUADataType> LUADataTypesMapping = {
        {"int", LUADataType::INT},
        {"float", LUADataType::FLOAT},
        {"string", LUADataType::STRING},
        {"Vertex3D", LUADataType::VERTEX3D},
    };

    enum collisionGroups {
        Player = 1,
        Enemy = 2,
        Projectile = 4,
        ProjectileEnemy = 8,
        Health = 16,
        Weapon = 32,
        AllFilter = -1  //all bits sets: DefaultFilter | StaticFilter | KinematicFilter | DebrisFilter | SensorTrigger
    };
};

#endif //SDL2_3D_ENGINE_ENGINESETUP_H