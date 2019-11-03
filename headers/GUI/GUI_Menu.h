
#ifndef SDL2_3D_ENGINE_GUI_MENU_H
#define SDL2_3D_ENGINE_GUI_MENU_H


#include "GUI.h"
#include "../../imgui/imgui.h"
#include "../Misc/cJSON.h"
#include "../Render/Logging.h"

class GUI_Menu : public GUI  {
public:

    virtual ~GUI_Menu() {}

    virtual void draw(bool &done, bool &show_window_inspector, bool &show_window_lights_inspector, bool &show_window_log, bool &show_camera_info, bool &show_window_physics, bool &show_window_weapons, cJSON *maps) {

        bool show_about_window = false;

        const float range_min_radius = 1;
        const float range_max_radius = 100000;

        const float range_min_lightmap_intensity = 0;
        const float range_max_lightmap_intensity = 1;
        const float range_sensibility_lightmap_intensity = 0.0001;

        const float range_sensibility_lava = 0.05;
        const float range_sensibility_lava_min = -5;
        const float range_sensibility_lava_max = 5;

        const float range_sensibility_lightnin = 0.05;
        const float range_sensibility_lightnin_min = -10;
        const float range_sensibility_lightnin_max = 10;

        const float range_min_lod = 1;
        const float range_max_lod = 8;

        const float range_min_test_int = 0;
        const float range_max_test_int = 10;

        const float range_sensibility = EngineSetup::getInstance()->GUI_BAR_SENSITIVITY;
        const float lod_sensibility = 0;
        const float range_test_sensibility = 0.1;

        const float range_sensibility_volume = 1;
        const float range_min_volume = 1;
        const float range_max_volume = 128;


        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Brakeza3D")) {
                if (ImGui::MenuItem("About Brakeza", "CTRL+I")) show_about_window = true;
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "CTRL+W")) done = true;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Render")) {
                ImGui::Checkbox("Tiled Based", &EngineSetup::getInstance()->BASED_TILE_RENDER);
                if (EngineSetup::getInstance()->BASED_TILE_RENDER) {
                    ImGui::Checkbox("Show Tiles Grid", &EngineSetup::getInstance()->DRAW_TILES_GRID);
                }
                ImGui::Separator();
                ImGui::Checkbox("OpenCL Rasterizer", &EngineSetup::getInstance()->RASTERIZER_OPENCL);
                ImGui::Checkbox("OpenCL Transforms", &EngineSetup::getInstance()->TRANSFORMS_OPENCL);
                ImGui::Separator();
                ImGui::Checkbox("Vertex", &EngineSetup::getInstance()->TRIANGLE_MODE_PIXELS);
                ImGui::Checkbox("WireFrame", &EngineSetup::getInstance()->TRIANGLE_MODE_WIREFRAME);
                ImGui::Checkbox("Solid", &EngineSetup::getInstance()->TRIANGLE_MODE_COLOR_SOLID);
                ImGui::Separator();
                ImGui::Checkbox("Textures", &EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED);
                if (EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED) {
                    ImGui::Separator();
                    ImGui::Checkbox("UV Bilinear", &EngineSetup::getInstance()->TEXTURES_BILINEAR_INTERPOLATION);
                    ImGui::Checkbox("Mip-Mapping", &EngineSetup::getInstance()->ENABLE_MIPMAPPING);
                    if (!EngineSetup::getInstance()->ENABLE_MIPMAPPING) {
                        if (ImGui::TreeNode("Force LOD level")) {
                            static int selected = 1;
                            for (int n = 1; n <= 8; n = 2 * n) {
                                char buf[32];
                                sprintf(buf, "LOD %d", n);
                                if (ImGui::Selectable(buf, selected == n)) {
                                    selected = n;
                                    EngineSetup::getInstance()->LOAD_OF_DETAIL = selected;
                                }
                            }
                            ImGui::TreePop();
                        }
                    }
                    ImGui::Separator();
                    ImGui::Checkbox("Lightmaps", &EngineSetup::getInstance()->ENABLE_LIGHTMAPPING);
                    if (EngineSetup::getInstance()->ENABLE_LIGHTMAPPING) {
                        ImGui::Checkbox("Lightmap Bilinear", &EngineSetup::getInstance()->LIGHTMAPS_BILINEAR_INTERPOLATION);
                        ImGui::Checkbox("Show Lightmaps", &EngineSetup::getInstance()->SHOW_LIGHTMAPPING);
                    }
                }
                ImGui::Separator();
                ImGui::Checkbox("Triangle Face-culling", &EngineSetup::getInstance()->TRIANGLE_BACK_FACECULLING);
                ImGui::Separator();
                ImGui::Checkbox("Depth Buffer", &EngineSetup::getInstance()->TRIANGLE_RENDER_DEPTH_BUFFER);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Sound")) {
                ImGui::Checkbox("Global enable", &EngineSetup::getInstance()->SOUND_ENABLED);
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    if (!EngineSetup::getInstance()->SOUND_ENABLED) {
                        Mix_Volume(EngineSetup::SoundChannels::SND_MENU, 0 );
                        Mix_Volume(EngineSetup::SoundChannels::SND_PLAYER, 0 );
                        Mix_Volume(EngineSetup::SoundChannels::SND_ENVIRONMENT, 0 );
                        Mix_VolumeMusic(0);
                    } else {
                        Mix_Volume(EngineSetup::SoundChannels::SND_MENU, EngineSetup::getInstance()->SOUND_VOLUME_MENU);
                        Mix_Volume(EngineSetup::SoundChannels::SND_PLAYER, EngineSetup::getInstance()->SOUND_VOLUME_PLAYER);
                        Mix_Volume(EngineSetup::SoundChannels::SND_ENVIRONMENT, EngineSetup::getInstance()->SOUND_VOLUME_ENVIRONMENT);
                        Mix_VolumeMusic(EngineSetup::getInstance()->SOUND_VOLUME_MUSIC);
                    }
                }

                ImGui::DragScalar("Music vol.", ImGuiDataType_Float, &EngineSetup::getInstance()->SOUND_VOLUME_MUSIC, range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) { Mix_VolumeMusic(EngineSetup::getInstance()->SOUND_VOLUME_MUSIC); }

                ImGui::DragScalar("Menu vol.", ImGuiDataType_Float, &EngineSetup::getInstance()->SOUND_VOLUME_MENU, range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) { Mix_Volume(EngineSetup::SoundChannels::SND_MENU, EngineSetup::getInstance()->SOUND_VOLUME_MENU); }

                ImGui::DragScalar("Player vol.", ImGuiDataType_Float, &EngineSetup::getInstance()->SOUND_VOLUME_PLAYER, range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) { Mix_Volume(EngineSetup::SoundChannels::SND_PLAYER, EngineSetup::getInstance()->SOUND_VOLUME_PLAYER); }

                ImGui::DragScalar("Environment vol.", ImGuiDataType_Float, &EngineSetup::getInstance()->SOUND_VOLUME_ENVIRONMENT, range_sensibility_volume, &range_min_volume, &range_max_volume, "%f", 1.0f);
                if (ImGui::IsItemEdited()) { Mix_Volume(EngineSetup::SoundChannels::SND_ENVIRONMENT, EngineSetup::getInstance()->SOUND_VOLUME_ENVIRONMENT); }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Physics")) {
                ImGui::Separator();
                ImGui::Checkbox("StepSimulation", &EngineSetup::getInstance()->BULLET_STEP_SIMULATION);
                ImGui::Checkbox("Debug Mode", &EngineSetup::getInstance()->BULLET_DEBUG_MODE);
                ImGui::Checkbox("Check All Pairs", &EngineSetup::getInstance()->BULLET_CHECK_ALL_PAIRS);
                ImGui::Separator();
                ImGui::DragScalar("Radius", ImGuiDataType_Float,  &EngineSetup::getInstance()->PLAYER_SPHERE_RADIUS, range_sensibility,  &range_min_radius, &range_max_radius, "%f", 1.0f);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Logging")) {
                ImGui::Checkbox("Show debug data", &EngineSetup::getInstance()->DEBUG_RENDER_INFO);
                ImGui::Separator();
                ImGui::Checkbox("Collision objects", &EngineSetup::getInstance()->LOG_COLLISION_OBJECTS);
                ImGui::Separator();
                ImGui::Checkbox("BSP type Leaf", &EngineSetup::getInstance()->LOG_LEAF_TYPE);
                ImGui::Separator();
                ImGui::Checkbox("OpenCL kernel time", &EngineSetup::getInstance()->OPENCL_SHOW_TIME_KERNELS);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Lights")) {
                ImGui::Checkbox("Light System", &EngineSetup::getInstance()->ENABLE_LIGHTS);
                if (EngineSetup::getInstance()->ENABLE_LIGHTS) {
                    ImGui::Checkbox("Shadow Casting", &EngineSetup::getInstance()->ENABLE_SHADOW_CASTING);
                }
                ImGui::Checkbox("Draw Lights Billboards", &EngineSetup::getInstance()->DRAW_LIGHTPOINTS_BILLBOARD);
                ImGui::Checkbox("Draw Lights Axis", &EngineSetup::getInstance()->DRAW_LIGHTPOINTS_AXIS);
                ImGui::Separator();
                ImGui::DragScalar("BSP Lightmapping Intensity", ImGuiDataType_Float,  &EngineSetup::getInstance()->LIGHTMAPPING_INTENSITY, range_sensibility_lightmap_intensity,  &range_min_lightmap_intensity, &range_max_lightmap_intensity, "%f", 1.0f);
                ImGui::DragScalar("Texture color Intensity", ImGuiDataType_Float,  &EngineSetup::getInstance()->TEXTURE_INTENSITY, range_sensibility_lightmap_intensity,  &range_min_lightmap_intensity, &range_max_lightmap_intensity, "%f", 1.0f);
                ImGui::EndMenu();
            }


            if (ImGui::BeginMenu("Effects FX")) {
                if (ImGui::BeginMenu("Liquid Shader")) {
                    ImGui::Checkbox("Animated textures", &EngineSetup::getInstance()->TRIANGLE_TEXTURES_ANIMATED);
                    //if (EngineSetup::get()->TRIANGLE_TEXTURES_ANIMATED) {
                    ImGui::DragScalar("Liquid Closeness", ImGuiDataType_Float,  &EngineSetup::getInstance()->LAVA_CLOSENESS, range_sensibility_lava,  &range_sensibility_lava_min, &range_sensibility_lava_max, "%f", 1.0f);
                    ImGui::DragScalar("Liquid Speed", ImGuiDataType_Float,  &EngineSetup::getInstance()->LAVA_SPEED, range_sensibility_lava,  &range_sensibility_lava_min, &range_sensibility_lava_max, "%f", 1.0f);
                    ImGui::DragScalar("Liquid Scale", ImGuiDataType_Float,  &EngineSetup::getInstance()->LAVA_SCALE, range_sensibility_lava,  &range_sensibility_lava_min, &range_sensibility_lava_max, "%f", 1.0f);
                    ImGui::DragScalar("Liquid Intensity", ImGuiDataType_Float,  &EngineSetup::getInstance()->LAVA_INTENSITY, range_sensibility_lava,  &range_sensibility_lava_min, &range_sensibility_lava_max, "%f", 1.0f);
                    //}
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Lightning")) {
                    ImGui::DragScalar("Generations", ImGuiDataType_Float,  &EngineSetup::getInstance()->LIGHTNING_GENERATIONS, range_sensibility_lightnin,  &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("Offset reduction", ImGuiDataType_Float,  &EngineSetup::getInstance()->LIGHTNING_OFFSET_REDUCTION, range_sensibility_lightnin,  &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("Probability branch", ImGuiDataType_Float,  &EngineSetup::getInstance()->LIGHTNING_PROBABILITY_BRANCH, range_sensibility_lightnin,  &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::DragScalar("Segment shift ", ImGuiDataType_Float,  &EngineSetup::getInstance()->LIGHTNING_SEGMENT_SHIFT, range_sensibility_lightnin,  &range_sensibility_lightnin_min, &range_sensibility_lightnin_max, "%f", 1.0f);
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Maps")) {
                ImGui::Checkbox("Render map", &EngineSetup::getInstance()->RENDER_BSP_MAP);

                cJSON *currentMap = NULL;
                cJSON_ArrayForEach(currentMap, maps) {
                    cJSON *name = cJSON_GetObjectItemCaseSensitive(currentMap, "name");
                    if (cJSON_IsString(name)) {
                        ImGui::MenuItem(std::string(name->valuestring).c_str(), "", false);
                        if (ImGui::IsItemClicked()) {
                            EngineSetup::getInstance()->EVENT_GUI = true;
                            EngineSetup::getInstance()->EVENT_LAUNCH = EngineSetup::getInstance()->EVENT_GUI_CHANGE_MAP;
                            EngineSetup::getInstance()->EVENT_DATA = name->valuestring;
                        }

                        ImGui::Separator();
                    }
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::Checkbox("Camera Inspector", &show_camera_info);
                ImGui::Checkbox("3D Objects Inspector", &show_window_inspector);
                ImGui::Checkbox("Ligths Inspector", &show_window_lights_inspector);
                ImGui::Checkbox("Log", &show_window_log);
                ImGui::Checkbox("Tiles", &show_window_physics);
                ImGui::Checkbox("Weapons", &show_window_weapons);
                ImGui::Separator();
                ImGui::Checkbox("Draw main Frustum", &EngineSetup::getInstance()->DRAW_FRUSTUM);
                ImGui::Separator();
                ImGui::Checkbox("Draw Main Axis", &EngineSetup::getInstance()->RENDER_MAIN_AXIS);
                ImGui::Checkbox("Draw Object3D Axis", &EngineSetup::getInstance()->RENDER_OBJECTS_AXIS);
                ImGui::Checkbox("Draw Object3D Billboards", &EngineSetup::getInstance()->DRAW_OBJECT3D_BILLBOARD);
                ImGui::Checkbox("Draw Decals wireframe", &EngineSetup::getInstance()->DRAW_DECAL_WIREFRAMES);
                ImGui::Separator();
                ImGui::Checkbox("Object3D Text Label", &EngineSetup::getInstance()->TEXT_ON_OBJECT3D);
                ImGui::Separator();
                ImGui::Checkbox("BSP Hulls", &EngineSetup::getInstance()->DRAW_BSP_HULLS);
                ImGui::Checkbox("Sprites", &EngineSetup::getInstance()->DRAW_SPRITES);
                ImGui::Separator();
                ImGui::Checkbox("Show FPS", &EngineSetup::getInstance()->DRAW_FPS);
                ImGui::Separator();
                ImGui::Checkbox("Show Menu Screen", &EngineSetup::getInstance()->MENU_ACTIVE);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Game")) {
                ImGui::Checkbox("Enable IA", &EngineSetup::getInstance()->ENABLE_IA);
                ImGui::Separator();
                ImGui::Checkbox("Show HUD", &EngineSetup::getInstance()->DRAW_HUD);
                ImGui::Checkbox("Show CrossHair", &EngineSetup::getInstance()->DRAW_CROSSHAIR);
                ImGui::Checkbox("Show weapon", &EngineSetup::getInstance()->DRAW_WEAPON);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Developers")) {
                ImGui::DragScalar("TESTING", ImGuiDataType_Float,  &EngineSetup::getInstance()->TESTING, range_test_sensibility,  &range_min_radius, &range_max_radius, "%f", 1.0f);
                ImGui::DragScalar("TESTING INT", ImGuiDataType_Float,  &EngineSetup::getInstance()->TESTING_INT, 1,  &range_min_test_int, &range_max_test_int, "%f", 1.0f);
                ImGui::Checkbox("Boolean TEST", &EngineSetup::getInstance()->TESTING_BOOL);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (show_about_window) ImGui::OpenPopup("New");

        if (ImGui::BeginPopup("New"))  {
            ImGui::Text("Brakeza v.0.6.");
            ImGui::Text("Eduardo Rodríguez <eduardo@brakeza.com>");
            ImGui::Text("https://brakeza.com");
            ImGui::EndPopup();
        }
    }
};


#endif //SDL2_3D_ENGINE_GUI_MENU_H
