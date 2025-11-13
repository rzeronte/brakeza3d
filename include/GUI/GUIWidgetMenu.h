//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETMENU_H
#define BRAKEZA3D_GUIWIDGETMENU_H

#include <SDL_mixer.h>
#include <dirent.h>
#include <sys/stat.h>
#include "imgui.h"
#include "../EngineSetup.h"
#include "../Render/Logging.h"
#include "../Misc/SceneLoader.h"
#include "../ComponentsManager.h"
#include "imgui_internal.h"
#include "../Misc/TexturePackage.h"

struct GUIWidgetMenu
{
    TexturePackage &ImGuiTextures;

    explicit GUIWidgetMenu(TexturePackage &imGuiTextures)
    :
        ImGuiTextures(imGuiTextures)
    {
    }

    void draw(bool &finish, bool &show_about_window)
    {
        auto setup = EngineSetup::get();
        
        const float range_sensibility = 0.75f;
        const float range_max_sensibility = 9999;
        const float range_min_sensibility = -9999;

        const float range_max_light_direction_size = 10;
        const float range_min_light_direction_size = 1;

        const float range_sensibility_volume = 1;
        const float range_min_volume = 1;
        const float range_max_volume = 128;

        const float range_far_plane_distance_sensibility = 1.0f;
        const float range_far_plane_min = 0;
        const float range_max_plane_max = 1000000;

        const int range_framerate_sensibility = 1;
        const int range_min_framerate_distance = 0;
        const int range_max_framerate_distance = 500;

        const float range_min_fov = 0;
        const float range_max_fov = 160;

        if (show_about_window) {
            ImGui::OpenPopup("About Brakeza3D");
        }

        if (ImGui::BeginPopupModal("About Brakeza3D", &show_about_window)) {
            ImGui::Text("Welcome to Brakeza3D!");
            ImGui::Text((setup->ENGINE_VERSION).c_str());
            ImGui::Text("By Eduardo Rodríguez Álvarez <eduardo@brakeza.com>");
            ImGui::Text("Website: https://brakeza.com");
            ImGui::Text("GitHub: https://github.com/rzeronte/brakeza3d");
            if (ImGui::Button("Close")) {
                ImGui::CloseCurrentPopup();
                show_about_window = false;
            }
            ImGui::EndPopup();
        }

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Brakeza3D")) {
                if (ImGui::MenuItem("About Brakeza3D")) {
                    show_about_window = true;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Exit")) finish = true;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Control")) {
                auto scripting = ComponentsManager::get()->getComponentScripting();
                auto state = scripting->getStateLUAScripts();
                ImGui::Image(icon("playIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Play scripts", "F1", false,  state != EngineSetup::LuaStateScripts::LUA_PLAY)) {
                    scripting->playLUAScripts();
                }
                ImGui::Separator();
                ImGui::Image(icon("stopIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Stop scripts", "F1", false, state != EngineSetup::LuaStateScripts::LUA_STOP)) {
                    scripting->stopLUAScripts();
                }
                ImGui::Separator();
                ImGui::Image(icon("reloadIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Reload scripts", "F2")) {
                    scripting->reloadLUAScripts();
                }
                ImGui::Separator();
                ImGui::Image(icon("removeIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Clear scene objects", "F3")) {
                    SceneLoader::clearScene();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Add object")) {

                ImGui::Image(icon("objectIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Object3D (Empty)")) {
                    SceneLoader::createObjectInScene();
                }

                ImGui::Separator();

                ImGui::Image(icon("particlesIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("ParticleEmitter")) {
                    SceneLoader::createParticleEmitterInScene();
                    ImGui::EndMenu();
                }
                ImGui::Separator();

                ImGui::Image(icon("Image2DIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::BeginMenu("2D Objects")) {

                    ImGui::Separator();

                    ImGui::Image(icon("Image2DIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::BeginMenu("Image2D")) {
                        drawImage2DItemsToLoad(setup->IMAGES_FOLDER);
                        ImGui::EndMenu();
                    }
                    ImGui::Image(icon("Image2DAnimationIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::BeginMenu("Image2D Animation")) {
                        drawImage2DAnimationItemsToLoad(setup->SPRITES_FOLDER);
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                ImGui::Image(icon("meshIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::BeginMenu("3D Objects")) {
                    ImGui::Image(icon("Image3DIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::BeginMenu("Image3D")) {
                        drawImage3DItemsToLoad(setup->IMAGES_FOLDER);
                        ImGui::EndMenu();
                    }

                    ImGui::Separator();

                    ImGui::Image(icon("BillboardAnimationIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::BeginMenu("Billboard Animation")) {
                        drawBillboardAnimationItemsToLoad(setup->SPRITES_FOLDER);
                        ImGui::EndMenu();
                    }
                    ImGui::Image(icon("BillboardAnimation8DirectionsIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem("Billboard 8-Directions")) {
                        SceneLoader::createBillboardAnimation8Directions();
                        ImGui::EndMenu();
                    }

                    ImGui::Separator();

                    ImGui::Image(icon("meshIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::BeginMenu("Mesh3D")) {
                        drawMesh3DItemsToLoad(setup->MODELS_FOLDER);
                        ImGui::EndMenu();
                    }

                    ImGui::Separator();

                    ImGui::Image(icon("Mesh3DAnimationIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::BeginMenu("Mesh3DAnimation")) {
                        drawMesh3DAnimationItemsToLoad(setup->ANIMATIONS_FOLDER);
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                }
                ImGui::Image(icon("lightIcon"), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::BeginMenu("Light Objects")) {
                    ImGui::Image(icon("lightIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem("PointLight")) {
                        SceneLoader::createPointLight3DInScene();
                        Logging::Message("Add PointLight");
                    }
                    ImGui::Image(icon("spotLightIcon"), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem("SpotLight")) {
                        SceneLoader::createSpotLight3DInScene();
                        Logging::Message("Add SpotLight");
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Render")) {
                ImGui::Checkbox("v-Sync", &setup->V_SYNC);
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
                ImGui::Checkbox("Forward rendering", &setup->FORWARD_RENDER);
                if (ImGui::IsItemEdited()) {
                    if (setup->FORWARD_RENDER) {
                        ComponentsManager::get()->getComponentWindow()->setUseDeferredRendering(false);
                    } else {
                        ComponentsManager::get()->getComponentWindow()->setUseDeferredRendering(true);
                    }
                }
                ImGui::Separator();
                ImGui::DragScalar("FOV", ImGuiDataType_Float, &setup->HORIZONTAL_FOV, 1, &range_min_fov,&range_max_fov, "%f", 1.0f);
                ImGui::Separator();
                ImGui::Checkbox("Limit frame rate", &setup->LIMIT_FRAMERATE);
                if (setup->LIMIT_FRAMERATE) {
                    ImGui::DragScalar("Limite frames to:", ImGuiDataType_S32, &setup->FRAMERATE, range_framerate_sensibility, &range_min_framerate_distance,&range_max_framerate_distance, "%d", 1.0f);

                }
                ImGui::Separator();
                ImGui::DragScalar("Frustum Far Plane Distance", ImGuiDataType_Float, &setup->FRUSTUM_FARPLANE_DISTANCE, range_far_plane_distance_sensibility, &range_far_plane_min, &range_max_plane_max, "%f", 1.0f);
                ImGui::Separator();
                ImGui::Checkbox("Depth Map", &setup->SHOW_DEPTH_OF_FIELD);
                ImGui::Separator();
                if (!setup->SHOW_DEPTH_OF_FIELD) {
                    ImGui::Checkbox("Vertex", &setup->TRIANGLE_MODE_PIXELS);
                    ImGui::Checkbox("WireFrame", &setup->TRIANGLE_MODE_WIREFRAME);
                    ImGui::Checkbox("Solid", &setup->TRIANGLE_MODE_COLOR_SOLID);
                    ImGui::Checkbox("Textures", &setup->TRIANGLE_MODE_TEXTURIZED);
                    ImGui::Separator();
                } else {
                    auto s = ComponentsManager::get()->getComponentRender()->getShaderOGLDepthMap();
                    ImGui::DragFloat("Intensity", &s->intensity, 0.01f, 0.0f, 1.0f);
                    ImGui::DragFloat("Far Plane", &s->farPlane, 0.01f, 0.0f, 100.0f);
                    ImGui::DragFloat("Near Plabe", &s->nearPlane, 0.01f, 0.0f, 100.0f);

                }
                ImGui::Checkbox("Draw Bones", &setup->DRAW_ANIMATION_BONES);
                ImGui::Separator();
                ImGui::Checkbox("Internal click selection", &setup->CLICK_SELECT_OBJECT3D);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Physics & Collisions")) {
                int minSubsteps = 0;
                int maxSubsteps = 10;
                int minFixedTime = 1;
                int maxFixedTime = 60;
                ImGui::Checkbox("Enable physics", &setup->BULLET_STEP_SIMULATION);
                ImGui::Separator();
                ImGui::DragScalar("Max sub-steps", ImGuiDataType_S32, &setup->BULLET_MAX_SUBSTEPS, 1,&minSubsteps, &maxSubsteps, "%d", 1.0f);
                ImGui::DragScalar("Fixed time step (1/x)", ImGuiDataType_S32, &setup->BULLET_FIXED_TIME_STEPS, 1, &minFixedTime, &maxFixedTime, "%d", 1.0f);
                ImGui::Separator();
                if (ImGui::Checkbox("Draw debug mode", &setup->BULLET_DEBUG_MODE)) {
                    ComponentsManager::get()->getComponentCollisions()->setEnableDebugMode(setup->BULLET_DEBUG_MODE);
                }
                if (setup->BULLET_STEP_SIMULATION) {
                    ImGui::Separator();
                    ImGui::Checkbox("Handle object collisions", &setup->BULLET_CHECK_ALL_PAIRS);
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
                    ImGui::DragScalar("ProjectileDemo Impulse", ImGuiDataType_Float, &setup->PROJECTILE_DEMO_IMPULSE, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                    ImGui::DragScalar("ProjectileDemo Accuracy", ImGuiDataType_Float, &setup->PROJECTILE_DEMO_ACCURACY, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                    ImGui::DragScalar("ProjectileDemo Mass", ImGuiDataType_Float, &setup->PROJECTILE_DEMO_MASS, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                    ImGui::Separator();
                    ImGui::Checkbox("ProjectileDemo SimpleMesh", &setup->PROJECTILE_SIMPLE_MESH);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Global illumination")) {
                drawGlobalIllumination();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Camera")) {
                drawCameraSettings();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Sound")) {
                ImGui::Checkbox("Global enable", &setup->SOUND_ENABLED);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    if (!setup->SOUND_ENABLED) {
                        Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, 0);
                        Mix_VolumeMusic((int) setup->SOUND_VOLUME_MUSIC);
                        Mix_VolumeMusic(0);
                    } else {
                        Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, (int) setup->SOUND_CHANNEL_GLOBAL);
                        Mix_VolumeMusic((int) setup->SOUND_VOLUME_MUSIC);
                    }
                }

                ImGui::DragScalar("Music volume", ImGuiDataType_Float, &setup->SOUND_VOLUME_MUSIC, range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) { Mix_VolumeMusic((int )setup->SOUND_VOLUME_MUSIC); }

                ImGui::DragScalar("Global Channel volume", ImGuiDataType_Float, &setup->SOUND_CHANNEL_GLOBAL, range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, (int) setup->SOUND_CHANNEL_GLOBAL);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Logging")) {
                ImGui::Checkbox("Output to Console", &setup->LOGGING);
                ImGui::Separator();
                ImGui::Checkbox("Log collisions", &setup->LOG_COLLISION_OBJECTS);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Layout")) {
                if (ImGui::MenuItem("Default", "F5")) {
                    ComponentsManager::get()->getComponentWindow()->ImGuiConfigChanged = ImGUIConfigs::DEFAULT;
                }
                if (ImGui::MenuItem("Coding", "F6")) {
                    ComponentsManager::get()->getComponentWindow()->ImGuiConfigChanged = ImGUIConfigs::CODING;
                }
                if (ImGui::MenuItem("Design", "F7")) {
                    ComponentsManager::get()->getComponentWindow()->ImGuiConfigChanged = ImGUIConfigs::DESIGN;
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Save current layout")) {
                    ComponentsManager::get()->getComponentWindow()->saveImGuiCurrentLayout();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::Checkbox("FullScreen (F11)", &setup->FULLSCREEN);
                if (ImGui::IsItemEdited()) {
                    ComponentsManager::get()->getComponentWindow()->toggleFullScreen();
                }
                ImGui::Separator();
                ImGui::Checkbox("UI (F4)", &setup->IMGUI_ENABLED);

                ImGui::Separator();
                ImGui::Checkbox("Draw Main Axis", &setup->RENDER_MAIN_AXIS);
                ImGui::Checkbox("Draw Object3D Axis", &setup->RENDER_OBJECTS_AXIS);
                if (setup->RENDER_OBJECTS_AXIS) {
                    const float sizeAxisMin = 0;
                    const float sizeAxisMax = 1;
                    const float sizeAxisSens = 0.01;

                    ImGui::DragScalar("Size Axis", ImGuiDataType_Float, &setup->OBJECT_AXIS_SIZE,
                                      sizeAxisSens, &sizeAxisMin, &sizeAxisMax, "%f", 1.0f);

                }
                ImGui::Separator();
                ImGui::Checkbox("Light System", &setup->ENABLE_LIGHTS);

                ImGui::Separator();

                ImGui::Checkbox("Draw Mesh3D AABB", &setup->DRAW_MESH3D_AABB);
                ImGui::Checkbox("Draw Mesh3D Octree", &setup->DRAW_MESH3D_OCTREE);
                ImGui::Checkbox("Draw Mesh3D Grid", &setup->DRAW_MESH3D_GRID);
                if (setup->DRAW_MESH3D_GRID) {
                    ImGui::Separator();
                    ImGui::Checkbox("Draw Test Passed Cells", &setup->DRAW_MESH3D_TEST_PASSED);
                    ImGui::Checkbox("Draw Not Test Passed Cells", &setup->DRAW_MESH3D_TEST_NOT_PASSED);
                    ImGui::Separator();
                    ImGui::Checkbox("Draw AStar Travel", &setup->DRAW_MESH3D_GRID_ASTAR);
                }
                ImGui::Separator();
                ImGui::Checkbox("Show FPS", &setup->DRAW_FPS);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
    }

    void drawImage2DItemsToLoad(const std::string& folder)
    {

        auto files= Tools::getFolderFiles(folder, "png");
        auto folders = Tools::getFolderFolders(folder);

        for (const auto & i : folders) {
            auto fullPath = folder + "/" + i;
            ImGui::Image(icon("folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::BeginMenu(i.c_str())) {
                drawImage2DItemsToLoad(fullPath);
                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < files.size(); i++) {
            auto file = files[i];
            auto title = std::to_string(i-1) + ") " + file;

            auto fullPath = folder + "/" + file;
            ImGui::Image(icon("Image2DIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::MenuItem(file.c_str())) {
                SceneLoader::createImage2DInScene(fullPath);
            }
        }
    }

    void drawImage2DAnimationItemsToLoad(const std::string& folder) {

        auto files= Tools::getFolderFiles(folder, "png");
        auto folders = Tools::getFolderFolders(folder);

        for (const auto & i : folders) {
            auto fullPath = folder + "/" + i;
            ImGui::Image(icon("folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::BeginMenu(i.c_str())) {
                drawImage2DAnimationItemsToLoad(fullPath);
                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < files.size(); i++) {
            auto file = files[i];
            auto title = std::to_string(i-1) + ") " + file;

            auto fullPath = folder + "/" + file;
            ImGui::Image(icon("Image2DAnimationIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::MenuItem(file.c_str())) {
                SceneLoader::createImage2DAnimatedInScene(fullPath);
            }
        }
    }

    void drawImage3DItemsToLoad(const std::string& folder) {
        auto files = Tools::getFolderFiles(folder, "png");
        auto folders = Tools::getFolderFolders(folder);

        for (const auto & i : folders) {
            auto fullPath = folder + "/" + i;
            ImGui::Image(icon("folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::BeginMenu(i.c_str())) {
                drawImage3DItemsToLoad(fullPath);
                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < files.size(); i++) {
            auto file = files[i];
            auto title = std::to_string(i-1) + ") " + file;

            auto fullPath = folder + file;
            ImGui::Image(icon("Image3DIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::MenuItem(file.c_str())) {
                SceneLoader::createImage3DToScene(fullPath);
            }
        }
    }

    ImTextureID icon(const char* iconTag)
    {
        return TexturePackage::getOGLTextureID(ImGuiTextures, iconTag);
    }
    
    void drawBillboardAnimationItemsToLoad(const std::string& folder) {

        auto files = Tools::getFolderFiles(folder, "png");
        auto folders = Tools::getFolderFolders(folder);

        for (const auto & i : folders) {
            auto fullPath = folder + "/" + i;
            ImGui::Image(icon("folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::BeginMenu(i.c_str())) {
                drawBillboardAnimationItemsToLoad(fullPath);
                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < files.size(); i++) {
            auto file = files[i];
            auto title = std::to_string(i-1) + ") " + file;

            auto fullPath = folder + "/" + file;
            ImGui::Image(icon("BillboardAnimationIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::MenuItem(file.c_str())) {
                SceneLoader::createBillboardAnimationInScene(fullPath);
            }
        }
    }

    void drawMesh3DItemsToLoad(const std::string& folder)
    {
        auto files= Tools::getFolderFiles(folder, "fbx");
        auto folders = Tools::getFolderFolders(folder);

        for (const auto & i : folders) {
            auto fullPath = folder + "/" + i;
            ImGui::Image(icon("folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::BeginMenu(i.c_str())) {
                drawMesh3DItemsToLoad(fullPath);
                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < files.size(); i++) {
            auto file = files[i];
            auto title = std::to_string(i-1) + ") " + file;

            auto fullPath = folder + "/" + file;
            ImGui::Image(icon("meshIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::MenuItem(file.c_str())) {
                SceneLoader::createMesh3D(fullPath);
            }
        }
    }

    void drawMesh3DAnimationItemsToLoad(const std::string& folder)
    {
        auto files= Tools::getFolderFiles(folder, "fbx");
        auto folders= Tools::getFolderFolders(folder);

        for (const auto & i : folders) {
            auto fullPath = folder + "/" + i;
            ImGui::Image(icon("folderIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::BeginMenu(i.c_str())) {
                drawMesh3DAnimationItemsToLoad(fullPath);
                ImGui::EndMenu();
            }
        }

        for (int i = 0; i < files.size(); i++) {
            auto file = files[i];
            auto title = std::to_string(i-1) + ") " + file;

            auto fullPath = folder + "/" + file;
            ImGui::Image(icon("Mesh3DAnimationIcon"), ImVec2(16, 16));
            ImGui::SameLine();
            if (ImGui::MenuItem(file.c_str())) {
                SceneLoader::createMesh3DAnimationToScene(fullPath);
            }
        }
    }

    void drawGlobalIllumination()
    {
        auto render = ComponentsManager::get()->getComponentRender();
        auto dirLight = ComponentsManager::get()->getComponentRender()->getShaderOGLRender()->getDirectionalLight();

        ImGui::DragFloat3("Light dir.", &dirLight.direction[0], 0.01f, -1.0f, 1.0f);

        ImGui::Separator();

        ImVec4 color = {dirLight.ambient.x, dirLight.ambient.y, dirLight.ambient.z, 1};
        bool changed_color = ImGui::ColorEdit4("Ambient##", (float *) &color, ImGuiColorEditFlags_NoOptions);
        if (changed_color) {
            dirLight.ambient = {color.x, color.y, color.z};
        }
        color = {dirLight.specular.x, dirLight.specular.y, dirLight.specular.z, 1};
        changed_color = ImGui::ColorEdit4("Specular##", (float *) &color, ImGuiColorEditFlags_NoOptions);
        if (changed_color) {
            dirLight.specular = {color.x, color.y, color.z};
        }

        color = {dirLight.diffuse.x, dirLight.diffuse.y, dirLight.diffuse.z, 1};
        changed_color = ImGui::ColorEdit4("Diffuse##", (float *) &color, ImGuiColorEditFlags_NoOptions);
        if (changed_color) {
            dirLight.diffuse = {color.x, color.y, color.z};
        }

        ImGui::Separator();

        const float focalMinValues = 0;
        const float focalMaxValues = 1;
        const float focalValueSens = 0.001;

        const float depthMinValues = 0;
        const float depthMaxValues = 100;
        const float depthValueSens = 0.1;

        const int minBlurRadius = 0;
        const int maxBlurRadius = 10;

        ImGui::Checkbox("Enable DOF", &EngineSetup::get()->ENABLE_DEPTH_OF_FIELD);

        if (EngineSetup::get()->ENABLE_DEPTH_OF_FIELD) {
            ImGui::DragScalar("Focal range", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLDOF()->focalRange, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
            ImGui::DragScalar("Focal distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLDOF()->focalDistance, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
            ImGui::DragScalar("Blur radius", ImGuiDataType_S32, &ComponentsManager::get()->getComponentRender()->getShaderOGLDOF()->blurRadius,1.0f, &minBlurRadius, &maxBlurRadius, "%d", 1.0f);
            ImGui::DragScalar("Intensity", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLDOF()->intensity, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
            ImGui::DragScalar("Far Plane", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLDOF()->farPlane, depthValueSens, &depthMinValues, &depthMaxValues, "%f", 1.0f);
        }

        ImGui::Separator();

        ImGui::Checkbox("Enable FOG", &EngineSetup::get()->ENABLE_FOG);

        if (EngineSetup::get()->ENABLE_FOG) {
            const float rangeFogSens = 0.1;
            const float rangeFogMin = 0;
            const float rangeFogMax = 0;

            ImGui::DragScalar("FOG min distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLFOG()->fogMinDist, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);
            ImGui::DragScalar("FOG max distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLFOG()->fogMaxDist, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);
            ImGui::DragScalar("FOG intensity", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLFOG()->intensity, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);

            if (ImGui::TreeNode("FOG Color")) {
                const float fogColorSend = 0.01;
                const float fogColorMin = 0;
                const float fogColorMax = 1;

                ImGui::DragScalar("x", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLFOG()->fogColor.r, fogColorSend,&fogColorMin, &fogColorMax, "%f", 1.0f);
                ImGui::DragScalar("y", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLFOG()->fogColor.g, fogColorSend,&fogColorMin, &fogColorMax, "%f", 1.0f);
                ImGui::DragScalar("z", ImGuiDataType_Float, &ComponentsManager::get()->getComponentRender()->getShaderOGLFOG()->fogColor.b, fogColorSend,&fogColorMin, &fogColorMax, "%f", 1.0f);
                ImGui::TreePop();
            }
        }
    }

    void drawCameraSettings()
    {
        auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

        // position
        float vec3f[3];
        camera->getPosition().toFloat(vec3f);
        if (ImGui::DragFloat3("Position", vec3f, 0.1f, -10000.0f, 10000.0f)) {
            camera->position.x = vec3f[0];
            camera->position.y = vec3f[1];
            camera->position.z = vec3f[2];
        }
        ImGui::Separator();

        float oldPitch = camera->getRotation().getPitchDegree();
        float oldYaw = camera->getRotation().getYawDegree();
        float oldRoll = camera->getRotation().getRollDegree();
        float pitch = oldPitch;
        float yaw = oldYaw;
        float roll = oldRoll;
        const float factor = 0.0025f;

        vec3f[0] = pitch;
        vec3f[1] = yaw;
        vec3f[2] = roll;
        if (ImGui::DragFloat3("Rotation", vec3f, 0.01f, -999999.0f, 999999.0f)) {
            pitch = vec3f[0];
            yaw = vec3f[1];
            roll = vec3f[2];
            if (abs(pitch - oldPitch) > 0) {
                auto partialRotX = M3::arbitraryAxis(camera->getRotation().X(), Maths::radiansToDegrees(pitch - oldPitch) * factor);
                camera->setRotation(camera->getRotation() * partialRotX);
                M3::normalize(camera->rotation);
            }

            if (abs(yaw - oldYaw) > 0) {
                auto partialRotY = M3::arbitraryAxis(camera->getRotation().Y(), Maths::radiansToDegrees(yaw - oldYaw) * factor);
                camera->setRotation(camera->getRotation() * partialRotY);
                M3::normalize(camera->rotation);
            }

            if (abs(roll - oldRoll) > 0) {
                auto partialRotZ = M3::arbitraryAxis(camera->getRotation().Z(), Maths::radiansToDegrees(roll - oldRoll) * factor);
                camera->setRotation(camera->getRotation() * partialRotZ);
                M3::normalize(camera->rotation);
            }
        }
        ImGui::Separator();

        ImGui::DragFloat("Cursors Walking", &EngineSetup::get()->WALKING_SPEED, 0.01f, 0.0f, 500.0f);
        ImGui::DragFloat("Cursors Strafe", &EngineSetup::get()->STRAFE_SPEED, 0.01f, 0.0f, 500.0f);

        ImGui::Separator();

        ImGui::Checkbox("Mouse Look", &EngineSetup::get()->MOUSE_LOOK);
        if (EngineSetup::get()->MOUSE_LOOK) {
            ImGui::DragFloat("Sensibility", &EngineSetup::get()->MOUSE_SENSITIVITY, 0.001f, 0.0f, 1.0f);
        }

    }
};

#endif //BRAKEZA3D_GUIWIDGETMENU_H
