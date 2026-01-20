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

    // Solo si está activada, configurar la ventana
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

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));

    if (ImGui::Begin("MainToolBar", nullptr, flags)) {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 6));
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
    }
    ImGui::End();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
}

void GUIAddonToolbar::LayoutIcons()
{
    auto *window = Components::get()->Window();

    GUI::DrawButton("Enable/Disable UI", IconGUI::TOOLBAR_ENABLE_GUI, GUIType::Sizes::ICONS_TOOLBAR, Config::get()->ENABLE_IMGUI,[]() {
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
    GUI::DrawButton("Enable/Disable physic world", IconGUI::TOOLBAR_ENABLE_STEP_SIMULATION, GUIType::Sizes::ICONS_TOOLBAR, Config::get()->ENABLE_BULLET_STEP_SIMULATION, [&]() {
        GUI::Toggle(Config::get()->ENABLE_BULLET_STEP_SIMULATION);
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

    GUI::DrawButton(label, icon, GUIType::Sizes::ICONS_TOOLBAR, isStop, [&]() { isStop ? callbackPlay() : callbackStop(); });
    ImGui::SameLine();
    GUI::DrawButton("Reload scripts", IconGUI::LUA_RELOAD, GUIType::Sizes::ICONS_TOOLBAR, false, [&]() { scripting->ReloadLUAScripts(); });
    ImGui::SameLine();
    GUI::DrawButton("Clear scene", IconGUI::LUA_REMOVE, GUIType::Sizes::ICONS_TOOLBAR, false, [&]() { SceneLoader::ClearScene(); });
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
    ImGui::SameLine();
    if (!Brakeza::get()->GUI()->getResourcesHub()->isAuthenticated()) {
        GUI::DrawButton("Assets Hub - Login", IconGUI::SESSION_OPEN, GUIType::Sizes::ICONS_TOOLBAR, false,[&]() {
            Brakeza::get()->GUI()->getResourcesHub()->showLogin();
        });
    } else {
       GUI::DrawButton("Assets Hub - Logout", IconGUI::SESSION_CLOSE, GUIType::Sizes::ICONS_TOOLBAR, false,[&]() {
            //Brakeza::get()->GUI()->getResourcesHub()->lo();
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
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
    ImGui::Text("|");
    ImGui::SameLine();
}