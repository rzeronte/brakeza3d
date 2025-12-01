//
// Created by Eduardo on 01/12/2025.
//

#include "GUIAddonMenu.h"

#include "../../Components/ComponentsManager.h"
#include "../../Misc/Logging.h"
#include "../../Misc/ToolsMaths.h"
#include "../../Serializers/Object3DSerializer.h"
#include "../../Serializers/LightPointSerializer.h"
#include "../../Serializers/LightSpotSerializer.h"
#include "../../Serializers/ParticleEmmitterSerializer.h"
#include "../../Serializers/Image3DAnimation8DirectionsSerializer.h"
#include "../../Serializers/Mesh3DAnimationSerializer.h"
#include "../../Serializers/Mesh3DSerializer.h"
#include "../../Serializers/Image3DSerializer.h"
#include "../../Serializers/Image2DSerializer.h"
#include "../../Serializers/Image2DAnimationSerializer.h"
#include "../../Serializers/Image3DAnimationSerializer.h"
#include "../Objects/FileSystemGUI.h"

GUIAddonMenu::GUIAddonMenu(std::vector<GUITypes::GUIWindowsStatus> &windowStatus)
:
    windowStatus(windowStatus)
{
}

void GUIAddonMenu::Draw()
{
    std::vector<GUITypes::GUIMenuItem> menus = {
        {"Brakeza3D", [this]() { this->DrawMainMenuBrakeza3D(); }},
        {"Script Controls",[this]() { this->DrawMainMenuScriptControls(); }},
        {"Add Object",[this]() { this->DrawMainMenuAddObject(); }},
        {"Video", [this]() { this->DrawMainMenuVideo(); }},
        {"Colliders", [this]() { this->DrawMainMenuColliders(); }},
        {"Illumination", [this]() { this->DrawMainMenuIllumination(); }},
        {"Camera",[this]() { this->DrawMainMenuCamera(); }},
        {"Sound", [this]() { this->DrawMainMenuSound(); }},
        {"Logging", [this]() { this->DrawMainMenuLogging(); }},
        {"Layout", [this]() { this->DrawMainMenuLayout(); }},
        {"Window", [this]() { this->DrawMainMenuWindow(); }},
    };

    if (ImGui::BeginMainMenuBar()) {
        for (const auto & menu : menus) {
            if (ImGui::BeginMenu(menu.label.c_str())) {
                menu.functionCallBack();
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }

    DrawAboutMeModal();
}

void GUIAddonMenu::DrawMainMenuBrakeza3D()
{
    if (ImGui::MenuItem("About Brakeza3D")) {
        BrakezaSetup::get()->SHOW_ABOUT_ME_MODAL = true;
    }

    ImGui::Separator();
    if (ImGui::MenuItem("Exit")) BrakezaSetup::get()->EXIT = true;
}

void GUIAddonMenu::DrawMainMenuScriptControls()
{
    auto scripting = ComponentsManager::get()->getComponentScripting();
    auto state = scripting->getStateLUAScripts();
    ImGui::Image(FileSystemGUI::IconTag(IconsByGUI::PLAY), ImVec2(16, 16));
    ImGui::SameLine();
    if (ImGui::MenuItem("Play scripts", "F1", false,  state != BrakezaSetup::LuaStateScripts::LUA_PLAY)) {
        scripting->playLUAScripts();
    }
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::IconTag(IconsByGUI::STOP), ImVec2(16, 16));
    ImGui::SameLine();
    if (ImGui::MenuItem("Stop scripts", "F1", false, state != BrakezaSetup::LuaStateScripts::LUA_STOP)) {
        scripting->stopLUAScripts();
    }
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::IconTag(IconsByGUI::RELOAD), ImVec2(16, 16));
    ImGui::SameLine();
    if (ImGui::MenuItem("Reload scripts", "F2")) {
        scripting->reloadLUAScripts();
    }
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::IconTag(IconsByGUI::REMOVE), ImVec2(16, 16));
    ImGui::SameLine();
    if (ImGui::MenuItem("Clear scene objects", "F3")) {
        SceneLoader::ClearScene();
    }
}

void GUIAddonMenu::DrawMainMenuAddObject()
{
    auto setup = BrakezaSetup::get();

    ImGui::Image(FileSystemGUI::IconTag(IconsByObject::OBJECT_3D), ImVec2(16, 16));
    ImGui::SameLine();
    if (ImGui::MenuItem("Object3D (Empty)")) {
        Object3DSerializer().LoadFileIntoScene("");
    }
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::IconTag(IconsByObject::PARTICLE_EMITTER), ImVec2(16, 16));
    ImGui::SameLine();
    if (ImGui::MenuItem("ParticleEmitter")) {
        ParticleEmmitterSerializer().LoadFileIntoScene("");
        ImGui::EndMenu();
    }
    ImGui::Separator();

    ImGui::Image(FileSystemGUI::IconTag(IconsByObject::IMAGE_2D), ImVec2(16, 16));
    ImGui::SameLine();
    if (ImGui::BeginMenu("2D Objects")) {
        ImGui::Image(FileSystemGUI::IconTag(IconsByObject::IMAGE_2D), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::BeginMenu("Image2D")) {
            DrawImage2DMenuItem(setup->IMAGES_FOLDER);
            ImGui::EndMenu();
        }
        ImGui::Image(FileSystemGUI::IconTag(IconsByObject::IMAGE_2D_ANIMATION), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::BeginMenu("Image2D Animation")) {
            DrawImage2DAnimationMenuItem(setup->SPRITES_FOLDER);
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::IconTag(IconsByObject::MESH_3D), ImVec2(16, 16));
    ImGui::SameLine();
    if (ImGui::BeginMenu("3D Objects")) {
        ImGui::Image(FileSystemGUI::IconTag(IconsByObject::IMAGE_3D), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::BeginMenu("Image3D")) {
            DrawImage3DMenuItem(setup->IMAGES_FOLDER);
            ImGui::EndMenu();
        }

        ImGui::Separator();

        ImGui::Image(FileSystemGUI::IconTag(IconsByObject::IMAGE_3D_ANIMATION), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::BeginMenu("Billboard Animation")) {
            DrawBillboardAnimationMenuItem(setup->SPRITES_FOLDER);
            ImGui::EndMenu();
        }
        ImGui::Image(FileSystemGUI::IconTag(IconsByObject::IMAGE_3D_ANIMATION_8DIR), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::MenuItem("Billboard 8-Directions")) {
            Image3DAnimation8DirectionsSerializer().LoadFileIntoScene("");
            ImGui::EndMenu();
        }
        ImGui::Separator();
        ImGui::Image(FileSystemGUI::IconTag(IconsByObject::MESH_3D), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::BeginMenu("Mesh3D")) {
            DrawMesh3DMenuItem(setup->MODELS_FOLDER);
            ImGui::EndMenu();
        }
        ImGui::Separator();
        ImGui::Image(FileSystemGUI::IconTag(IconsByObject::MESH_3D_ANIMATION), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::BeginMenu("Mesh3DAnimation")) {
            DrawMesh3DAnimationMenuItem(setup->ANIMATIONS_FOLDER);
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
    ImGui::Separator();
    ImGui::Image(FileSystemGUI::IconTag(IconsByObject::LIGHT_POINT), ImVec2(16, 16));
    ImGui::SameLine();
    if (ImGui::BeginMenu("Light Objects")) {
        ImGui::Image(FileSystemGUI::IconTag(IconsByObject::LIGHT_POINT), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::MenuItem("PointLight")) {
            LightPointSerializer().LoadFileIntoScene("");
        }
        ImGui::Image(FileSystemGUI::IconTag(IconsByObject::LIGHT_SPOT), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::MenuItem("SpotLight")) {
            LightSpotSerializer().LoadFileIntoScene("");
        }
        ImGui::EndMenu();
    }
}

void GUIAddonMenu::DrawMainMenuVideo()
{
    auto setup = BrakezaSetup::get();

    const float range_far_plane_distance_sensibility = 1.0f;
    ImGui::MenuItem("V-Sync", nullptr, &setup->V_SYNC);
    if (ImGui::IsItemEdited()) {
        if (setup->V_SYNC) {
            Logging::Message("Set V-Sync enabled");
            SDL_GL_SetSwapInterval(1);
            SDL_RenderSetVSync(ComponentsManager::get()->getComponentWindow()->getRenderer(), 1);
        } else {
            Logging::Message("Set V-Sync disabled");
            SDL_GL_SetSwapInterval(0);
            SDL_RenderSetVSync(ComponentsManager::get()->getComponentWindow()->getRenderer(), 0);
        }
    }
    ImGui::Separator();
    ImGui::MenuItem("Limit frame rate", nullptr,  &setup->LIMIT_FRAMERATE);
    if (setup->LIMIT_FRAMERATE) {
        const int range_framerate_sensibility = 1;
        ImGui::DragScalar("Limit frames to:", ImGuiDataType_S32, &setup->FRAMERATE, range_framerate_sensibility, &GUITypes::GUIConstants::DRAG_FRAMERATE_MIN, &GUITypes::GUIConstants::DRAG_FRAMERATE_MAX, "%d", 1.0f);

    }
    ImGui::Separator();
    ImGui::DragScalar("FOV", ImGuiDataType_Float, &setup->HORIZONTAL_FOV, 1, &GUITypes::GUIConstants::DRAG_FOV_MIN, &GUITypes::GUIConstants::DRAG_FOV_MAX, "%f", 1.0f);
    ImGui::Separator();
    ImGui::DragScalar("Frustum Far Plane Distance", ImGuiDataType_Float, &setup->FRUSTUM_FARPLANE_DISTANCE, range_far_plane_distance_sensibility, &GUITypes::GUIConstants::DRAG_FAR_PLANE_MIN, &GUITypes::GUIConstants::DRAG_FAR_PLANE_MIN, "%f", 1.0f);
    ImGui::Separator();
    ImGui::MenuItem("Vertex", nullptr, &setup->TRIANGLE_MODE_PIXELS);
    ImGui::MenuItem("WireFrame",nullptr,  &setup->TRIANGLE_MODE_WIREFRAME);
    ImGui::MenuItem("Solid", nullptr,  &setup->TRIANGLE_MODE_SHADING);
    ImGui::MenuItem("Picking Colors", nullptr, &setup->TRIANGLE_MODE_PICKING_COLORS);
    ImGui::MenuItem("Textures",nullptr, &setup->TRIANGLE_MODE_TEXTURIZED);
    ImGui::Separator();

    const float focalMinValues = 0;
    const float focalMaxValues = 1;
    const float focalValueSens = 0.001;

    ImGui::MenuItem("Enable DOF", nullptr, &BrakezaSetup::get()->ENABLE_DOF_BLUR);

    if (BrakezaSetup::get()->ENABLE_DOF_BLUR) {
        const float depthValueSens = 0.1;
        const float depthMinValues = 0;
        const float depthMaxValues = 100;
        ImGui::DragScalar("Focal range", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLDOF()->focalRange, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
        ImGui::DragScalar("Focal distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLDOF()->focalDistance, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);

        const int minBlurRadius = 0;
        const int maxBlurRadius = 10;
        ImGui::DragScalar("Blur radius", ImGuiDataType_S32, &ComponentsManager::get()->getComponentRender()->getShaderOGLDOF()->blurRadius,1.0f, &minBlurRadius, &maxBlurRadius, "%d", 1.0f);
        ImGui::DragScalar("Intensity", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLDOF()->intensity, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
        ImGui::DragScalar("Far Plane", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLDOF()->farPlane, depthValueSens, &depthMinValues, &depthMaxValues, "%f", 1.0f);
    }

    ImGui::Separator();

    ImGui::MenuItem("Enable FOG", nullptr, &BrakezaSetup::get()->ENABLE_FOG);

    if (BrakezaSetup::get()->ENABLE_FOG) {
        const float rangeFogSens = 0.1;
        const float rangeFogMin = 0.1;
        const float rangeFogMax = 1000;
        const float rangeFogIntensityMax= 1.0;

        ImGui::DragScalar("FOG min distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLFOG()->fogMinDist, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);
        ImGui::DragScalar("FOG max distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLFOG()->fogMaxDist, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);
        ImGui::DragScalar("FOG intensity", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLFOG()->intensity, rangeFogSens, &rangeFogMin, &rangeFogIntensityMax, "%f", 1.0f);

        auto p = ComponentsManager::get()->getComponentRender()->getShaderOGLFOG()->fogColor;
        ImVec4 fogVecColor = {p.r, p.b, p.b, 1};
        if (ImGui::ColorEdit4("FOG Color##", reinterpret_cast<float *>(&fogVecColor), ImGuiColorEditFlags_NoOptions)) {
            ComponentsManager::get()->getComponentRender()->getShaderOGLFOG()->fogColor = {fogVecColor.x, fogVecColor.y, fogVecColor.z};
        }
    }
    ImGui::Separator();
    ImGui::MenuItem("Draw Object3D Axis", nullptr, &setup->RENDER_OBJECTS_AXIS);
    if (setup->RENDER_OBJECTS_AXIS) {
        const float sizeAxisMin = 0;
        const float sizeAxisMax = 1;
        const float sizeAxisSens = 0.01;
        ImGui::DragScalar("Size Axis", ImGuiDataType_Float, &setup->OBJECT_AXIS_SIZE, sizeAxisSens, &sizeAxisMin, &sizeAxisMax, "%f", 1.0f);
    }
    ImGui::Separator();
    ImGui::MenuItem("Draw Mesh3D AABB", nullptr, &setup->DRAW_MESH3D_AABB);
    ImGui::MenuItem("Draw Mesh3D Octree", nullptr, &setup->DRAW_MESH3D_OCTREE);
    ImGui::MenuItem("Draw Mesh3D Grid", nullptr, &setup->DRAW_MESH3D_GRID);
    if (setup->DRAW_MESH3D_GRID) {
        ImGui::Separator();
        ImGui::MenuItem("Draw Test Passed Cells", nullptr, &setup->DRAW_MESH3D_TEST_PASSED);
        ImGui::MenuItem("Draw Not Test Passed Cells", nullptr, &setup->DRAW_MESH3D_TEST_NOT_PASSED);
        ImGui::Separator();
        ImGui::MenuItem("Draw AStar Travel", nullptr, &setup->DRAW_MESH3D_GRID_ASTAR);
    }
    ImGui::Separator();
    ImGui::MenuItem("Show FPS", nullptr, &setup->DRAW_FPS_RENDER);

    ImGui::Separator();
    ImGui::MenuItem("Draw Bones", nullptr, &setup->DRAW_ANIMATION_BONES);
    ImGui::Separator();
    ImGui::MenuItem("Internal click selection", nullptr, &setup->MOUSE_CLICK_SELECT_OBJECT3D);
}

void GUIAddonMenu::DrawMainMenuColliders()
{
    auto setup = BrakezaSetup::get();

    int minSubsteps = 0;
    int maxSubsteps = 10;
    int minFixedTime = 1;
    int maxFixedTime = 60;
    ImGui::MenuItem("Enable physics", nullptr, &setup->ENABLE_BULLET_STEP_SIMULATION);
    ImGui::Separator();
    ImGui::DragScalar("Max sub-steps", ImGuiDataType_S32, &setup->BULLET_MAX_SUBSTEPS, 1, &minSubsteps, &maxSubsteps, "%d", 1.0f);
    ImGui::DragScalar("Fixed time step (1/x)", ImGuiDataType_S32, &setup->BULLET_FIXED_TIME_STEPS, 1, &minFixedTime, &maxFixedTime, "%d", 1.0f);
    ImGui::Separator();
    if (ImGui::MenuItem("Draw debug mode", nullptr, &setup->BULLET_DEBUG_MODE)) {
        ComponentsManager::get()->getComponentCollisions()->setEnableDebugMode(setup->BULLET_DEBUG_MODE);
    }
    if (setup->ENABLE_BULLET_STEP_SIMULATION) {
        const float range_sensibility = 0.75f;
        ImGui::Separator();
        ImGui::MenuItem("Handle object collisions", nullptr, &setup->BULLET_CHECK_ALL_PAIRS);
        ImGui::Separator();

        float vec3f[3];
        setup->gravity.toFloat(vec3f);
        if (ImGui::DragFloat3("Gravity", vec3f, 0.01f, -1000.0f, 1000.0f)) {
            setup->gravity.x = vec3f[0];
            setup->gravity.y = vec3f[1];
            setup->gravity.z = vec3f[2];
            ComponentsManager::get()->getComponentCollisions()->setGravity(setup->gravity);
        }
        ImGui::Separator();
        ImGui::DragScalar("ProjectileDemo Impulse", ImGuiDataType_Float, &setup->PROJECTILE_DEMO_IMPULSE, range_sensibility, &GUITypes::GUIConstants::DRAG_UNIT_MIN, &GUITypes::GUIConstants::DRAG_UNIT_MAX, "%f", 1.0f);
        ImGui::DragScalar("ProjectileDemo Accuracy", ImGuiDataType_Float, &setup->PROJECTILE_DEMO_ACCURACY, range_sensibility, &GUITypes::GUIConstants::DRAG_UNIT_MIN, &GUITypes::GUIConstants::DRAG_UNIT_MAX, "%f", 1.0f);
        ImGui::DragScalar("ProjectileDemo Mass", ImGuiDataType_Float, &setup->PROJECTILE_DEMO_MASS, range_sensibility, &GUITypes::GUIConstants::DRAG_UNIT_MIN, &GUITypes::GUIConstants::DRAG_UNIT_MAX, "%f", 1.0f);
        ImGui::Separator();
        ImGui::MenuItem("ProjectileDemo SimpleMesh", nullptr, &setup->PROJECTILE_SIMPLE_MESH);
    }
}

void GUIAddonMenu::DrawMainMenuIllumination()
{
    auto setup = BrakezaSetup::get();

    ImGui::Separator();
    ImGui::MenuItem("Enable Lights System", nullptr, &setup->ENABLE_LIGHTS);
    if (setup->ENABLE_LIGHTS) {
        ImGui::MenuItem("Enable Shadow Mapping", nullptr, &setup->ENABLE_SHADOW_MAPPING);
        if (setup->ENABLE_SHADOW_MAPPING) {
            ImGui::Separator();
            ImGui::MenuItem("Enable Sun shadows", nullptr, &setup->SHADOW_MAPPING_ENABLE_DIRECTIONAL_LIGHT);
            ImGui::DragFloat("Shadows Intensity", &setup->SHADOW_MAPPING_INTENSITY, 0.1f, -5.0f, 5.0f);
            ImGui::DragFloat("DepthMaps Near plane", &setup->SHADOW_MAPPING_DEPTH_NEAR_PLANE, 0.1f, 1.0f, 500.0f);
            ImGui::DragFloat("DepthMaps Far plane", &setup->SHADOW_MAPPING_DEPTH_FAR_PLANE, 0.1f, 1.0f, 500.0f);
            ImGui::DragFloat("DepthMaps Frustum Size", &setup->SHADOW_MAPPING_FRUSTUM_SIZE, 0.1f, 100.0f);
            ImGui::MenuItem("Shadow Mapping debug mode", nullptr, &setup->SHADOW_MAPPING_DEBUG);
        }
    }
    ImGui::Separator();
    auto& dirLight = ComponentsManager::get()->getComponentRender()->getShaderOGLRenderForward()->getDirectionalLight();
    ImGui::DragFloat3("Sun direction", &dirLight.direction[0], 0.01f, -1.0f, 1.0f);
    ImGui::Separator();
    ImVec4 color = {dirLight.ambient.x, dirLight.ambient.y, dirLight.ambient.z, 1};
    bool changed_color = ImGui::ColorEdit4("Sun Ambient##", reinterpret_cast<float *>(&color), ImGuiColorEditFlags_NoOptions);
    if (changed_color) {
        dirLight.ambient = {color.x, color.y, color.z};
    }
    color = {dirLight.specular.x, dirLight.specular.y, dirLight.specular.z, 1};
    changed_color = ImGui::ColorEdit4("Sun Specular##", reinterpret_cast<float *>(&color), ImGuiColorEditFlags_NoOptions);
    if (changed_color) {
        dirLight.specular = {color.x, color.y, color.z};
    }
    color = {dirLight.diffuse.x, dirLight.diffuse.y, dirLight.diffuse.z, 1};
    changed_color = ImGui::ColorEdit4("Sun Diffuse##", reinterpret_cast<float *>(&color), ImGuiColorEditFlags_NoOptions);
    if (changed_color) {
        dirLight.diffuse = {color.x, color.y, color.z};
    }
    ImGui::Separator();
    if (setup->ENABLE_LIGHTS && setup->TRIANGLE_MODE_TEXTURIZED) {
        ImGui::MenuItem("Show Depth Map", nullptr, &setup->ENABLE_TRIANGLE_MODE_DEPTHMAP);
    }
    if (setup->ENABLE_TRIANGLE_MODE_DEPTHMAP) {
        auto s = ComponentsManager::get()->getComponentRender()->getShaderOGLDepthMap();
        ImGui::DragFloat("Intensity DepthMap", &s->intensity, 0.01f, 0.0f, 10.0f);
        ImGui::DragFloat("Far Plane", &s->farPlane, 0.01f, 0.0f, 100.0f);
        ImGui::DragFloat("Near Plane", &s->nearPlane, 0.01f, 0.0f, 100.0f);
        ImGui::Separator();
    }
}

void GUIAddonMenu::DrawMainMenuCamera()
{
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    // position
    float vec3f[3];
    camera->getPosition().toFloat(vec3f);
    if (ImGui::DragFloat3("Position", vec3f, 0.1f, -10000.0f, 10000.0f)) {
        camera->positionPointer().x = vec3f[0];
        camera->positionPointer().y = vec3f[1];
        camera->positionPointer().z = vec3f[2];
    }
    ImGui::Separator();

    float oldPitch = camera->getRotation().getPitchDegree();
    float oldYaw = camera->getRotation().getYawDegree();
    float oldRoll = camera->getRotation().getRollDegree();
    float pitch = oldPitch;
    float yaw = oldYaw;
    float roll = oldRoll;

    vec3f[0] = pitch;
    vec3f[1] = yaw;
    vec3f[2] = roll;
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
    ImGui::Separator();
    ImGui::DragFloat("Cursors Walking", &BrakezaSetup::get()->WALKING_SPEED, 0.01f, 0.0f, 500.0f);
    ImGui::DragFloat("Cursors Strafe", &BrakezaSetup::get()->STRAFE_SPEED, 0.01f, 0.0f, 500.0f);
    ImGui::Separator();
    ImGui::MenuItem("Mouse Look", nullptr, &BrakezaSetup::get()->MOUSE_LOOK);
    if (BrakezaSetup::get()->MOUSE_LOOK) {
        ImGui::DragFloat("Sensibility", &BrakezaSetup::get()->MOUSE_SENSITIVITY, 0.001f, 0.0f, 1.0f);
    }
}

void GUIAddonMenu::DrawMainMenuSound()
{
    auto setup = BrakezaSetup::get();

    const float range_sensibility_volume = 1;
    ImGui::MenuItem("Global enable", nullptr, &setup->ENABLE_SOUND);
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        if (!setup->ENABLE_SOUND) {
            Mix_Volume(BrakezaSetup::SoundChannels::SND_GLOBAL, 0);
            Mix_VolumeMusic(static_cast<int>(setup->SOUND_VOLUME_MUSIC));
            Mix_VolumeMusic(0);
        } else {
            Mix_Volume(BrakezaSetup::SoundChannels::SND_GLOBAL, static_cast<int>(setup->SOUND_CHANNEL_GLOBAL));
            Mix_VolumeMusic(static_cast<int>(setup->SOUND_VOLUME_MUSIC));
        }
    }

    ImGui::DragScalar("Music volume", ImGuiDataType_Float, &setup->SOUND_VOLUME_MUSIC, range_sensibility_volume, &GUITypes::GUIConstants::DRAG_VOLUME_MIN, &GUITypes::GUIConstants::DRAG_VOLUME_MAX, "%f", 1.0f);
    if (ImGui::IsItemEdited()) { Mix_VolumeMusic(static_cast<int>(setup->SOUND_VOLUME_MUSIC)); }

    ImGui::DragScalar("Global Channel volume", ImGuiDataType_Float, &setup->SOUND_CHANNEL_GLOBAL, range_sensibility_volume, &GUITypes::GUIConstants::DRAG_VOLUME_MIN, &GUITypes::GUIConstants::DRAG_VOLUME_MAX, "%f", 1.0f);
    if (ImGui::IsItemEdited()) {
        Mix_Volume(BrakezaSetup::SoundChannels::SND_GLOBAL, static_cast<int>(setup->SOUND_CHANNEL_GLOBAL));
    }
}

void GUIAddonMenu::DrawMainMenuLogging()
{
    auto setup = BrakezaSetup::get();

    ImGui::MenuItem("Output to Console", nullptr, &setup->ENABLE_LOGGING);
    ImGui::MenuItem("Output to STD", nullptr, &setup->ENABLE_LOGGING_STD);
    ImGui::Separator();
    ImGui::MenuItem("Log collider collisions", nullptr, &setup->LOG_COLLISION_OBJECTS);
}

void GUIAddonMenu::DrawMainMenuLayout()
{
    if (ImGui::MenuItem("Default", "F5")) {
        ComponentsManager::get()->getComponentWindow()->setImGuiConfig(BrakezaSetup::ImGUIConfigs::DEFAULT);
    }
    if (ImGui::MenuItem("Coding", "F6")) {
        ComponentsManager::get()->getComponentWindow()->setImGuiConfig(BrakezaSetup::ImGUIConfigs::CODING);
    }
    if (ImGui::MenuItem("Design", "F7")) {
        ComponentsManager::get()->getComponentWindow()->setImGuiConfig(BrakezaSetup::ImGUIConfigs::DESIGN);
    }
    ImGui::Separator();
    if (ImGui::MenuItem("Save current layout")) {
        ComponentsManager::get()->getComponentWindow()->saveImGuiCurrentLayout();
    }
}

void GUIAddonMenu::DrawMainMenuWindow()
{
    auto setup = BrakezaSetup::get();

    ImGui::MenuItem("FullScreen (F11)", nullptr, &setup->FULLSCREEN);
    if (ImGui::IsItemEdited()) {
        ComponentsManager::get()->getComponentWindow()->toggleFullScreen();
    }
    ImGui::Separator();
    ImGui::MenuItem("GUI Toolbar", nullptr, &setup->ENABLE_IMGUI_TOOLBAR);
    ImGui::Separator();
    ImGui::MenuItem("On/Off UI (F4)", nullptr, &setup->ENABLE_IMGUI);
    ImGui::Separator();
    for (auto &w : windowStatus) {
        ImGui::MenuItem(w.label.c_str(), nullptr, &w.isOpen);
    }
}

// Ahora tus funciones se simplifican a:
void GUIAddonMenu::DrawImage2DMenuItem(const std::string& folder)
{
    drawItemsToLoad(folder, "png", IconsByObject::IMAGE_2D,
        [](const std::string& path) {
            Image2DSerializer().LoadFileIntoScene(path);
        });
}

void GUIAddonMenu::DrawImage2DAnimationMenuItem(const std::string& folder)
{
    drawItemsToLoad(folder, "png", IconsByObject::IMAGE_2D_ANIMATION,
        [](const std::string& path) {
            Image2DAnimationSerializer().LoadFileIntoScene(path);
        });
}

void GUIAddonMenu::DrawImage3DMenuItem(const std::string& folder)
{
    drawItemsToLoad(folder, "png", IconsByObject::IMAGE_3D,
        [](const std::string& path) {
            Image3DSerializer().LoadFileIntoScene(path);
        });
}

void GUIAddonMenu::DrawBillboardAnimationMenuItem(const std::string& folder)
{
    drawItemsToLoad(folder, "png", IconsByObject::IMAGE_3D_ANIMATION,
        [](const std::string& path) {
            Image3DAnimationSerializer().LoadFileIntoScene(path);
        });
}

void GUIAddonMenu::DrawMesh3DMenuItem(const std::string& folder)
{
    drawItemsToLoad(folder, "fbx", IconsByObject::MESH_3D,
        [](const std::string& path) {
            Mesh3DSerializer().LoadFileIntoScene(path);
        });
}

void GUIAddonMenu::DrawMesh3DAnimationMenuItem(const std::string& folder)
{
    drawItemsToLoad(folder, "fbx", IconsByObject::MESH_3D_ANIMATION,
        [](const std::string& path) {
            Mesh3DAnimationSerializer().LoadFileIntoScene(path);
        });
}

void GUIAddonMenu::DrawAboutMeModal()
{
    if (ImGui::BeginPopupModal("About Brakeza3D", &BrakezaSetup::get()->SHOW_ABOUT_ME_MODAL)) {
        auto setup = BrakezaSetup::get();

        ImGui::Text("Welcome to Brakeza3D!");
        ImGui::Text(setup->ENGINE_VERSION.c_str());
        ImGui::Text(setup->ENGINE_TITLE.c_str());
        ImGui::Text("Website: %s", setup->ENGINE_WEBSITE.c_str());
        ImGui::Text("GitHub: %s", setup->ENGINE_SOURCE_WEBSITE.c_str());
        if (ImGui::Button("Close")) {
            ImGui::CloseCurrentPopup();
            BrakezaSetup::get()->SHOW_ABOUT_ME_MODAL = false;
        }
        ImGui::EndPopup();
    }
}

void GUIAddonMenu::drawItemsToLoad(
    const std::string& folder,
    const std::string& extension,
    const std::string& tagIcon,
    const std::function<void(const std::string&)>& loadFunction)
{
    auto files = Tools::getFolderFiles(folder, extension);
    auto folders = Tools::getFolderFolders(folder);

    // Procesar subcarpetas
    for (const auto& folderName : folders) {
        auto fullPath = folder + "/" + folderName;
        ImGui::Image(FileSystemGUI::IconTag(IconsByGUI::FOLDER), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::BeginMenu(folderName.c_str())) {
            drawItemsToLoad(fullPath, extension, tagIcon, loadFunction);
            ImGui::EndMenu();
        }
    }

    // Procesar archivos
    for (const auto& file : files) {
        auto fullPath = folder + "/" + file;
        ImGui::Image(FileSystemGUI::IconTag(tagIcon.c_str()), ImVec2(16, 16));
        ImGui::SameLine();
        if (ImGui::MenuItem(file.c_str())) {
            loadFunction(fullPath);
        }
    }
}