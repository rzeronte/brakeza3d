
#ifndef SDL2_3D_ENGINE_GUI_MENU_H
#define SDL2_3D_ENGINE_GUI_MENU_H


#include "GUI.h"
#include "../../imgui/imgui.h"

class GUI_Menu : public GUI  {
public:

    virtual ~GUI_Menu() {}

    virtual void draw(bool &done, bool &show_window_inspector, bool &show_window_lights_inspector, bool &show_window_log, bool &show_camera_info, bool &show_window_physics) {

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
        const float range_max_test_int = 3;

        const float range_sensibility = EngineSetup::getInstance()->GUI_BAR_SENSITIVITY;
        const float lod_sensibility = 0;
        const float range_test_sensibility = 0.1;

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
                    //if (EngineSetup::getInstance()->TRIANGLE_TEXTURES_ANIMATED) {
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

            if (ImGui::BeginMenu("View")) {
                ImGui::Checkbox("Camera Inspector", &show_camera_info);
                ImGui::Checkbox("3D Objects Inspector", &show_window_inspector);
                ImGui::Checkbox("Ligths Inspector", &show_window_lights_inspector);
                ImGui::Checkbox("Log", &show_window_log);
                ImGui::Checkbox("Tiles", &show_window_physics);
                ImGui::Separator();
                ImGui::Checkbox("Draw main Frustum", &EngineSetup::getInstance()->DRAW_FRUSTUM);
                ImGui::Separator();
                ImGui::Checkbox("Draw Main Axis", &EngineSetup::getInstance()->RENDER_MAIN_AXIS);
                ImGui::Checkbox("Draw Object3D Axis", &EngineSetup::getInstance()->RENDER_OBJECTS_AXIS);
                ImGui::Checkbox("Draw Object3D Billboards", &EngineSetup::getInstance()->DRAW_OBJECT3D_BILLBOARD);
                ImGui::Separator();
                ImGui::Checkbox("Object3D Text Label", &EngineSetup::getInstance()->TEXT_ON_OBJECT3D);
                ImGui::Separator();
                ImGui::Checkbox("BSP Hulls", &EngineSetup::getInstance()->DRAW_BSP_HULLS);
                ImGui::Separator();
                ImGui::Checkbox("Show weapon", &EngineSetup::getInstance()->SHOW_WEAPON);
                ImGui::Separator();
                ImGui::Checkbox("Show FPS", &EngineSetup::getInstance()->DRAW_FPS);
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
