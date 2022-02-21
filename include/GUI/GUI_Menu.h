
#ifndef SDL2_3D_ENGINE_GUI_MENU_H
#define SDL2_3D_ENGINE_GUI_MENU_H


#include "GUI.h"
#include "../../imgui/imgui.h"
#include "../Misc/cJSON.h"
#include "../Render/Logging.h"
#include "../EngineSetup.h"
#include "../Components/Camera3D.h"
#include "../Misc/Tools.h"

class GUI_Menu : public GUI {
public:

    virtual ~GUI_Menu() {}

    virtual void
    draw(bool &done, bool &show_window_inspector, bool &show_camera_info,
         bool &show_window_physics, bool &show_window_weapons, Camera3D *cam) {

        bool show_about_window = false;

        const float range_min_radius = 1;
        const float range_max_radius = 100000;

        const float range_min_lightmap_intensity = 0;
        const float range_max_lightmap_intensity = 1;
        const float range_sensibility_lightmap_intensity = 0.0001;

        const float range_min_fog_intensity = 0;
        const float range_max_fog_intensity = 1;
        const float range_sensibility_fog_intensity = 0.01;

        const float range_min_fog_distance = 0;
        const float range_max_fog_distance = 5000;
        const float range_sensibility_fog_distance = 1;

        const float range_sensibility_lava = 0.05;
        const float range_sensibility_lava_min = -5;
        const float range_sensibility_lava_max = 5;

        const float range_sensibility_lightnin = 0.05;
        const float range_sensibility_lightnin_min = -10;
        const float range_sensibility_lightnin_max = 10;

        const float range_sensibility_fire = 1;
        const float range_sensibility_fire_min = 0;
        const float range_sensibility_fire_max = 512;


        const float range_min_test_int = 0;
        const float range_max_test_int = 10000;

        const float range_sensibility = 0.75f;
        const float range_test_sensibility = 0.1;
        const float range_max_sensibility = 999;
        const float range_min_sensibility = -999;

        const float range_sensibility_volume = 1;
        const float range_min_volume = 1;
        const float range_max_volume = 128;

        const float range_frustum_fardistance_sensibility = 1;
        const float range_min_frustum_fardistance = 1;
        const float range_max_frustum_fardistance = 10000;

        const float range_frustum_clipping_distance_sensibility = 0.001;
        const float range_min_frustum_clipping_distance = 0;
        const float range_max_frustum_clipping_distance = 1;

        const float range_fov_sensibility = 1;
        const float range_min_fov = 20;
        const float range_max_fov = 160;

        const float range_stepsimulation_multiplier_sensibility = 0.01;
        const float range_min_stepsimulation_multiplier = 0;
        const float range_max_stepsimulation_multiplier = 1;

        bool changedFOGcolor = false;
        int misc_flags = ImGuiColorEditFlags_NoOptions;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Brakeza3D")) {
                if (ImGui::MenuItem("About Brakeza", "CTRL+I")) show_about_window = true;
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "CTRL+W")) done = true;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Render")) {

                ImGui::Checkbox("onUpdate Objects", &EngineSetup::get()->EXECUTE_GAMEOBJECTS_ONUPDATE);
                ImGui::Checkbox("Draw Main Z-Buffer", &EngineSetup::get()->DRAW_MAIN_DEEP_MAPPING);

                ImGui::Separator();
                ImGui::DragScalar("FOV", ImGuiDataType_Float, &EngineSetup::get()->HORIZONTAL_FOV,
                                  range_fov_sensibility, &range_min_fov, &range_max_fov, "%f", 1.0f);

                if (ImGui::IsItemEdited()) {
                    cam->frustum->setup(
                            cam->getPosition(),
                            Vertex3D(0, 0, 1),
                            EngineSetup::get()->up,
                            EngineSetup::get()->right,
                            EngineSetup::get()->HORIZONTAL_FOV,
                            ((float) EngineSetup::get()->screenHeight / (float) EngineSetup::get()->screenWidth),
                            EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE
                    );
                    cam->updateFrustum();
                }
                ImGui::Separator();

                ImGui::Checkbox("Tiled Based", &EngineSetup::get()->BASED_TILE_RENDER);

                if (EngineSetup::get()->BASED_TILE_RENDER) {
                    ImGui::Checkbox("Tiled Based Threaded", &EngineSetup::get()->BASED_TILE_RENDER_THREADED);
                    ImGui::Checkbox("Show Tiles Grid", &EngineSetup::get()->DRAW_TILES_GRID);
                }
                ImGui::Separator();
                ImGui::Checkbox("Frustum Clipping", &EngineSetup::get()->ENABLE_CLIPPING);
                if (EngineSetup::get()->ENABLE_CLIPPING) {
                    ImGui::DragScalar("Frustum Clipping Distance", ImGuiDataType_Float,
                                      &EngineSetup::get()->FRUSTUM_CLIPPING_DISTANCE,
                                      range_frustum_clipping_distance_sensibility, &range_min_frustum_clipping_distance,
                                      &range_max_frustum_clipping_distance, "%f", 1.0f);
                    ImGui::DragScalar("EPSILON", ImGuiDataType_Float, &EngineSetup::get()->EPSILON,
                                      range_frustum_clipping_distance_sensibility, &range_min_frustum_clipping_distance,
                                      &range_max_frustum_clipping_distance, "%f", 1.0f);
                }

                ImGui::Separator();
                ImGui::DragScalar("Frustum FarDistance", ImGuiDataType_Float,
                                  &EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE,
                                  range_frustum_fardistance_sensibility, &range_min_frustum_fardistance,
                                  &range_max_frustum_fardistance, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    cam->frustum->setup(
                            cam->getPosition(),
                            Vertex3D(0, 0, 1),
                            EngineSetup::get()->up,
                            EngineSetup::get()->right,
                            EngineSetup::get()->HORIZONTAL_FOV,
                            ((float) EngineSetup::get()->screenHeight / (float) EngineSetup::get()->screenWidth),
                            EngineSetup::get()->FRUSTUM_FARPLANE_DISTANCE
                    );
                    cam->updateFrustum();
                }

                ImGui::Separator();

                ImGui::Checkbox("Enable FOG", &EngineSetup::get()->ENABLE_FOG);
                changedFOGcolor = ImGui::ColorEdit4("FOG Color", (float *) &EngineSetup::get()->FOG_IMGUI_COLOR,
                                                    misc_flags);

                if (changedFOGcolor) {
                    EngineSetup::get()->FOG_COLOR = Color(
                            EngineSetup::get()->FOG_IMGUI_COLOR.x * 256,
                            EngineSetup::get()->FOG_IMGUI_COLOR.y * 256,
                            EngineSetup::get()->FOG_IMGUI_COLOR.z * 256
                    );
                }

                ImGui::Separator();

                ImGui::DragScalar("FOG Intensity", ImGuiDataType_Float, &EngineSetup::get()->FOG_INTENSITY,
                                  range_sensibility_fog_intensity, &range_min_fog_intensity, &range_max_fog_intensity,
                                  "%f", 1.0f);
                ImGui::DragScalar("FOG Distance", ImGuiDataType_Float, &EngineSetup::get()->FOG_DISTANCE,
                                  range_sensibility_fog_distance, &range_min_fog_distance, &range_max_fog_distance,
                                  "%f", 1.0f);

                ImGui::Separator();
                ImGui::Checkbox("Vertex", &EngineSetup::get()->TRIANGLE_MODE_PIXELS);
                ImGui::Checkbox("WireFrame", &EngineSetup::get()->TRIANGLE_MODE_WIREFRAME);
                ImGui::Checkbox("Solid", &EngineSetup::get()->TRIANGLE_MODE_COLOR_SOLID);
                ImGui::Separator();
                ImGui::Checkbox("Textures", &EngineSetup::get()->TRIANGLE_MODE_TEXTURIZED);
                if (EngineSetup::get()->TRIANGLE_MODE_TEXTURIZED) {
                    ImGui::Separator();
                    ImGui::Checkbox("UV Bilinear", &EngineSetup::get()->TEXTURES_BILINEAR_INTERPOLATION);
                    ImGui::Checkbox("Mip-Mapping", &EngineSetup::get()->ENABLE_MIPMAPPING);
                    if (!EngineSetup::get()->ENABLE_MIPMAPPING) {
                        if (ImGui::TreeNode("Force LOD level")) {
                            static int selected = 1;
                            for (int n = 1; n <= 8; n = 2 * n) {
                                char buf[32];
                                sprintf(buf, "LOD %d", n);
                                if (ImGui::Selectable(buf, selected == n)) {
                                    selected = n;
                                    EngineSetup::get()->LOAD_OF_DETAIL = selected;
                                }
                            }
                            ImGui::TreePop();
                        }
                    }

                    ImGui::Separator();
                    ImGui::Checkbox("Lightmaps", &EngineSetup::get()->ENABLE_LIGHTMAPPING);
                    if (EngineSetup::get()->ENABLE_LIGHTMAPPING) {
                        ImGui::Checkbox("Lightmap Bilinear",
                                        &EngineSetup::get()->LIGHTMAPS_BILINEAR_INTERPOLATION);
                        ImGui::Checkbox("Show Lightmaps", &EngineSetup::get()->SHOW_LIGHTMAPPING);
                    }
                }
                ImGui::Separator();
                ImGui::Checkbox("Draw Bones", &EngineSetup::get()->DRAW_ANIMATION_BONES);
                ImGui::Separator();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Sound")) {
                ImGui::Checkbox("Global enable", &EngineSetup::get()->SOUND_ENABLED);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    if (!EngineSetup::get()->SOUND_ENABLED) {
                        Mix_Volume(EngineSetup::SoundChannels::SND_MENU, 0);
                        Mix_Volume(EngineSetup::SoundChannels::SND_PLAYER, 0);
                        Mix_Volume(EngineSetup::SoundChannels::SND_ENVIRONMENT, 0);
                        Mix_VolumeMusic(0);
                    } else {
                        Mix_Volume(EngineSetup::SoundChannels::SND_MENU, EngineSetup::get()->SOUND_VOLUME_MENU);
                        Mix_Volume(EngineSetup::SoundChannels::SND_PLAYER,
                                   EngineSetup::get()->SOUND_VOLUME_PLAYER);
                        Mix_Volume(EngineSetup::SoundChannels::SND_ENVIRONMENT,
                                   EngineSetup::get()->SOUND_VOLUME_ENVIRONMENT);
                        Mix_VolumeMusic(EngineSetup::get()->SOUND_VOLUME_MUSIC);
                    }
                }

                ImGui::DragScalar("Music vol.", ImGuiDataType_Float, &EngineSetup::get()->SOUND_VOLUME_MUSIC,
                                  range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) { Mix_VolumeMusic(EngineSetup::get()->SOUND_VOLUME_MUSIC); }

                ImGui::DragScalar("Menu vol.", ImGuiDataType_Float, &EngineSetup::get()->SOUND_VOLUME_MENU,
                                  range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    Mix_Volume(EngineSetup::SoundChannels::SND_MENU, EngineSetup::get()->SOUND_VOLUME_MENU);
                }

                ImGui::DragScalar("Player vol.", ImGuiDataType_Float, &EngineSetup::get()->SOUND_VOLUME_PLAYER,
                                  range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    Mix_Volume(EngineSetup::SoundChannels::SND_PLAYER, EngineSetup::get()->SOUND_VOLUME_PLAYER);
                }

                ImGui::DragScalar("Environment vol.", ImGuiDataType_Float,
                                  &EngineSetup::get()->SOUND_VOLUME_ENVIRONMENT, range_sensibility_volume,
                                  &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) {
                    Mix_Volume(EngineSetup::SoundChannels::SND_ENVIRONMENT,
                               EngineSetup::get()->SOUND_VOLUME_ENVIRONMENT);
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Physics")) {
                ImGui::Checkbox("StepSimulation", &EngineSetup::get()->BULLET_STEP_SIMULATION);

                ImGui::Separator();
                ImGui::Checkbox("Debug Mode", &EngineSetup::get()->BULLET_DEBUG_MODE);
                ImGui::Checkbox("Check All Pairs", &EngineSetup::get()->BULLET_CHECK_ALL_PAIRS);

                ImGui::Separator();
                // gravity
                ImGui::DragScalar("X Gravity", ImGuiDataType_Float, &EngineSetup::get()->gravity.x, range_sensibility,
                                  &range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::DragScalar("Y Gravity", ImGuiDataType_Float, &EngineSetup::get()->gravity.y, range_sensibility,
                                  &range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::DragScalar("Z Gravity", ImGuiDataType_Float, &EngineSetup::get()->gravity.z, range_sensibility,
                                  &range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Logging")) {
                ImGui::Checkbox("Show debug data", &EngineSetup::get()->DEBUG_RENDER_INFO);
                ImGui::Separator();
                ImGui::Checkbox("Collision objects", &EngineSetup::get()->LOG_COLLISION_OBJECTS);
                ImGui::Separator();
                ImGui::Checkbox("Weapon System", &EngineSetup::get()->LOG_WEAPONS_SYSTEM);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Lights")) {
                ImGui::Checkbox("Light System", &EngineSetup::get()->ENABLE_LIGHTS);
                ImGui::Checkbox("Create lights Z-Buffer", &EngineSetup::get()->CREATE_LIGHT_ZBUFFER);
                if (EngineSetup::get()->CREATE_LIGHT_ZBUFFER) {
                    ImGui::Checkbox("Lights Deep Test", &EngineSetup::get()->ENABLE_SHADOW_MAPPING);
                }
                ImGui::Separator();
                ImGui::Checkbox("Specularity", &EngineSetup::get()->ENABLE_LIGHTS_SPECULAR);

                ImGui::Separator();
                ImGui::Checkbox("Draw Lights Billboards", &EngineSetup::get()->DRAW_LIGHTPOINTS_BILLBOARD);
                ImGui::Separator();
                ImGui::DragScalar("Lightmap Blend Intensity", ImGuiDataType_Float,
                                  &EngineSetup::get()->LIGHTMAPPING_BLEND_INTENSITY,
                                  range_sensibility_lightmap_intensity, &range_min_lightmap_intensity,
                                  &range_max_lightmap_intensity, "%f", 1.0f);
                ImGui::DragScalar("Lightmap Intensity", ImGuiDataType_Float,
                                  &EngineSetup::get()->LIGHTMAPPING_INTENSITY,
                                  range_sensibility_lightmap_intensity, &range_min_lightmap_intensity,
                                  &range_max_lightmap_intensity, "%f", 1.0f);
                ImGui::EndMenu();
            }


            if (ImGui::BeginMenu("Effects FX")) {
                if (ImGui::BeginMenu("Liquid Shader")) {
                    ImGui::Checkbox("Animated textures", &EngineSetup::get()->TRIANGLE_TEXTURES_ANIMATED);
                    //if (EngineSetup::get()->TRIANGLE_TEXTURES_ANIMATED) {
                    ImGui::DragScalar("Liquid Closeness", ImGuiDataType_Float,
                                      &EngineSetup::get()->LAVA_CLOSENESS, range_sensibility_lava,
                                      &range_sensibility_lava_min, &range_sensibility_lava_max, "%f", 1.0f);
                    ImGui::DragScalar("Liquid Speed", ImGuiDataType_Float, &EngineSetup::get()->LAVA_SPEED,
                                      range_sensibility_lava, &range_sensibility_lava_min, &range_sensibility_lava_max,
                                      "%f", 1.0f);
                    ImGui::DragScalar("Liquid Scale", ImGuiDataType_Float, &EngineSetup::get()->LAVA_SCALE,
                                      range_sensibility_lava, &range_sensibility_lava_min, &range_sensibility_lava_max,
                                      "%f", 1.0f);
                    ImGui::DragScalar("Liquid Intensity", ImGuiDataType_Float,
                                      &EngineSetup::get()->LAVA_INTENSITY, range_sensibility_lava,
                                      &range_sensibility_lava_min, &range_sensibility_lava_max, "%f", 1.0f);
                    //}
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

                if (ImGui::BeginMenu("Fire Effect")) {
                    ImGui::DragScalar("Divisor1", ImGuiDataType_Float, &EngineSetup::get()->FIRE_DIVISOR1, 1,
                                      &range_sensibility_fire_min, &range_sensibility_fire_max, "%f", 1.0f);
                    ImGui::DragScalar("Divisor2", ImGuiDataType_Float, &EngineSetup::get()->FIRE_DIVISOR2, 1,
                                      &range_sensibility_fire_min, &range_sensibility_fire_max, "%f", 1.0f);
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::Checkbox("3D Objects Inspector", &show_window_inspector);
                ImGui::Checkbox("Camera Inspector", &show_camera_info);
                ImGui::Separator();
                ImGui::Checkbox("Tiles", &show_window_physics);
                ImGui::Checkbox("Weapons", &show_window_weapons);
                ImGui::Separator();
                ImGui::Checkbox("Draw main Frustum", &EngineSetup::get()->DRAW_FRUSTUM);
                ImGui::Separator();
                ImGui::Checkbox("Draw Main Axis", &EngineSetup::get()->RENDER_MAIN_AXIS);
                ImGui::Checkbox("Draw Object3D Axis", &EngineSetup::get()->RENDER_OBJECTS_AXIS);
                if (EngineSetup::get()->RENDER_OBJECTS_AXIS) {
                    ImGui::Checkbox("Draw Lights Axis", &EngineSetup::get()->DRAW_LIGHTPOINTS_AXIS);
                    ImGui::DragScalar("Size Axis", ImGuiDataType_Float, &EngineSetup::get()->OBJECT_AXIS_SIZE,
                                      range_test_sensibility, &range_min_sensibility, &range_max_sensibility, "%f", 1.0f);

                }
                ImGui::Separator();
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
                ImGui::Checkbox("Show FPS", &EngineSetup::get()->DRAW_FPS);
                ImGui::Separator();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Game")) {
                ImGui::Checkbox("Enable IA", &EngineSetup::get()->ENABLE_IA);
                ImGui::Separator();
                ImGui::Checkbox("Show HUD", &EngineSetup::get()->DRAW_HUD);
                ImGui::Checkbox("Show CrossHair", &EngineSetup::get()->DRAW_CROSSHAIR);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Developers")) {
                ImGui::DragScalar("TESTING", ImGuiDataType_Float, &EngineSetup::get()->TESTING,range_test_sensibility, &range_min_sensibility, &range_max_sensibility, "%f", 1.0f);
                ImGui::DragScalar("TESTING INT1", ImGuiDataType_Float, &EngineSetup::get()->TESTING_INT1, 1,&range_min_test_int, &range_max_test_int, "%f", 1.0f);
                ImGui::DragScalar("TESTING INT2", ImGuiDataType_Float, &EngineSetup::get()->TESTING_INT2, 1,&range_min_test_int, &range_max_test_int, "%f", 1.0f);
                ImGui::DragScalar("TESTING INT3", ImGuiDataType_Float, &EngineSetup::get()->TESTING_INT3, 1,&range_min_test_int, &range_max_test_int, "%f", 1.0f);
                ImGui::DragScalar("TESTING INT4", ImGuiDataType_Float, &EngineSetup::get()->TESTING_INT4, 1,&range_min_test_int, &range_max_test_int, "%f", 1.0f);
                ImGui::Checkbox("Boolean TEST", &EngineSetup::get()->TESTING_BOOL);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (show_about_window) ImGui::OpenPopup("New");

        if (ImGui::BeginPopup("New")) {
            ImGui::Text("Brakeza v.0.6.");
            ImGui::Text("Eduardo Rodríguez <eduardo@brakeza.com>");
            ImGui::Text("https://brakeza.com");
            ImGui::EndPopup();
        }
    }
};


#endif //SDL2_3D_ENGINE_GUI_MENU_H
