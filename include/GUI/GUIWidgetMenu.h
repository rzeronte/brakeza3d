//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETMENU_H
#define BRAKEZA3D_GUIWIDGETMENU_H

#include <SDL_mixer.h>
#include <dirent.h>
#include "imgui.h"
#include "../EngineSetup.h"
#include "../Render/Logging.h"
#include "../Misc/SceneLoader.h"
#include "../ComponentsManager.h"

struct GUIWidgetMenu
{
    TexturePackage &ImGuiTextures;
    std::string directory_path_models;

    explicit GUIWidgetMenu(TexturePackage &imGuiTextures)
    :
        ImGuiTextures(imGuiTextures),
        directory_path_models(EngineSetup::get()->MODELS_FOLDER)
    {

    }

    void draw(bool &finish)
    {
        bool show_about_window = false;

        const float range_sensibility_lightnin = 0.5;
        const float range_sensibility_lightnin_min = -10000;
        const float range_sensibility_lightnin_max = 100000;

        const float range_sensibility = 0.75f;
        const float range_test_sensibility = 0.1;
        const float range_max_sensibility = 999;
        const float range_min_sensibility = -999;

        const float range_sensibility_volume = 1;
        const float range_min_volume = 1;
        const float range_max_volume = 128;

        const float range_far_plane_distance_sensibility = 1.0f;
        const float range_far_plane_min = 0;
        const float range_max_plane_max = 1000000;

        const int range_framerate_sensibility = 1;
        const int range_min_framerate_distance = 0;
        const int range_max_framerate_distance = 500;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Brakeza3D")) {
                if (ImGui::MenuItem("About Brakeza", "CTRL+I")) show_about_window = true;
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "CTRL+W")) finish = true;
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Add object")) {
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("objectIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Object3D", "CTRL+O")) {
                    SceneLoader::createObjectInScene();
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("lightIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("PointLight", "CTRL+O")) {
                    SceneLoader::createPointLight3DInScene();
                    Logging::Message("Add PointLight");
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("spotLightIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("SpotLight", "CTRL+O")) {
                    SceneLoader::createSpotLight3DInScene();
                    Logging::Message("Add SpotLight");
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("meshIcon")->getOGLTextureID(), ImVec2(16, 16));

                ImGui::SameLine();
                if (ImGui::BeginMenu("Mesh3D")) {
                    drawMesh3DItemsToLoad();
                    ImGui::EndMenu();
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("gearIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::BeginMenu("RigidBody")) {
                    drawRigidBodiesItemsToLoad();
                    ImGui::EndMenu();
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("ghostIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::BeginMenu("GhostBody")) {
                    drawGhostItemsToLoad();
                    ImGui::EndMenu();
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("particlesIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("ParticleEmitter", "CTRL+x")) {
                    SceneLoader::createParticleEmitterInScene();
                    ImGui::EndMenu();
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("sprite3DIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Sprite3D", "CTRL+2")) {
                    SceneLoader::createSprite3DInScene();
                }
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("sprite2DIcon")->getOGLTextureID(), ImVec2(16, 16));
                ImGui::SameLine();
                if (ImGui::MenuItem("Sprite2D", "CTRL+2")) {
                    SceneLoader::createSprite2DInScene();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Render")) {
                ImGui::Separator();

                ImGui::Checkbox("Limit frame rate", &EngineSetup::get()->LIMIT_FRAMERATE);
                if (EngineSetup::get()->LIMIT_FRAMERATE) {
                    ImGui::DragScalar("Limite frames to:", ImGuiDataType_S32, &EngineSetup::get()->FRAMERATE, range_framerate_sensibility, &range_min_framerate_distance,&range_max_framerate_distance, "%d", 1.0f);

                }
                ImGui::Separator();

                ImGui::Checkbox("update Objects", &EngineSetup::get()->EXECUTE_GAMEOBJECTS_ONUPDATE);
                ImGui::Checkbox("Draw Main Z-Buffer", &EngineSetup::get()->DRAW_MAIN_DEEP_MAPPING);

                ImGui::Separator();

                ImGui::Checkbox("Tiled Based", &EngineSetup::get()->BASED_TILE_RENDER);

                if (EngineSetup::get()->BASED_TILE_RENDER) {
                    ImGui::Checkbox("Tiled Based Threaded", &EngineSetup::get()->BASED_TILE_RENDER_THREADED);
                    ImGui::Checkbox("Show Tiles Grid", &EngineSetup::get()->DRAW_TILES_GRID);
                }
                ImGui::Separator();
                ImGui::DragScalar("Frustum Far Plane Distance", ImGuiDataType_Float, &EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE, range_far_plane_distance_sensibility, &range_far_plane_min, &range_max_plane_max, "%f", 1.0f);
                ImGui::Separator();
                ImGui::Checkbox("Vertex", &EngineSetup::get()->TRIANGLE_MODE_PIXELS);
                ImGui::Checkbox("WireFrame", &EngineSetup::get()->TRIANGLE_MODE_WIREFRAME);
                ImGui::Checkbox("Solid", &EngineSetup::get()->TRIANGLE_MODE_COLOR_SOLID);
                ImGui::Separator();
                ImGui::Checkbox("Textures", &EngineSetup::get()->TRIANGLE_MODE_TEXTURIZED);
                ImGui::Separator();
                ImGui::Checkbox("Draw Bones", &EngineSetup::get()->DRAW_ANIMATION_BONES);
                ImGui::Separator();
                ImGui::Checkbox("Internal click selection", &EngineSetup::get()->CLICK_SELECT_OBJECT3D);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Sound")) {
                ImGui::Checkbox("Global enable", &EngineSetup::get()->SOUND_ENABLED);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    if (!EngineSetup::get()->SOUND_ENABLED) {
                        Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, 0);
                        Mix_VolumeMusic((int) EngineSetup::get()->SOUND_VOLUME_MUSIC);
                        Mix_VolumeMusic(0);
                    } else {
                        Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, (int) EngineSetup::get()->SOUND_CHANNEL_GLOBAL);
                        Mix_VolumeMusic((int) EngineSetup::get()->SOUND_VOLUME_MUSIC);
                    }
                }

                ImGui::DragScalar("Music volume", ImGuiDataType_Float, &EngineSetup::get()->SOUND_VOLUME_MUSIC, range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) { Mix_VolumeMusic((int )EngineSetup::get()->SOUND_VOLUME_MUSIC); }

                ImGui::DragScalar("Global Channel volume", ImGuiDataType_Float, &EngineSetup::get()->SOUND_CHANNEL_GLOBAL, range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, (int) EngineSetup::get()->SOUND_CHANNEL_GLOBAL);
                }

                /*ImGui::DragScalar("Player vol.", ImGuiDataType_Float, &EngineSetup::get()->SOUND_VOLUME_PLAYER,range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    Mix_Volume(EngineSetup::SoundChannels::SND_PLAYER, EngineSetup::get()->SOUND_VOLUME_PLAYER);
                }

                ImGui::DragScalar("Enemies vol.", ImGuiDataType_Float, &EngineSetup::get()->SOUND_VOLUME_ENEMIES,range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    Mix_Volume(EngineSetup::SoundChannels::SND_ENEMIES, EngineSetup::get()->SOUND_VOLUME_ENEMIES);
                }

                ImGui::DragScalar("Environment vol.", ImGuiDataType_Float,&EngineSetup::get()->SOUND_VOLUME_ENVIRONMENT, range_sensibility_volume,&range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    Mix_Volume(EngineSetup::SoundChannels::SND_ENVIRONMENT,EngineSetup::get()->SOUND_VOLUME_ENVIRONMENT);
                }*/

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Bullet")) {
                ImGui::Checkbox("Step Simulation", &EngineSetup::get()->BULLET_STEP_SIMULATION);
                ImGui::Checkbox("Collisions between objects", &EngineSetup::get()->BULLET_CHECK_ALL_PAIRS);
                ImGui::Separator();
                ImGui::Checkbox("Draw debug mode", &EngineSetup::get()->BULLET_DEBUG_MODE);
                ImGui::Separator();
                // gravity
                ImGui::DragScalar("X Gravity", ImGuiDataType_Float, &EngineSetup::get()->gravity.x, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::DragScalar("Y Gravity", ImGuiDataType_Float, &EngineSetup::get()->gravity.y, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::DragScalar("Z Gravity", ImGuiDataType_Float, &EngineSetup::get()->gravity.z, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::Separator();
                ImGui::DragScalar("Projectile Impulse", ImGuiDataType_Float, &EngineSetup::get()->PROJECTILE_DEMO_IMPULSE, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::DragScalar("Projectile Accuracy", ImGuiDataType_Float, &EngineSetup::get()->PROJECTILE_DEMO_ACCURACY, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::DragScalar("Projectile Mass", ImGuiDataType_Float, &EngineSetup::get()->PROJECTILE_DEMO_MASS, range_sensibility,&range_min_sensibility, &range_max_sensibility, "%f", 1.0f);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Logging")) {
                ImGui::Checkbox("Output to Console", &EngineSetup::get()->LOGGING);
                ImGui::Separator();
                ImGui::Checkbox("Show debug data", &EngineSetup::get()->DEBUG_RENDER_INFO);
                ImGui::Separator();
                ImGui::Checkbox("Collision objects", &EngineSetup::get()->LOG_COLLISION_OBJECTS);
                ImGui::Separator();
                ImGui::Checkbox("Weapon System", &EngineSetup::get()->LOG_WEAPONS_SYSTEM);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Lights")) {
                ImGui::Checkbox("Light System", &EngineSetup::get()->ENABLE_LIGHTS);
                ImGui::Separator();
                ImGui::Checkbox("Draw Lights Billboards", &EngineSetup::get()->DRAW_LIGHTPOINTS_BILLBOARD);
                ImGui::Separator();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Effects FX")) {
                if (ImGui::BeginMenu("Shader Particles")) {
                    ImGui::DragScalar("PARTICLES_SHADER_GRAVITY", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_GRAVITY, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_STEP_ADD_PARTICLE", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_STEP_ADD_PARTICLE, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_PARTICLE_LIFESPAN", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_PARTICLE_LIFESPAN, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_SMOKE_ANGLE_RANGE", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_SMOKE_ANGLE_RANGE, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_MIN_VELOCITY", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_MIN_VELOCITY, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_MAX_VELOCITY", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_MAX_VELOCITY, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_POSITION_NOISE", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_POSITION_NOISE, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_VELOCITY_NOISE", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_VELOCITY_NOISE, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("PARTICLES_SHADER_DECELERATION_FACTOR", ImGuiDataType_Float, &EngineSetup::get()->PARTICLES_SHADER_DECELERATION_FACTOR, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::Separator();
                    ImGui::DragScalar("SHADER EXPLOSION TTL", ImGuiDataType_Float, &EngineSetup::get()->SHADER_PARTICLE_EXPLOSION_TTL, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("SHADER_PARTICLE_EXPLOSION_EMISSION_TIME", ImGuiDataType_Float, &EngineSetup::get()->SHADER_PARTICLE_EXPLOSION_EMISSION_TIME, 0.1f, &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Lightning")) {
                    ImGui::DragScalar("Generations", ImGuiDataType_Float,
                                      &EngineSetup::get()->LIGHTNING_GENERATIONS, range_sensibility_lightnin,
                                      &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("Offset reduction", ImGuiDataType_Float,
                                      &EngineSetup::get()->LIGHTNING_OFFSET_REDUCTION,
                                      range_sensibility_lightnin, &range_sensibility_lightnin_min,
                                      &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("Probability branch", ImGuiDataType_Float,
                                      &EngineSetup::get()->LIGHTNING_PROBABILITY_BRANCH,
                                      range_sensibility_lightnin, &range_sensibility_lightnin_min,
                                      &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("Segment shift ", ImGuiDataType_Float,
                                      &EngineSetup::get()->LIGHTNING_SEGMENT_SHIFT, range_sensibility_lightnin,
                                      &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::Checkbox("FullScreen", &EngineSetup::get()->FULLSCREEN);

                if (ImGui::IsItemEdited()) {
                    if (EngineSetup::get()->FULLSCREEN) {
                        SDL_SetWindowFullscreen(ComponentsManager::get()->getComponentWindow()->getWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);

                    } else {
                        SDL_SetWindowFullscreen(ComponentsManager::get()->getComponentWindow()->getWindow(), 0);
                    }
                }
                ImGui::Separator();
                ImGui::Checkbox("Draw Main Axis", &EngineSetup::get()->RENDER_MAIN_AXIS);
                ImGui::Checkbox("Draw Object3D Axis", &EngineSetup::get()->RENDER_OBJECTS_AXIS);
                if (EngineSetup::get()->RENDER_OBJECTS_AXIS) {
                    const float sizeAxisMin = 0;
                    const float sizeAxisMax = 1;
                    const float sizeAxisSens = 0.01;

                    ImGui::DragScalar("Size Axis", ImGuiDataType_Float, &EngineSetup::get()->OBJECT_AXIS_SIZE,
                                      sizeAxisSens, &sizeAxisMin, &sizeAxisMax, "%f", 1.0f);

                }
                ImGui::Separator();
                ImGui::Checkbox("Draw light direction", &EngineSetup::get()->DRAW_LIGHTS_DIRECTION);
                if (EngineSetup::get()->DRAW_LIGHTS_DIRECTION) {
                    ImGui::DragScalar("Size Direction", ImGuiDataType_Float, &EngineSetup::get()->LIGHTS_DIRECTION_SIZE,
                                      range_test_sensibility, &range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                }

                ImGui::Separator();

                ImGui::Checkbox("Draw Mesh3D AABB", &EngineSetup::get()->DRAW_MESH3D_AABB);
                ImGui::Checkbox("Draw Mesh3D Octree", &EngineSetup::get()->DRAW_MESH3D_OCTREE);
                ImGui::Checkbox("Draw Mesh3D Grid", &EngineSetup::get()->DRAW_MESH3D_GRID);
                if (EngineSetup::get()->DRAW_MESH3D_GRID) {
                    ImGui::Separator();
                    ImGui::Checkbox("Grid3D: Empty", &EngineSetup::get()->DRAW_MESH3D_GRID_EMPTY);
                    ImGui::Checkbox("Grid3D: No Empty", &EngineSetup::get()->DRAW_MESH3D_GRID_NO_EMPTY);
                    ImGui::Separator();
                    ImGui::Checkbox("Grid3D: Draw Center", &EngineSetup::get()->DRAW_MESH3D_GRID_POINTS);
                    ImGui::Checkbox("Grid3D: Draw AABB", &EngineSetup::get()->DRAW_MESH3D_GRID_CUBES);
                    ImGui::Separator();
                }
                ImGui::Separator();
                ImGui::Checkbox("Pendulum thread", &EngineSetup::get()->DRAW_PENDULUM_THREAD);
                ImGui::Checkbox("Draw Object3D Billboards", &EngineSetup::get()->DRAW_OBJECT3D_BILLBOARD);
                ImGui::Checkbox("Draw Decals wireframe", &EngineSetup::get()->DRAW_DECAL_WIREFRAMES);
                ImGui::Separator();
                ImGui::Checkbox("Object3D Text Label", &EngineSetup::get()->TEXT_ON_OBJECT3D);
                ImGui::Separator();
                ImGui::Checkbox("Show CrossHair", &EngineSetup::get()->DRAW_CROSSHAIR);
                ImGui::Separator();
                ImGui::Checkbox("Show FPS", &EngineSetup::get()->DRAW_FPS);
                ImGui::Separator();
                ImGui::EndMenu();
            }
            static const char *itemCurrent = "--Niveles--"; // Here our selection is a single pointer stored outside the object.
            static ImGuiComboFlags flags = 0;

            ImGui::EndMainMenuBar();
        }
    }
    void drawMesh3DItemsToLoad() {
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string> result;
        if ((dir = opendir (directory_path_models.c_str())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                result.emplace_back(ent->d_name);
            }
            std::sort( result.begin(), result.end() );

            closedir (dir);

            for (int i = 0; i < result.size(); i++) {
                auto file = result[i];
                auto title = std::to_string(i-1) + ") " + file;
                if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("meshIcon")->getOGLTextureID(), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem(file.c_str())) {
                        Tools::addSceneObject(file, "added_item");
                    }
                }
            }
        }
    }

    void drawRigidBodiesItemsToLoad() {
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string> result;
        if ((dir = opendir (directory_path_models.c_str())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                result.emplace_back(ent->d_name);
            }
            std::sort( result.begin(), result.end() );

            closedir (dir);

            for (int i = 0; i < result.size(); i++) {
                auto file = result[i];
                auto title = std::to_string(i-1) + ") " + file;
                if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("gearIcon")->getOGLTextureID(), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem(file.c_str())) {
                        SceneLoader::createMesh3DBodyToScene(file, "added_item");
                    }
                }
            }
        }
    }

    void drawGhostItemsToLoad() {
        DIR *dir;
        struct dirent *ent;
        std::vector<std::string> result;
        if ((dir = opendir (directory_path_models.c_str())) != NULL) {
            while ((ent = readdir (dir)) != NULL) {
                result.emplace_back(ent->d_name);
            }
            std::sort( result.begin(), result.end() );

            closedir (dir);

            for (int i = 0; i < result.size(); i++) {
                auto file = result[i];
                auto title = std::to_string(i-1) + ") " + file;
                if (strcmp(file.c_str(), ".") != 0 && strcmp(file.c_str(), "..") != 0) {
                    ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("ghostIcon")->getOGLTextureID(), ImVec2(16, 16));
                    ImGui::SameLine();
                    if (ImGui::MenuItem(file.c_str())) {
                        SceneLoader::createGhostBody3DToScene(file, "added_item");
                    }
                }
            }
        }
    }

};
#endif //BRAKEZA3D_GUIWIDGETMENU_H