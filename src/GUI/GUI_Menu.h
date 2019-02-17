
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
        const float range_max_lightmap_intensity = 0.5;
        const float range_sensibility_lightmap_intensity = 0.0001;

        const float range_min_lod = 1;
        const float range_max_lod = 8;

        const float range_sensibility = EngineSetup::getInstance()->GUI_BAR_SENSITIVITY;
        const float lod_sensibility = 0;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Brakeza3D")) {
                if (ImGui::MenuItem("About Brakeza", "CTRL+I")) show_about_window = true;
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "CTRL+W")) done = true;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Render")) {
                ImGui::Checkbox("Vertex", &EngineSetup::getInstance()->TRIANGLE_MODE_PIXELS);
                ImGui::Checkbox("WireFrame", &EngineSetup::getInstance()->TRIANGLE_MODE_WIREFRAME);
                ImGui::Checkbox("Solid", &EngineSetup::getInstance()->TRIANGLE_MODE_COLOR_SOLID);
                ImGui::Separator();
                ImGui::Checkbox("Textures", &EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED);
                if (EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED) {
                    ImGui::Checkbox("Bilinear Interpolation", &EngineSetup::getInstance()->TEXTURES_BILINEAR_INTERPOLATION);
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
                    ImGui::Checkbox("Lightmaps", &EngineSetup::getInstance()->ENABLE_LIGHTMAPPING);
                    if (EngineSetup::getInstance()->ENABLE_LIGHTMAPPING) {
                        ImGui::Checkbox("Show Lightmaps", &EngineSetup::getInstance()->SHOW_LIGHTMAPPING);
                    }
                }
                ImGui::Separator();
                ImGui::Checkbox("Draw extra line", &EngineSetup::getInstance()->TRIANGLE_DEMO_EXTRALINE_ENABLED);
                ImGui::Separator();
                ImGui::Checkbox("Triangle Face-culling", &EngineSetup::getInstance()->TRIANGLE_BACK_FACECULLING);
                if (EngineSetup::getInstance()->TRIANGLE_BACK_FACECULLING) {
                    ImGui::Checkbox("Show BFC triangles", &EngineSetup::getInstance()->SHOW_WIREFRAME_FOR_BFC_HIDDEN_TRIANGLES);
                }
                ImGui::Separator();
                ImGui::Checkbox("Depth Buffer", &EngineSetup::getInstance()->TRIANGLE_RENDER_DEPTH_BUFFER);
                ImGui::Checkbox("Frustum Culling", &EngineSetup::getInstance()->TRIANGLE_FRUSTUM_CULLING);
                ImGui::Checkbox("Frustum Clipping", &EngineSetup::getInstance()->TRIANGLE_RENDER_CLIPPING);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Physics")) {
                ImGui::Separator();
                ImGui::Checkbox("Gravity", &EngineSetup::getInstance()->ENABLE_GRAVITY);
                ImGui::Checkbox("Fly", &EngineSetup::getInstance()->ENABLE_FLYING);
                ImGui::Checkbox("Friction", &EngineSetup::getInstance()->ENABLE_FRICTION);
                ImGui::Checkbox("Air", &EngineSetup::getInstance()->ENABLE_AIR_FRICTION);
                ImGui::Separator();
                ImGui::Checkbox("Collisions", &EngineSetup::getInstance()->BSP_COLLISIONS_ENABLED);
                ImGui::DragScalar("Radius", ImGuiDataType_Float,  &EngineSetup::getInstance()->PLAYER_SPHERE_RADIUS, range_sensibility,  &range_min_radius, &range_max_radius, "%f", 1.0f);

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Logging")) {
                ImGui::Checkbox("Show debug data", &EngineSetup::getInstance()->MESH_DEBUG_INFO);
                ImGui::Separator();
                ImGui::Checkbox("Capture BSP data", &EngineSetup::getInstance()->DEBUG_BSP_MODE);
                ImGui::Checkbox("Capture Mesh data", &EngineSetup::getInstance()->DEBUG_MESH_MODE);
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
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::Checkbox("Camera Inspector", &show_camera_info);
                ImGui::Checkbox("3D Objects Inspector", &show_window_inspector);
                ImGui::Checkbox("Ligths Inspector", &show_window_lights_inspector);
                ImGui::Checkbox("Log", &show_window_log);
                ImGui::Checkbox("Physics", &show_window_physics);
                ImGui::Separator();
                ImGui::Checkbox("Draw main Frustum", &EngineSetup::getInstance()->DRAW_FRUSTUM);
                ImGui::Checkbox("Draw Triangle normal", &EngineSetup::getInstance()->TRIANGLE_RENDER_NORMAL);
                ImGui::Separator();
                ImGui::Checkbox("Draw Object3D Axis", &EngineSetup::getInstance()->RENDER_OBJECTS_AXIS);
                ImGui::Checkbox("Draw Object3D Billboards", &EngineSetup::getInstance()->DRAW_OBJECT3D_BILLBOARD);
                ImGui::Separator();
                ImGui::Checkbox("Object3D Text Label", &EngineSetup::getInstance()->TEXT_ON_OBJECT3D);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Developers")) {
                ImGui::DragScalar("TESTING", ImGuiDataType_Float,  &EngineSetup::getInstance()->TESTING, range_sensibility,  &range_min_radius, &range_max_radius, "%f", 1.0f);
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
