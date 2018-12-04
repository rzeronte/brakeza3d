
#ifndef SDL2_3D_ENGINE_GUI_MENU_H
#define SDL2_3D_ENGINE_GUI_MENU_H


#include "GUI.h"
#include "../../imgui/imgui.h"

class GUI_Menu : public GUI  {
public:

    virtual ~GUI_Menu() {}

    virtual void draw(bool &done, bool &show_window_inspector, bool &show_window_lights_inspector, bool &show_window_log) {

        bool show_about_window = false;

        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Brakeza")) {
                if (ImGui::MenuItem("About Brakeza", "CTRL+I")) show_about_window = true;
                ImGui::Separator();
                if (ImGui::MenuItem("Exit", "CTRL+W")) done = true;
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Render")) {
                ImGui::Checkbox("WireFrame", &EngineSetup::getInstance()->TRIANGLE_MODE_WIREFRAME);
                ImGui::Checkbox("Textures", &EngineSetup::getInstance()->TRIANGLE_MODE_TEXTURIZED);
                ImGui::Checkbox("Solid", &EngineSetup::getInstance()->TRIANGLE_MODE_COLOR_SOLID);
                ImGui::Checkbox("Vertex", &EngineSetup::getInstance()->TRIANGLE_MODE_PIXELS);
                ImGui::Separator();
                ImGui::Checkbox("Draw extra line", &EngineSetup::getInstance()->TRIANGLE_DEMO_EXTRALINE_ENABLED);
                ImGui::Separator();
                ImGui::Checkbox("Triangle Face-culling", &EngineSetup::getInstance()->TRIANGLE_FACECULLING);
                if (EngineSetup::getInstance()->TRIANGLE_FACECULLING) {
                    ImGui::Checkbox("Show BFC triangles", &EngineSetup::getInstance()->SHOW_WIREFRAME_FOR_BFC_HIDDEN_TRIANGLES);
                }
                ImGui::Separator();
                ImGui::Checkbox("Depth Buffer", &EngineSetup::getInstance()->TRIANGLE_RENDER_DEPTH_BUFFER);
                ImGui::Checkbox("Frustum Culling", &EngineSetup::getInstance()->TRIANGLE_FRUSTUM_CULLING);
                ImGui::Checkbox("Frustum Clipping", &EngineSetup::getInstance()->TRIANGLE_RENDER_CLIPPING);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View")) {
                ImGui::Checkbox("3D Objects Inspector", &show_window_inspector);
                ImGui::Checkbox("Ligths Inspector", &show_window_lights_inspector);
                ImGui::Checkbox("Log", &show_window_log);
                ImGui::Separator();
                ImGui::Checkbox("Draw main Frustum", &EngineSetup::getInstance()->DRAW_FRUSTUM);
                ImGui::Checkbox("Draw Triangle normal", &EngineSetup::getInstance()->TRIANGLE_RENDER_NORMAL);
                ImGui::Separator();
                ImGui::Checkbox("Draw Object3D Axis", &EngineSetup::getInstance()->RENDER_OBJECTS_AXIS);
                ImGui::Checkbox("Draw Object3D Billboards", &EngineSetup::getInstance()->DRAW_OBJECT3D_BILLBOARD);
                ImGui::Separator();
                ImGui::Checkbox("Object3D Text Label", &EngineSetup::getInstance()->TEXT_ON_OBJECT3D);
                ImGui::Separator();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Logging")) {
                ImGui::Checkbox("Show debug data", &EngineSetup::getInstance()->MESH_DEBUG_INFO);
                ImGui::Separator();
                ImGui::Checkbox("Capture BSP data", &EngineSetup::getInstance()->DEBUG_BSP_MODE);
                ImGui::Checkbox("Capture Mesh data", &EngineSetup::getInstance()->DEBUG_MESH_MODE);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("BSP")) {
                ImGui::Checkbox("Show Map Faces", &EngineSetup::getInstance()->Q1MAP_FACES);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Lights")) {
                ImGui::Checkbox("Light System", &EngineSetup::getInstance()->ENABLE_LIGHTS);
                if (EngineSetup::getInstance()->ENABLE_LIGHTS) {
                    ImGui::Checkbox("Shadow Casting", &EngineSetup::getInstance()->ENABLE_SHADOW_CASTING);
                }
                ImGui::Checkbox("Draw Lights Billboards", &EngineSetup::getInstance()->DRAW_LIGHTPOINTS_BILLBOARD);
                ImGui::Checkbox("Draw Lights Axis", &EngineSetup::getInstance()->DRAW_LIGHTPOINTS_AXIS);
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        if (show_about_window) ImGui::OpenPopup("New");

        if (ImGui::BeginPopup("New"))  {
            ImGui::Text("Brakeza v.0.1");
            ImGui::Text("Eduardo Rodríguez <eduardo@brakeza.com>");
            ImGui::Text("https://brakeza.com");
            ImGui::EndPopup();
        }
    }
};


#endif //SDL2_3D_ENGINE_GUI_MENU_H
