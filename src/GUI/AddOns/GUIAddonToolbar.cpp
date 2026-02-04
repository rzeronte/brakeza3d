//
// Created by Eduardo on 03/12/2025.
//

#include "../../../include/GUI/AddOns/GUIAddonToolbar.h"
#include "../../../include/GUI/GUI.h"
#include "../../../include/Brakeza.h"
#include "../../../include/Config.h"
#include "../../../include/Components/Components.h"

void GUIAddonToolbar::Draw()
{
    if (!Config::get()->ENABLE_IMGUI_TOOLBAR) return;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float toolbar_height = Config::get()->TOOL_BAR_HEIGHT;
    const float menu_bar_height = ImGui::GetFrameHeight();

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + menu_bar_height));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, toolbar_height));
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoDocking;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(4, 4));

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    if (ImGui::Begin("MainToolBar", nullptr, flags)) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 6));
        ImGui::Dummy(ImVec2(14.0f, 0.0f));
        ImGui::SameLine();
        LUAStatusIcons();
        VerticalSeparator();
        LayoutIcons();
        VerticalSeparator();
        MouseOptionsIcons();
        VerticalSeparator();
        StepSimulationOptionsIcons();
        VerticalSeparator();
        TransformationsToolsIcons();
        VerticalSeparator();
        RenderTriangleModes();
        VerticalSeparator();
        LightsOptions();
        VerticalSeparator();
        Helpers();
        ImGui::PopStyleVar();
        DrawBaseLine();
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(4);
}

void GUIAddonToolbar::LayoutIcons()
{
    auto *window = Components::get()->Window();

    GUI::DrawButton("Enable/Disable UI", IconGUI::TOOLBAR_ENABLE_GUI, GUIType::Sizes::ICONS_TOOLBAR, false,[]() {
        GUI::Toggle(Config::get()->ENABLE_IMGUI);
    });
    ImGui::SameLine();
    GUI::DrawButton("Default Layout", IconGUI::TOOLBAR_LAYOUT_DEFAULT, GUIType::Sizes::ICONS_TOOLBAR, window->getImGuiConfig() == Config::ImGUIConfigs::DEFAULT, [&]() {
        window->setImGuiConfig(Config::ImGUIConfigs::DEFAULT);
    });
    ImGui::SameLine();
    GUI::DrawButton("Developer Layout", IconGUI::TOOLBAR_LAYOUT_CODING, GUIType::Sizes::ICONS_TOOLBAR, window->getImGuiConfig() == Config::ImGUIConfigs::CODING, [&]() {
        window->setImGuiConfig(Config::ImGUIConfigs::CODING);
    });
    ImGui::SameLine();
    GUI::DrawButton("Design Layout", IconGUI::TOOLBAR_LAYOUT_DESIGN, GUIType::Sizes::ICONS_TOOLBAR, window->getImGuiConfig() == Config::ImGUIConfigs::DESIGN,[&]() {
        window->setImGuiConfig(Config::ImGUIConfigs::DESIGN);
    });
}

void GUIAddonToolbar::TransformationsToolsIcons()
{
    auto window = Components::get()->Window();
    auto operation = window->getGuiZmoOperation();

    GUI::DrawButton("Translate", IconGUI::TOOLBAR_TRANSLATE, GUIType::Sizes::ICONS_TOOLBAR, operation == ImGuizmo::OPERATION::TRANSLATE, [&]() {
        window->setGuiZmoOperation(ImGuizmo::OPERATION::TRANSLATE);
    });
    ImGui::SameLine();
    GUI::DrawButton("Rotate", IconGUI::TOOLBAR_ROTATE, GUIType::Sizes::ICONS_TOOLBAR, operation == ImGuizmo::OPERATION::ROTATE, [&]() {
        window->setGuiZmoOperation(ImGuizmo::OPERATION::ROTATE);
    });
    ImGui::SameLine();
    GUI::DrawButton("Scale", IconGUI::TOOLBAR_SCALE, GUIType::Sizes::ICONS_TOOLBAR, operation == ImGuizmo::OPERATION::SCALE_X, [&]() {
        window->setGuiZmoOperation(ImGuizmo::OPERATION::SCALE_X);
    } );
}

void GUIAddonToolbar::StepSimulationOptionsIcons()
{
    bool isPhysicWorld = Components::get()->Collisions()->isEnabled();
    GUI::DrawButton("Enable/Disable physic world", IconGUI::TOOLBAR_ENABLE_STEP_SIMULATION, GUIType::Sizes::ICONS_TOOLBAR, isPhysicWorld, [&]() {
        Components::get()->Collisions()->setEnabled(!isPhysicWorld);
    } );
    ImGui::SameLine();
    GUI::DrawButton( "Draw collider AABB", IconGUI::TOOLBAR_DRAW_COLLIDERS, GUIType::Sizes::ICONS_TOOLBAR, Config::get()->BULLET_DEBUG_MODE, [&]() {
        GUI::Toggle(Config::get()->BULLET_DEBUG_MODE);
        Components::get()->Collisions()->setEnableDebugMode(Config::get()->BULLET_DEBUG_MODE);
    });
}

void GUIAddonToolbar::MouseOptionsIcons()
{
    GUI::DrawButton("Enable/Disable item click selection", IconGUI::TOOLBAR_CLICK_OBJECTS, GUIType::Sizes::ICONS_TOOLBAR, Config::get()->MOUSE_CLICK_SELECT_OBJECT3D, [&]() {
        GUI::Toggle(Config::get()->MOUSE_CLICK_SELECT_OBJECT3D);
    });
    ImGui::SameLine();
    GUI::DrawButton("Enable/Disable mouse look", IconGUI::TOOLBAR_MOUSE_LOOK, GUIType::Sizes::ICONS_TOOLBAR, Config::get()->MOUSE_LOOK,[&]() {
        GUI::Toggle(Config::get()->MOUSE_LOOK);
    });
}

void GUIAddonToolbar::LightsOptions()
{
    GUI::DrawButton("Enable/Disable lights system", IconGUI::TOOLBAR_ENABLE_LIGHT_SYSTEM, GUIType::Sizes::ICONS_TOOLBAR, Config::get()->ENABLE_LIGHTS,
       [&]() { GUI::Toggle(Config::get()->ENABLE_LIGHTS); }
    );

    if (Config::get()->ENABLE_LIGHTS) {
        ImGui::SameLine();
        GUI::DrawButton("Enable/Disable shadow mapping", IconGUI::TOOLBAR_ENABLE_SHADOW_MAPPING, GUIType::Sizes::ICONS_TOOLBAR, Config::get()->ENABLE_SHADOW_MAPPING,[&]() {
            GUI::Toggle(Config::get()->ENABLE_SHADOW_MAPPING);
        });
    }
}

void GUIAddonToolbar::LUAStatusIcons()
{
    auto scripting = Components::get()->Scripting();
    bool isStop = scripting->getStateLUAScripts() == Config::LuaStateScripts::LUA_STOP;
    auto icon = isStop ? IconGUI::LUA_PLAY : IconGUI::LUA_STOP;
    auto label = isStop ? "Play" : "Stop";
    auto callbackStop = [&]() { scripting->StopLUAScripts(); };
    auto callbackPlay = [&]() { scripting->PlayLUAScripts(); };

    GUI::DrawButton(label, icon, GUIType::Sizes::ICONS_TOOLBAR, isStop, [&]() {
        isStop ? callbackPlay() : callbackStop();
    });
    ImGui::SameLine();
    GUI::DrawButton("Reload scripts", IconGUI::LUA_RELOAD, GUIType::Sizes::ICONS_TOOLBAR, false, [&]() {
        scripting->ReloadLUAScripts();
    });
    VerticalSeparator();
    ImGui::SameLine();
    GUI::DrawButton("Clear scene", IconGUI::CLEAR_SCENE, GUIType::Sizes::ICONS_TOOLBAR, false, [&]() {
        SceneLoader::ClearScene();
    });
    ImGui::SameLine();
    GUI::DrawButton("Clean scene", IconGUI::CLEAN_SCENE, GUIType::Sizes::ICONS_TOOLBAR, false, [&]() {
        SceneLoader::CleanScene();
    });

}

void GUIAddonToolbar::Helpers()
{
    GUI::DrawButton("Grid background", IconGUI::TOOLBAR_GRID_BACKGROUND, GUIType::Sizes::ICONS_TOOLBAR, Config::get()->ENABLE_GRID_BACKGROUND,[&]() {
        GUI::Toggle(Config::get()->ENABLE_GRID_BACKGROUND);
    });
    ImGui::SameLine();
    GUI::DrawButton("Show picking colors", IconGUI::TOOLBAR_PICKING_COLORS, GUIType::Sizes::ICONS_TOOLBAR, Config::get()->TRIANGLE_MODE_PICKING_COLORS,[&]() {
        GUI::Toggle(Config::get()->TRIANGLE_MODE_PICKING_COLORS);
    });
    ImGui::SameLine();
    GUI::DrawButton("Screenshot", IconGUI::TOOLBAR_TAKE_SCREENSHOT, GUIType::Sizes::ICONS_TOOLBAR, Config::get()->TRIANGLE_MODE_PICKING_COLORS,[&]() {
        ComponentRender::MakeScreenShot();
    });
    VerticalSeparator();
    ImGui::SameLine();
    GUI::DrawButton("Documentation", IconGUI::WIN_DOCUMENTATION, GUIType::Sizes::ICONS_TOOLBAR, Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::DOCUMENTATION)->isOpen,[&]() {
        Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::DOCUMENTATION)->isOpen = !Brakeza::get()->GUI()->getWindowStatus(GUIType::Window::DOCUMENTATION)->isOpen;
    });
    VerticalSeparator();
    ImGui::SameLine();
    if (!Brakeza::get()->GUI()->getResourcesHub()->isAuthenticated()) {
        GUI::DrawButton("Assets Hub - Login", IconGUI::SESSION_OPEN, GUIType::Sizes::ICONS_TOOLBAR, false,[&]() {
            Brakeza::get()->GUI()->getResourcesHub()->showLogin();
        });
    } else {
        GUI::DrawButton("Assets Hub - Browser", IconGUI::MNU_RESOURCESHUB, GUIType::Sizes::ICONS_TOOLBAR, false,[&]() {
             Brakeza::get()->GUI()->getResourcesHub()->showBrowser();
        });
        ImGui::SameLine();
        GUI::DrawButton("Assets Hub - Logout", IconGUI::SESSION_CLOSE, GUIType::Sizes::ICONS_TOOLBAR, false,[&]() {
            Brakeza::get()->GUI()->getResourcesHub()->performLogout();
       });
    }
}

void GUIAddonToolbar::RenderTriangleModes()
{
    GUI::DrawButton("Pixels", IconGUI::RENDER_PIXELS, GUIType::Sizes::ICONS_TOOLBAR,Config::get()->TRIANGLE_MODE_PIXELS,[&]() {
        GUI::Toggle(Config::get()->TRIANGLE_MODE_PIXELS);
    });
    ImGui::SameLine();
    GUI::DrawButton("Shading", IconGUI::RENDER_SHADING, GUIType::Sizes::ICONS_TOOLBAR,Config::get()->TRIANGLE_MODE_SHADING, [&]() {
        GUI::Toggle(Config::get()->TRIANGLE_MODE_SHADING);
    });
    ImGui::SameLine();
    GUI::DrawButton("Wireframe", IconGUI::RENDER_WIRE, GUIType::Sizes::ICONS_TOOLBAR,Config::get()->TRIANGLE_MODE_WIREFRAME, [&]() {
        GUI::Toggle(Config::get()->TRIANGLE_MODE_WIREFRAME);
    });
    ImGui::SameLine();
    GUI::DrawButton("Diffuse", IconGUI::RENDER_TEXTURE, GUIType::Sizes::ICONS_TOOLBAR,Config::get()->TRIANGLE_MODE_TEXTURIZED, [&]() {
        GUI::Toggle(Config::get()->TRIANGLE_MODE_TEXTURIZED);
    });
}

void GUIAddonToolbar::VerticalSeparator()
{
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(10.0f, 0.0f));
    ImGui::SameLine();
}

void GUIAddonToolbar::DrawSeparatorLine()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    const float line_height = 2.0f;
    const float toolbar_height = Config::get()->TOOL_BAR_HEIGHT;
    const float menu_bar_height = ImGui::GetFrameHeight();

    // Posicionar justo debajo del toolbar
    float y_pos = viewport->Pos.y + menu_bar_height + toolbar_height;

    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, y_pos));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, line_height));
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoSavedSettings |
                             ImGuiWindowFlags_NoDocking |
                             ImGuiWindowFlags_NoInputs |
                             ImGuiWindowFlags_NoFocusOnAppearing |
                             ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

    // Color de fondo de la línea (ajusta según tu gradiente)
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));

    if (ImGui::Begin("ToolbarSeparatorLine", nullptr, flags)) {
        // Aquí puedes dibujar tu gradiente si quieres
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 win_pos = ImGui::GetWindowPos();
        ImVec2 win_size = ImGui::GetWindowSize();

        // Ejemplo de gradiente horizontal
        draw_list->AddRectFilledMultiColor(
            win_pos,
            ImVec2(win_pos.x + win_size.x, win_pos.y + win_size.y),
            IM_COL32(50, 50, 50, 255),   // Izquierda
            IM_COL32(100, 100, 100, 255), // Derecha
            IM_COL32(100, 100, 100, 255), // Abajo derecha
            IM_COL32(50, 50, 50, 255)     // Abajo izquierda
        );
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
}

void GUIAddonToolbar::DrawBaseLine() {
    // DEGRADADO LIME → CYAN → LIME
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 win_pos = ImGui::GetWindowPos();
    ImVec2 win_size = ImGui::GetWindowSize();

    const float lineHeight = 2.0f;
    const float yPosition = win_pos.y + win_size.y - lineHeight;

    // Colores lime y cyan
    const ImU32 colorLime = IM_COL32(132, 204, 22, 255);   // lime-500
    const ImU32 colorCyan = IM_COL32(6, 182, 212, 255);    // cyan-500

    const float width = win_size.x;
    const float segment = width / 3.0f;

    // Segmento izquierdo: lime → cyan
    drawList->AddRectFilledMultiColor(
        ImVec2(win_pos.x, yPosition),
        ImVec2(win_pos.x + segment, yPosition + lineHeight),
        colorLime, colorCyan, colorCyan, colorLime
    );

    // Segmento central: cyan puro
    drawList->AddRectFilled(
        ImVec2(win_pos.x + segment, yPosition),
        ImVec2(win_pos.x + segment * 2.0f, yPosition + lineHeight),
        colorCyan
    );

    // Segmento derecho: cyan → lime
    drawList->AddRectFilledMultiColor(
        ImVec2(win_pos.x + segment * 2.0f, yPosition),
        ImVec2(win_pos.x + win_size.x, yPosition + lineHeight),
        colorCyan, colorLime, colorLime, colorCyan
    );
}
