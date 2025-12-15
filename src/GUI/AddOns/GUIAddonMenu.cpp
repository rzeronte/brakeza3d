//
// Created by Eduardo on 01/12/2025.
//

#include "../../../include/GUI/AddOns/GUIAddonMenu.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Brakeza.h"
#include "../../../include/Misc/Logging.h"
#include "../../../include/Misc/ToolsMaths.h"
#include "../../../include/Serializers/Object3DSerializer.h"
#include "../../../include/Serializers/LightPointSerializer.h"
#include "../../../include/Serializers/LightSpotSerializer.h"
#include "../../../include/Serializers/ParticleEmmitterSerializer.h"
#include "../../../include/Serializers/Image3DAnimation360Serializer.h"
#include "../../../include/Serializers/Mesh3DAnimationSerializer.h"
#include "../../../include/Serializers/Mesh3DSerializer.h"
#include "../../../include/Serializers/Image3DSerializer.h"
#include "../../../include/Serializers/Image2DSerializer.h"
#include "../../../include/Serializers/Image2DAnimationSerializer.h"
#include "../../../include/Serializers/Image3DAnimationSerializer.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"

GUIAddonMenu::GUIAddonMenu()
{
}

void GUIAddonMenu::Draw(GUIManager *gui)
{
    if (ImGui::BeginMainMenuBar()) {
        for (const auto & menu : gui->menus) {
            if (ImGui::BeginMenu(menu.label.c_str())) {
                menu.cb();
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }
    AboutMeModal();
}

void GUIAddonMenu::MenuBrakeza3D()
{
    ImGui::Image(FileSystemGUI::Icon(IconGUI::BRAKEZA_ABOUT_ME), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    if (ImGui::MenuItem("About Brakeza3D")) {
        Config::get()->SHOW_ABOUT_ME_MODAL = true;
    }

    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::BRAKEZA_EXIT), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    if (ImGui::MenuItem("Exit")) Config::get()->EXIT = true;
}

void GUIAddonMenu::MenuScriptControls()
{
    auto scripting = Components::get()->Scripting();
    auto state = scripting->getStateLUAScripts();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::LUA_PLAY), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::MenuItem("Play scripts", "F1", false,  state != Config::LuaStateScripts::LUA_PLAY)) {
        scripting->PlayLUAScripts();
    }
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::LUA_STOP), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::MenuItem("Stop scripts", "F1", false, state != Config::LuaStateScripts::LUA_STOP)) {
        scripting->StopLUAScripts();
    }
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::LUA_RELOAD), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::MenuItem("Reload scripts", "F2")) {
        scripting->ReloadLUAScripts();
    }
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::CLEAR_SCENE), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::MenuItem("Clear scene objects", "F3")) {
        SceneLoader::ClearScene();
    }
}

void GUIAddonMenu::MenuAddObject()
{
    auto setup = Config::get();

    ImGui::Image(FileSystemGUI::Icon(IconObject::OBJECT_3D), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::MenuItem("Object3D (Empty)")) {
        Object3DSerializer().MenuLoad("");
    }
    ImGui::SeparatorText("2D Objects");
    ImGui::Image(FileSystemGUI::Icon(IconObject::IMAGE_2D), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::BeginMenu("Image2D")) {
        DrawItemsToLoad(setup->IMAGES_FOLDER, Config::get()->IMAGES_EXT, IconObject::IMAGE_2D, [](const std::string& path) { Image2DSerializer().MenuLoad(path); });
        ImGui::EndMenu();
    }
    ImGui::Image(FileSystemGUI::Icon(IconObject::IMAGE_2D_ANIMATION), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::BeginMenu("Image2D Animation")) {
        DrawItemsToLoad(setup->SPRITES_FOLDER, Config::get()->IMAGES_EXT, IconObject::IMAGE_2D_ANIMATION, [](const std::string& path) { Image2DAnimationSerializer().MenuLoad(path); });
        ImGui::EndMenu();
    }
    ImGui::SeparatorText("3D Objects");
    ImGui::Image(FileSystemGUI::Icon(IconObject::IMAGE_3D), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::BeginMenu("Image3D")) {
        DrawItemsToLoad(setup->IMAGES_FOLDER, Config::get()->IMAGES_EXT, IconObject::IMAGE_3D ,[](const std::string& path) { Image3DSerializer().MenuLoad(path); });
        ImGui::EndMenu();
    }
    ImGui::Image(FileSystemGUI::Icon(IconObject::IMAGE_3D_ANIMATION), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::BeginMenu("Image3D Animation")) {
        DrawItemsToLoad(setup->SPRITES_FOLDER, Config::get()->IMAGES_EXT, IconObject::IMAGE_3D_ANIMATION, [](const std::string& path) { Image3DAnimationSerializer().MenuLoad(path); });
        ImGui::EndMenu();
    }
    ImGui::Image(FileSystemGUI::Icon(IconObject::IMAGE_3D_ANIMATION_360), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::MenuItem("Image3D Animation 360")) {
        Image3DAnimation360Serializer().MenuLoad("");
        ImGui::EndMenu();
    }
    ImGui::Image(FileSystemGUI::Icon(IconObject::MESH_3D), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::BeginMenu("Mesh3D")) {
        DrawItemsToLoad(setup->MODELS_FOLDER, Config::get()->MESH3D_EXT, IconObject::MESH_3D, [](const std::string& path) { Mesh3DSerializer().MenuLoad(path); });
        ImGui::EndMenu();
    }
    ImGui::Image(FileSystemGUI::Icon(IconObject::MESH_3D_ANIMATION), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::BeginMenu("Mesh3DAnimation")) {
        DrawItemsToLoad(setup->ANIMATIONS_FOLDER, Config::get()->MESH3D_EXT, IconObject::MESH_3D_ANIMATION, [](const std::string& path) { Mesh3DAnimationSerializer().MenuLoad(path); });
        ImGui::EndMenu();
    }
    ImGui::SeparatorText("Lights");
    ImGui::Image(FileSystemGUI::Icon(IconObject::LIGHT_POINT), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::MenuItem("PointLight")) {
        LightPointSerializer().MenuLoad("");
    }
    ImGui::Image(FileSystemGUI::Icon(IconObject::LIGHT_SPOT), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::MenuItem("SpotLight")) {
        LightSpotSerializer().MenuLoad("");
    }
    ImGui::SeparatorText("Emitters");
    ImGui::Image(FileSystemGUI::Icon(IconObject::PARTICLE_EMITTER), GUIType::Sizes::ICON_SIZE_MENUS);
    ImGui::SameLine();
    if (ImGui::MenuItem("ParticleEmitter")) {
        ParticleEmmitterSerializer().MenuLoad("");
        ImGui::EndMenu();
    }
}

void GUIAddonMenu::MenuVideo()
{
    auto setup = Config::get();

    ImGui::SeparatorText("Framerate");
    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_VSYNC), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("V-Sync", nullptr, &setup->V_SYNC);
    if (ImGui::IsItemEdited()) {
        if (setup->V_SYNC) {
            Logging::Message("Set V-Sync enabled");
            SDL_GL_SetSwapInterval(1);
            SDL_RenderSetVSync(Components::get()->Window()->getRenderer(), 1);
        } else {
            Logging::Message("Set V-Sync disabled");
            SDL_GL_SetSwapInterval(0);
            SDL_RenderSetVSync(Components::get()->Window()->getRenderer(), 0);
        }
    }
    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_LIMIT_FRAMERATE), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Limit frame rate", nullptr,  &setup->LIMIT_FRAMERATE);
    if (setup->LIMIT_FRAMERATE) {
        const int range_framerate_sensibility = 1;
        ImGui::DragScalar("Max FPS:", ImGuiDataType_S32, &setup->FRAMERATE, range_framerate_sensibility, &GUIType::Levels::DRAG_FRAMERATE_MIN, &GUIType::Levels::DRAG_FRAMERATE_MAX, "%d", 1.0f);

    }
    ImGui::SeparatorText("Render modes");
    ImGui::Image(FileSystemGUI::Icon(IconGUI::RENDER_PIXELS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Vertex", nullptr, &setup->TRIANGLE_MODE_PIXELS);
    ImGui::Image(FileSystemGUI::Icon(IconGUI::RENDER_WIRE), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("WireFrame",nullptr,  &setup->TRIANGLE_MODE_WIREFRAME);
    ImGui::Image(FileSystemGUI::Icon(IconGUI::RENDER_SHADING), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Solid", nullptr,  &setup->TRIANGLE_MODE_SHADING);
    ImGui::Image(FileSystemGUI::Icon(IconGUI::RENDER_TEXTURE), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Textures",nullptr, &setup->TRIANGLE_MODE_TEXTURIZED);

    const float focalMinValues = 0;
    const float focalMaxValues = 1;
    const float focalValueSens = 0.001;

    ImGui::SeparatorText("Post-processing");
    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_DOF), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Enable DOF", nullptr, &Config::get()->ENABLE_DOF_BLUR);
    if (Config::get()->ENABLE_DOF_BLUR) {
        const float depthValueSens = 0.1;
        const float depthMinValues = 0;
        const float depthMaxValues = 100;
        ImGui::DragScalar("Focal range", ImGuiDataType_Float, &Components::get()->Render()->getShaders()->shaderOGLDOFBlur->focalRange, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
        ImGui::DragScalar("Focal distance", ImGuiDataType_Float, &Components::get()->Render()->getShaders()->shaderOGLDOFBlur->focalDistance, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);

        const int minBlurRadius = 0;
        const int maxBlurRadius = 10;
        ImGui::DragScalar("Blur radius", ImGuiDataType_S32, &Components::get()->Render()->getShaders()->shaderOGLDOFBlur->blurRadius,1.0f, &minBlurRadius, &maxBlurRadius, "%d", 1.0f);
        ImGui::DragScalar("Intensity", ImGuiDataType_Float, &Components::get()->Render()->getShaders()->shaderOGLDOFBlur->intensity, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
        ImGui::DragScalar("Far Plane", ImGuiDataType_Float, &Components::get()->Render()->getShaders()->shaderOGLDOFBlur->farPlane, depthValueSens, &depthMinValues, &depthMaxValues, "%f", 1.0f);
    }

    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_FOG), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Enable FOG", nullptr, &Config::get()->ENABLE_FOG);
    if (Config::get()->ENABLE_FOG) {
        const float rangeFogSens = 0.1;
        const float rangeFogMin = 0.1;
        const float rangeFogMax = 1000;
        const float rangeFogIntensityMax= 1.0;

        ImGui::DragScalar("FOG min distance", ImGuiDataType_Float, &Components::get()->Render()->getShaders()->shaderOGLFOG->fogMinDist, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);
        ImGui::DragScalar("FOG max distance", ImGuiDataType_Float, &Components::get()->Render()->getShaders()->shaderOGLFOG->fogMaxDist, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);
        ImGui::DragScalar("FOG intensity", ImGuiDataType_Float, &Components::get()->Render()->getShaders()->shaderOGLFOG->intensity, rangeFogSens, &rangeFogMin, &rangeFogIntensityMax, "%f", 1.0f);

        auto p = Components::get()->Render()->getShaders()->shaderOGLFOG->fogColor;
        ImVec4 fogVecColor = {p.r, p.g, p.b, 1};
        if (ImGui::ColorEdit4("FOG Color##", reinterpret_cast<float *>(&fogVecColor), ImGuiColorEditFlags_NoOptions)) {
            Components::get()->Render()->getShaders()->shaderOGLFOG->fogColor = {fogVecColor.x, fogVecColor.y, fogVecColor.z};
        }
    }
    ImGui::SeparatorText("Screen helpers");
    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_SHADER_GRID), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Draw Grid", nullptr, &setup->ENABLE_GRID_BACKGROUND);
    if (setup->ENABLE_GRID_BACKGROUND) {
        auto shaderGrid = Components::get()->Render()->getShaders()->shaderOGLGrid;
        const float sizeMin = 0; const float sizeMax = 100; const float sizeSens = 0.1;
        const float alphaMin = 0; const float alphaMax = 1; const float alphaSens = 0.01;
        ImGui::DragScalar("Size grid", ImGuiDataType_Float, &shaderGrid->gridSize, sizeSens, &sizeMin, &sizeMax, "%f", 1.0f);
        ImGui::DragScalar("Alpha grid", ImGuiDataType_Float, &shaderGrid->opacity, alphaSens, &alphaMin, &alphaMax, "%f", 1.0f);
        auto p = shaderGrid->color;
        ImVec4 vecColor = {p.r, p.g, p.b, 1};
        if (ImGui::ColorEdit4("Grid Color##", reinterpret_cast<float *>(&vecColor), ImGuiColorEditFlags_NoOptions)) {
            shaderGrid->color = {vecColor.x, vecColor.y, vecColor.z};
        }
        ImGui::Separator();
    }

    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_OBJECT_AXIS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Draw axis objects", nullptr, &setup->RENDER_OBJECTS_AXIS);
    if (setup->RENDER_OBJECTS_AXIS) {
        const float sizeAxisMin = 0;
        const float sizeAxisMax = 1;
        const float sizeAxisSens = 0.01;
        ImGui::DragScalar("Size Axis", ImGuiDataType_Float, &setup->OBJECT_AXIS_SIZE, sizeAxisSens, &sizeAxisMin, &sizeAxisMax, "%f", 1.0f);
    }
    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_AABB), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Draw Mesh3D AABB", nullptr, &setup->DRAW_MESH3D_AABB);
    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_OCTREE), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Draw Mesh3D octree", nullptr, &setup->DRAW_MESH3D_OCTREE);
    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_GRID), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Draw Mesh3D grid", nullptr, &setup->DRAW_MESH3D_GRID);
    if (setup->DRAW_MESH3D_GRID) {
        ImGui::Separator();
        ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_GRID), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::MenuItem("Draw Test Passed Cells", nullptr, &setup->DRAW_MESH3D_TEST_PASSED);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_GRID), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::MenuItem("Draw Not Test Passed Cells", nullptr, &setup->DRAW_MESH3D_TEST_NOT_PASSED);
        ImGui::Separator();
        ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_GRID), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::MenuItem("Draw A* Travel", nullptr, &setup->DRAW_MESH3D_GRID_ASTAR);
    }
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_PICKING_COLORS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Picking colors", nullptr, &setup->TRIANGLE_MODE_PICKING_COLORS);
    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_FPS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Show FPS", nullptr, &setup->DRAW_FPS_RENDER);
    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_SHOW_BONES), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Draw bones", nullptr, &setup->DRAW_ANIMATION_BONES);
    ImGui::SeparatorText("Tools");
    ImGui::Image(FileSystemGUI::Icon(IconGUI::VIDEO_TAKE_SCREENSHOT), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    if (ImGui::MenuItem("Screenshot", nullptr, false)) {
        ComponentRender::MakeScreenShot();
    }
}

void GUIAddonMenu::MenuColliders()
{
    auto setup = Config::get();

    int minSubsteps = 0;
    int maxSubsteps = 10;
    int minFixedTime = 1;
    int maxFixedTime = 60;

    ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDERS_COLLISION_SYSTEM), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Enable Collisions System", nullptr, &setup->ENABLE_BULLET_STEP_SIMULATION);
    if (!setup->ENABLE_BULLET_STEP_SIMULATION) return;
    ImGui::Separator();

    ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDERS_HANDLE_OBJECTS_COLLISIONS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Handle object collisions", nullptr, &setup->BULLET_CHECK_ALL_PAIRS);
    ImGui::SeparatorText("Step simulation setup");
    ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDERS_MAX_SUBSTEPS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::DragScalar("Max sub-steps", ImGuiDataType_S32, &setup->BULLET_MAX_SUBSTEPS, 1, &minSubsteps, &maxSubsteps, "%d", 1.0f);
    ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDERS_FIXED_TIMESTEP), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::DragScalar("Fixed time step (1/x)", ImGuiDataType_S32, &setup->BULLET_FIXED_TIME_STEPS, 1, &minFixedTime, &maxFixedTime, "%d", 1.0f);

    ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDERS_GRAVITY), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    float vec3f[3];
    setup->gravity.toFloat(vec3f);
    if (ImGui::DragFloat3("Gravity", vec3f, 0.01f, -1000.0f, 1000.0f)) {
        setup->gravity.x = vec3f[0];
        setup->gravity.y = vec3f[1];
        setup->gravity.z = vec3f[2];
        Components::get()->Collisions()->setGravity(setup->gravity);
    }

    ImGui::SeparatorText("Screen helpers");
    ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDERS_DEBUG_MODE), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    if (ImGui::MenuItem("Draw debug mode", nullptr, &setup->BULLET_DEBUG_MODE)) {
        Components::get()->Collisions()->setEnableDebugMode(setup->BULLET_DEBUG_MODE);
    }
    ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDERS_DEMO_APP), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Launch demo objects", nullptr, &setup->PROJECTILE_SIMPLE_MESH);

    if (setup->PROJECTILE_SIMPLE_MESH) {
        const float range_sensibility = 0.75f;
        ImGui::Separator();
        ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDERS_DEMO_APP), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::DragScalar("Objects impulse", ImGuiDataType_Float, &setup->PROJECTILE_DEMO_IMPULSE, range_sensibility, &GUIType::Levels::DRAG_UNIT_MIN, &GUIType::Levels::DRAG_UNIT_MAX, "%f", 1.0f);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDERS_DEMO_APP), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::DragScalar("Objects accuracy", ImGuiDataType_Float, &setup->PROJECTILE_DEMO_ACCURACY, range_sensibility, &GUIType::Levels::DRAG_UNIT_MIN, &GUIType::Levels::DRAG_UNIT_MAX, "%f", 1.0f);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::COLLIDERS_DEMO_APP), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::DragScalar("Objects mass", ImGuiDataType_Float, &setup->PROJECTILE_DEMO_MASS, range_sensibility, &GUIType::Levels::DRAG_UNIT_MIN, &GUIType::Levels::DRAG_UNIT_MAX, "%f", 1.0f);
    }
}

void GUIAddonMenu::MenuIllumination()
{
    auto setup = Config::get();

    ImGui::Image(FileSystemGUI::Icon(IconGUI::ILLUMINATION_ENABLE_SYSTEM), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Enable Lights System", nullptr, &setup->ENABLE_LIGHTS);

    if (!setup->ENABLE_LIGHTS) return;

    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::ILLUMINATION_ENABLE_SHADOW_MAPPING), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Enable Shadow Mapping", nullptr, &setup->ENABLE_SHADOW_MAPPING);
    if (setup->ENABLE_SHADOW_MAPPING) {
        ImGui::SeparatorText("Shadow Mapping setup");
        ImGui::Image(FileSystemGUI::Icon(IconGUI::ILLUMINATION_ENABLE_SHADOW_MAPPING), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::DragFloat("Shadows Intensity", &setup->SHADOW_MAPPING_INTENSITY, 0.1f, -5.0f, 5.0f);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::ILLUMINATION_ENABLE_SHADOW_MAPPING), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::DragFloat("DepthMaps Near plane", &setup->SHADOW_MAPPING_DEPTH_NEAR_PLANE, 0.1f, 1.0f, 500.0f);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::ILLUMINATION_ENABLE_SHADOW_MAPPING), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::DragFloat("DepthMaps Far plane", &setup->SHADOW_MAPPING_DEPTH_FAR_PLANE, 0.1f, 1.0f, 500.0f);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::ILLUMINATION_ENABLE_SHADOW_MAPPING), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::DragFloat("DepthMaps Frustum Size", &setup->SHADOW_MAPPING_FRUSTUM_SIZE, 0.1f, 100.0f);
    }
    ImGui::SeparatorText("Sun light setup");
    auto& dirLight = Components::get()->Render()->getShaders()->shaderOGLRender->getDirectionalLight();

    ImGui::Image(FileSystemGUI::Icon(IconGUI::ILLUMINATION_SUN_DIRECTION), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::DragFloat3("Sun direction", &dirLight.direction[0], 0.01f, -1.0f, 1.0f);

    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::ILLUMINATION_COLORS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImVec4 colorAmbient = {dirLight.ambient.x, dirLight.ambient.y, dirLight.ambient.z, 1};
    if (ImGui::ColorEdit4("Ambient##", reinterpret_cast<float *>(&colorAmbient), ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoInputs)) {
        dirLight.ambient = {colorAmbient.x, colorAmbient.y, colorAmbient.z};
    }
    ImGui::SameLine();
    ImVec4 colorSpecular = {dirLight.specular.x, dirLight.specular.y, dirLight.specular.z, 1};
    if (ImGui::ColorEdit4("Specular##", reinterpret_cast<float *>(&colorSpecular), ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoInputs)) {
        dirLight.specular = {colorSpecular.x, colorSpecular.y, colorSpecular.z};
    }
    ImVec4 colorDiffuse = {dirLight.diffuse.x, dirLight.diffuse.y, dirLight.diffuse.z, 1};
    ImGui::SameLine();
    if (ImGui::ColorEdit4("Diffuse##", reinterpret_cast<float *>(&colorDiffuse), ImGuiColorEditFlags_None | ImGuiColorEditFlags_NoInputs)) {
        dirLight.diffuse = {colorDiffuse.x, colorDiffuse.y, colorDiffuse.z};
    }
    if (setup->ENABLE_SHADOW_MAPPING) {
        ImGui::Separator();
        ImGui::Image(FileSystemGUI::Icon(IconGUI::ILLUMINATION_ENABLE_SUN_SHADOWS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::MenuItem("Enable Sun shadows", nullptr, &setup->SHADOW_MAPPING_ENABLE_DIRECTIONAL_LIGHT);
    }
    ImGui::SeparatorText("Screen helpers");
    ImGui::Image(FileSystemGUI::Icon(IconGUI::ILLUMINATION_SHADOW_MAPPING_DEBUG), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Shadow Mapping debug mode", nullptr, &setup->SHADOW_MAPPING_DEBUG);
    if (setup->ENABLE_LIGHTS && setup->TRIANGLE_MODE_TEXTURIZED) {
        ImGui::Image(FileSystemGUI::Icon(IconGUI::WIN_DEPTH_LIGHTS_MAPS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::MenuItem("Show Depth Map", nullptr, &setup->ENABLE_TRIANGLE_MODE_DEPTHMAP);
    }

    if (setup->ENABLE_TRIANGLE_MODE_DEPTHMAP) {
        auto s = Components::get()->Render()->getShaderOGLDepthMap();
        ImGui::Image(FileSystemGUI::Icon(IconGUI::WIN_DEPTH_LIGHTS_MAPS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::DragFloat("Intensity DepthMap", &s->intensity, 0.01f, 0.0f, 10.0f);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::WIN_DEPTH_LIGHTS_MAPS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::DragFloat("Far Plane", &s->farPlane, 0.01f, 0.0f, 100.0f);
        ImGui::Image(FileSystemGUI::Icon(IconGUI::WIN_DEPTH_LIGHTS_MAPS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::DragFloat("Near Plane", &s->nearPlane, 0.01f, 0.0f, 100.0f);
        ImGui::Separator();
    }
}

void GUIAddonMenu::MenuCamera()
{
    auto setup = Config::get();
    auto camera = Components::get()->Camera()->getCamera();

    ImGui::SeparatorText("Camera setup");
    ImGui::Image(FileSystemGUI::Icon(IconGUI::CAMERA_FOV), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::DragScalar("FOV", ImGuiDataType_Float, &setup->HORIZONTAL_FOV, 1, &GUIType::Levels::DRAG_FOV_MIN, &GUIType::Levels::DRAG_FOV_MAX, "%f", 1.0f);
    const float range_far_plane_distance_sensibility = 1.0f;
    ImGui::DragScalar("Frustum Far Plane Distance", ImGuiDataType_Float, &setup->FRUSTUM_FARPLANE_DISTANCE, range_far_plane_distance_sensibility, &GUIType::Levels::DRAG_FAR_PLANE_MIN, &GUIType::Levels::DRAG_FAR_PLANE_MIN, "%f", 1.0f);
    // position
    ImGui::Image(FileSystemGUI::Icon(IconGUI::CAMERA_POSITION), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    float vec3f[3];
    camera->getPosition().toFloat(vec3f);
    if (ImGui::DragFloat3("Position", vec3f, 0.1f, -10000.0f, 10000.0f)) {
        camera->positionPointer().x = vec3f[0];
        camera->positionPointer().y = vec3f[1];
        camera->positionPointer().z = vec3f[2];
    }

    float oldPitch = camera->getRotation().getPitchDegree();
    float oldYaw = camera->getRotation().getYawDegree();
    float oldRoll = camera->getRotation().getRollDegree();
    float pitch = oldPitch;
    float yaw = oldYaw;
    float roll = oldRoll;

    vec3f[0] = pitch;
    vec3f[1] = yaw;
    vec3f[2] = roll;
    ImGui::Image(FileSystemGUI::Icon(IconGUI::CAMERA_ROTATION), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    if (ImGui::DragFloat3("Rotation", vec3f, 0.01f, -999999.0f, 999999.0f)) {
        const float factor = 0.0025f;
        pitch = vec3f[0];
        yaw = vec3f[1];
        roll = vec3f[2];
        if (abs(pitch - oldPitch) > 0) {
            auto partialRotX = M3::arbitraryAxis(camera->getRotation().X(), ToolsMaths::RadiansToDegrees(pitch - oldPitch) * factor);
            camera->setRotation(camera->getRotation() * partialRotX);
            M3::normalize(camera->rotationPointer());
        }

        if (abs(yaw - oldYaw) > 0) {
            auto partialRotY = M3::arbitraryAxis(camera->getRotation().Y(), ToolsMaths::RadiansToDegrees(yaw - oldYaw) * factor);
            camera->setRotation(camera->getRotation() * partialRotY);
            M3::normalize(camera->rotationPointer());
        }

        if (abs(roll - oldRoll) > 0) {
            auto partialRotZ = M3::arbitraryAxis(camera->getRotation().Z(), ToolsMaths::RadiansToDegrees(roll - oldRoll) * factor);
            camera->setRotation(camera->getRotation() * partialRotZ);
            M3::normalize(camera->rotationPointer());
        }
    }
    ImGui::SeparatorText("Keyboard controls setup");
    ImGui::Image(FileSystemGUI::Icon(IconGUI::CAMERA_CURSOR_SPEED_WALKING), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::DragFloat("Cursors speed walking", &Config::get()->WALKING_SPEED, 0.01f, 0.0f, 500.0f);
    ImGui::Image(FileSystemGUI::Icon(IconGUI::CAMERA_CURSOR_SPEED_STRAFE), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::DragFloat("Cursors speed strafe", &Config::get()->STRAFE_SPEED, 0.01f, 0.0f, 500.0f);
    ImGui::SeparatorText("Camera GUI features");
    ImGui::Image(FileSystemGUI::Icon(IconGUI::CAMERA_CLICK_OBJECTS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Enable click objects", nullptr, &setup->MOUSE_CLICK_SELECT_OBJECT3D);

    ImGui::Image(FileSystemGUI::Icon(IconGUI::CAMERA_MOUSE_LOOK), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Enable Mouse Look (Right Button)", nullptr, &Config::get()->MOUSE_LOOK);
    if (Config::get()->MOUSE_LOOK) {
        ImGui::Image(FileSystemGUI::Icon(IconGUI::CAMERA_MOUSE_LOOK), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::DragFloat("Sensibility", &Config::get()->MOUSE_SENSITIVITY, 0.001f, 0.0f, 1.0f);
    }
}

void GUIAddonMenu::MenuSound()
{
    auto setup = Config::get();

    const float range_sensibility_volume = 1;
    ImGui::Image(FileSystemGUI::Icon(IconGUI::SOUND_ENABLE_SYSTEM), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Global Sound System", nullptr, &setup->ENABLE_SOUND);
    ImGui::Separator();
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        if (!setup->ENABLE_SOUND) {
            Mix_Volume(Config::SoundChannels::SND_GLOBAL, 0);
            Mix_VolumeMusic(static_cast<int>(setup->SOUND_VOLUME_MUSIC));
            Mix_VolumeMusic(0);
        } else {
            Mix_Volume(Config::SoundChannels::SND_GLOBAL, static_cast<int>(setup->SOUND_CHANNEL_GLOBAL));
            Mix_VolumeMusic(static_cast<int>(setup->SOUND_VOLUME_MUSIC));
        }
    }

    ImGui::Image(FileSystemGUI::Icon(IconGUI::SOUND_MUSIC_VOLUME), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::DragScalar("Music volume", ImGuiDataType_Float, &setup->SOUND_VOLUME_MUSIC, range_sensibility_volume, &GUIType::Levels::DRAG_VOLUME_MIN, &GUIType::Levels::DRAG_VOLUME_MAX, "%f", 1.0f);
    if (ImGui::IsItemEdited()) { Mix_VolumeMusic(static_cast<int>(setup->SOUND_VOLUME_MUSIC)); }

    ImGui::Image(FileSystemGUI::Icon(IconGUI::SOUND_GLOBAL_CHANNEL_VOLUME), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::DragScalar("Global Channel volume", ImGuiDataType_Float, &setup->SOUND_CHANNEL_GLOBAL, range_sensibility_volume, &GUIType::Levels::DRAG_VOLUME_MIN, &GUIType::Levels::DRAG_VOLUME_MAX, "%f", 1.0f);
    if (ImGui::IsItemEdited()) {
        Mix_Volume(Config::SoundChannels::SND_GLOBAL, static_cast<int>(setup->SOUND_CHANNEL_GLOBAL));
    }
}

void GUIAddonMenu::MenuLogging()
{
    auto setup = Config::get();

    ImGui::Image(FileSystemGUI::Icon(IconGUI::LOGGING_OUTPUT_CONSOLE), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Output to Console", nullptr, &setup->ENABLE_LOGGING);
    ImGui::Image(FileSystemGUI::Icon(IconGUI::LOGGING_OUTPUT_STD), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Output to STD", nullptr, &setup->ENABLE_LOGGING_STD);
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::LOGGING_COLLIDER), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Log collider collisions", nullptr, &setup->LOG_COLLISION_OBJECTS);
}

void GUIAddonMenu::MenuLayout()
{
    auto setup = Config::get();

    ImGui::Image(FileSystemGUI::Icon(IconGUI::LAYOUTS_ENABLE_GUI), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("Enable/Disable GUI (F4)", nullptr, &setup->ENABLE_IMGUI);
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::LAYOUTS_ENABLE_TOOLBAR), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("GUI Toolbar", nullptr, &setup->ENABLE_IMGUI_TOOLBAR);

    ImGui::SeparatorText("Layout modes");
    ImGui::Image(FileSystemGUI::Icon(IconGUI::LAYOUTS_LAYOUT_DEFAULT), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    if (ImGui::MenuItem("Default", "F5")) {
        Brakeza::get()->GUI()->setLayoutToDefault(Config::ImGUIConfigs::DEFAULT);
    }
    ImGui::Image(FileSystemGUI::Icon(IconGUI::LAYOUTS_LAYOUT_DEVS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    if (ImGui::MenuItem("Coding", "F6")) {
        Components::get()->Window()->setImGuiConfig(Config::ImGUIConfigs::CODING);
    }
    ImGui::Image(FileSystemGUI::Icon(IconGUI::LAYOUTS_LAYOUT_DESIGNERS), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    if (ImGui::MenuItem("Design", "F7")) {
        Components::get()->Window()->setImGuiConfig(Config::ImGUIConfigs::DESIGN);
    }
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::LAYOUTS_RESET_CURRENT), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    if (ImGui::MenuItem("Reset to default")) {
        Brakeza::get()->GUI()->setLayoutToDefault(Components::get()->Window()->getImGuiConfig());
    }
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::Icon(IconGUI::LAYOUTS_SAVE_LAYOUT), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    if (ImGui::MenuItem("Save current layout")) {
        Components::get()->Window()->SaveImGuiCurrentLayout();
    }
}

void GUIAddonMenu::MenuWindow(GUIManager *gui)
{
    auto setup = Config::get();

    ImGui::Image(FileSystemGUI::Icon(IconGUI::WINDOW_FULLSCREEN), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
    ImGui::MenuItem("FullScreen (F11)", nullptr, &setup->FULLSCREEN);
    if (ImGui::IsItemEdited()) {
        Components::get()->Window()->toggleFullScreen();
    }
    ImGui::SeparatorText("Windows/Widgets");
    for (auto &w : gui->windows) {
        ImGui::Image(FileSystemGUI::Icon(w.icon), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
        ImGui::MenuItem(w.label.c_str(), nullptr, &w.isOpen);
    }
}

void GUIAddonMenu::AboutMeModal()
{
    if (Config::get()->SHOW_ABOUT_ME_MODAL) {
        ImGui::OpenPopup("About Brakeza3D");
        Config::get()->SHOW_ABOUT_ME_MODAL = false;
    }

    if (ImGui::BeginPopupModal("About Brakeza3D", nullptr)) {
        auto setup = Config::get();

        ImGui::Text("Welcome to Brakeza3D!");
        ImGui::Text(setup->ENGINE_VERSION.c_str());
        ImGui::Text(setup->ENGINE_TITLE.c_str());
        ImGui::Text("Website: %s", setup->ENGINE_WEBSITE.c_str());
        ImGui::Text("GitHub: %s", setup->ENGINE_SOURCE_WEBSITE.c_str());
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            Config::get()->SHOW_ABOUT_ME_MODAL = false;
        }
        ImGui::EndPopup();
    }
}

void GUIAddonMenu::DrawItemsToLoad(
    std::string& folder,
    std::string& ext,
    GUIType::Sheet icon,
    const std::function<void(const std::string&)>& cb)
{
    auto files = Tools::getFolderFiles(folder, ext);
    auto folders = Tools::getFolderFolders(folder);

    // Procesar subcarpetas
    for (const auto& folderName : folders) {
        auto fullPath = folder + "/" + folderName;
        ImGui::Image(FileSystemGUI::Icon(IconGUI::FOLDER), GUIType::Sizes::ICON_SIZE_MENUS);
        ImGui::SameLine();
        if (ImGui::BeginMenu(folderName.c_str())) {
            DrawItemsToLoad(fullPath, ext, icon, cb);
            ImGui::EndMenu();
        }
    }

    // Procesar archivos
    for (const auto& file : files) {
        auto fullPath = folder + "/" + file;
        ImGui::Image(FileSystemGUI::Icon(icon), GUIType::Sizes::ICON_SIZE_MENUS);
        ImGui::SameLine();
        if (ImGui::MenuItem(file.c_str())) {
            cb(fullPath);
        }
    }
}

void GUIAddonMenu::MenuWorkers()
{
    if (ImGui::BeginTabBar("WorkerPoolsTabs"))
    {
        if (ImGui::BeginTabItem("Pool Compute"))
        {
            MenuWorker(Brakeza::get()->PoolCompute(), "Pool Compute");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Pool Images"))
        {
            MenuWorker(Brakeza::get()->PoolImages(), "Pool Images");
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

void GUIAddonMenu::MenuWorker(ThreadPool &pool, std::string title)
{
    ImGui::PushID(title.c_str());

    // Título y separador
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", title.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    // ===== INFORMACIÓN ACTUAL =====
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Estado Actual:");
    ImGui::Indent();

    ImGui::Text("Tareas Activas: %d", pool.getActiveTasks());
    ImGui::Text("Tareas Pendientes: %zu", pool.getPendingTasks());
    ImGui::Text("Callbacks Pendientes: %zu", pool.getPendingCallbacks());
    ImGui::Text("Contador Total: %d", pool.getCont());

    ImGui::Unindent();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ===== CONFIGURACIÓN =====
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "Configuración:");
    ImGui::Spacing();

    // Max Callbacks Per Frame
    {
        static int maxCallbacks = static_cast<int>(pool.getMaxCallbacksPerFrame());
        ImGui::Text("Max Callbacks Por Frame:");
        ImGui::SetNextItemWidth(200);

        if (ImGui::SliderInt("##maxCallbacks", &maxCallbacks, 1, 100)) {
            pool.setMaxCallbacksPerFrame(static_cast<size_t>(maxCallbacks));
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Número máximo de callbacks que se procesan\ncada frame en el hilo principal");
        }

        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("##inputCallbacks", &maxCallbacks, 1, 10)) {
            maxCallbacks = std::max(1, maxCallbacks);
            pool.setMaxCallbacksPerFrame(static_cast<size_t>(maxCallbacks));
        }
        ImGui::Spacing();
    }

    // Max Concurrent Tasks
    {
        static int maxConcurrent = static_cast<int>(pool.getMaxConcurrentTasks());
        ImGui::Text("Max Tareas Concurrentes:");
        ImGui::SetNextItemWidth(200);

        if (ImGui::SliderInt("##maxConcurrent", &maxConcurrent, 1, 32)) {
            pool.setMaxConcurrentTasks(static_cast<size_t>(maxConcurrent));
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Número máximo de tareas que pueden\nejecutarse simultáneamente");
        }

        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("##inputConcurrent", &maxConcurrent, 1, 4)) {
            maxConcurrent = std::max(1, maxConcurrent);
            pool.setMaxConcurrentTasks(static_cast<size_t>(maxConcurrent));
        }
        ImGui::Spacing();
    }

    // Max Enqueued Tasks
    {
        static int maxEnqueued = static_cast<int>(pool.getMaxEnqueuedTasks());
        ImGui::Text("Max Tareas En Cola:");
        ImGui::SetNextItemWidth(200);

        if (ImGui::SliderInt("##maxEnqueued", &maxEnqueued, 1, 1000)) {
            pool.setMaxEnqueuedTasks(static_cast<size_t>(maxEnqueued));
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Número máximo de tareas que pueden\nestar esperando en la cola");
        }

        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("##inputEnqueued", &maxEnqueued, 10, 100)) {
            maxEnqueued = std::max(1, maxEnqueued);
            pool.setMaxEnqueuedTasks(static_cast<size_t>(maxEnqueued));
        }
        ImGui::Spacing();
    }

    ImGui::Separator();
    ImGui::Spacing();

    // ===== BOTONES DE CONTROL =====
    if (ImGui::Button("Restaurar Valores por Defecto", ImVec2(250, 0))) {
        pool.setMaxCallbacksPerFrame(10);
        pool.setMaxConcurrentTasks(4);
        pool.setMaxEnqueuedTasks(100);
    }

    ImGui::SameLine();

    if (ImGui::Button("Esperar Todas las Tareas", ImVec2(200, 0))) {
        pool.waitAll();
    }

    ImGui::PopID();
}