//
// Created by Eduardo on 03/12/2025.
//

#include "../../../include/GUI/AddOns/GUIAddonToolbar.h"

#include "../../../include/GUI/GUI.h"
#include "../../../include/Brakeza.h"
#include "../../../include/Config.h"
#include "../../../include/Components/ComponentsManager.h"


void GUIAddonToolbar::Draw()
{
    if (!Config::get()->ENABLE_IMGUI_TOOLBAR) return;

    if (ImGui::Begin("MainToolBar")) {
        LUAStatusIcons();
        VerticalSeparator();
        GUI::DrawButton("Enable/Disable UI", IconGUI::GUI, GUIType::Sizes::ICON_SIZE_MEDIUM, Config::get()->ENABLE_IMGUI,[]() {
            GUI::Toggle(Config::get()->ENABLE_IMGUI);
        });
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
    }
    ImGui::End();
}

void GUIAddonToolbar::LayoutIcons()
{
    auto *window = ComponentsManager::get()->getComponentWindow();

    GUI::DrawButton("Default Layout", IconGUI::LAYOUT_DEFAULT, GUIType::Sizes::ICON_SIZE_MEDIUM, window->getImGuiConfig() == Config::ImGUIConfigs::DEFAULT, [&]() {
        window->setImGuiConfig(Config::ImGUIConfigs::DEFAULT);
    });
    ImGui::SameLine();
    GUI::DrawButton("Developer Layout", IconGUI::LAYOUT_CODING, GUIType::Sizes::ICON_SIZE_MEDIUM, window->getImGuiConfig() == Config::ImGUIConfigs::CODING, [&]() {
        window->setImGuiConfig(Config::ImGUIConfigs::CODING);
    });
    ImGui::SameLine();
    GUI::DrawButton("Design Layout", IconGUI::LAYOUT_DESIGN, GUIType::Sizes::ICON_SIZE_MEDIUM, window->getImGuiConfig() == Config::ImGUIConfigs::DESIGN,[&]() {
        window->setImGuiConfig(Config::ImGUIConfigs::DESIGN);
    });
}

void GUIAddonToolbar::TransformationsToolsIcons()
{
    auto window = ComponentsManager::get()->getComponentWindow();
    auto operation = window->getGuiZmoOperation();

    GUI::DrawButton("Translate", IconGUI::TRANSLATE, GUIType::Sizes::ICON_SIZE_MEDIUM, operation == ImGuizmo::OPERATION::TRANSLATE, [&]() {
        window->setGuiZmoOperation(ImGuizmo::OPERATION::TRANSLATE);
    });
    ImGui::SameLine();
    GUI::DrawButton("Rotate", IconGUI::ROTATE, GUIType::Sizes::ICON_SIZE_MEDIUM, operation == ImGuizmo::OPERATION::ROTATE, [&]() {
        window->setGuiZmoOperation(ImGuizmo::OPERATION::ROTATE);
    });
    ImGui::SameLine();
    GUI::DrawButton("Scale", IconGUI::SCALE, GUIType::Sizes::ICON_SIZE_MEDIUM, operation == ImGuizmo::OPERATION::SCALE_X, [&]() {
        window->setGuiZmoOperation(ImGuizmo::OPERATION::SCALE_X);
    } );
}

void GUIAddonToolbar::StepSimulationOptionsIcons()
{
    GUI::DrawButton("Enable/Disable physic world", IconGUI::GRAVITY, GUIType::Sizes::ICON_SIZE_MEDIUM, Config::get()->ENABLE_BULLET_STEP_SIMULATION, [&]() {
        GUI::Toggle(Config::get()->ENABLE_BULLET_STEP_SIMULATION);
    } );
    ImGui::SameLine();
    GUI::DrawButton( "Draw collider AABB", IconGUI::DRAW_COLLIDERS, GUIType::Sizes::ICON_SIZE_MEDIUM, Config::get()->BULLET_DEBUG_MODE, [&]() {
        GUI::Toggle(Config::get()->BULLET_DEBUG_MODE);
        ComponentsManager::get()->getComponentCollisions()->setEnableDebugMode(Config::get()->BULLET_DEBUG_MODE);
    });
}

void GUIAddonToolbar::MouseOptionsIcons()
{
    GUI::DrawButton("Enable/Disable item click selection", IconGUI::CLICK, GUIType::Sizes::ICON_SIZE_MEDIUM, Config::get()->MOUSE_CLICK_SELECT_OBJECT3D, [&]() {
        GUI::Toggle(Config::get()->MOUSE_CLICK_SELECT_OBJECT3D);
    });
    ImGui::SameLine();
    GUI::DrawButton("Enable/Disable mouse look", IconGUI::MOUSE_LOOK, GUIType::Sizes::ICON_SIZE_MEDIUM, Config::get()->MOUSE_LOOK,[&]() {
        GUI::Toggle(Config::get()->MOUSE_LOOK);
    });
}

void GUIAddonToolbar::LightsOptions()
{
    GUI::DrawButton("Enable/Disable mouse look", IconGUI::LIGHT_SYSTEM, GUIType::Sizes::ICON_SIZE_MEDIUM, Config::get()->ENABLE_LIGHTS,
       [&]() { GUI::Toggle(Config::get()->ENABLE_LIGHTS); }
    );

    if (Config::get()->ENABLE_LIGHTS) {
        ImGui::SameLine();
        GUI::DrawButton("Enable/Disable Light System", IconGUI::SHADOW_MAPPING, GUIType::Sizes::ICON_SIZE_MEDIUM, Config::get()->ENABLE_SHADOW_MAPPING,[&]() {
            GUI::Toggle(Config::get()->ENABLE_SHADOW_MAPPING);
        });
    }
}

void GUIAddonToolbar::LUAStatusIcons()
{
    auto scripting = ComponentsManager::get()->getComponentScripting();
    bool isStop = scripting->getStateLUAScripts() == Config::LuaStateScripts::LUA_STOP;
    auto icon = isStop ? IconGUI::STOP : IconGUI::PLAY;
    auto label = isStop ? "Stop" : "Play";
    auto callbackStop = [&]() { scripting->StopLUAScripts(); };
    auto callbackPlay = [&]() { scripting->PlayLUAScripts(); };

    GUI::DrawButton(label, icon, GUIType::Sizes::ICON_SIZE_MEDIUM, isStop, [&]() { isStop ? callbackPlay() : callbackStop(); });
    ImGui::SameLine();
    GUI::DrawButton("Reload scripts", IconGUI::RELOAD, GUIType::Sizes::ICON_SIZE_MEDIUM, false, [&]() { scripting->ReloadLUAScripts(); });
    ImGui::SameLine();
    GUI::DrawButton("Clear scene", IconGUI::REMOVE, GUIType::Sizes::ICON_SIZE_MEDIUM, false, [&]() { SceneLoader::ClearScene(); });
}

void GUIAddonToolbar::RenderTriangleModes()
{
    GUI::DrawButton("Pixels", IconGUI::RENDER_PIXELS, GUIType::Sizes::ICON_SIZE_MEDIUM,Config::get()->TRIANGLE_MODE_PIXELS,[&]() {
        GUI::Toggle(Config::get()->TRIANGLE_MODE_PIXELS);
    });
    ImGui::SameLine();
    GUI::DrawButton("Shading", IconGUI::RENDER_SHADING, GUIType::Sizes::ICON_SIZE_MEDIUM,Config::get()->TRIANGLE_MODE_SHADING, [&]() {
        GUI::Toggle(Config::get()->TRIANGLE_MODE_SHADING);
    });
    ImGui::SameLine();
    GUI::DrawButton("Wireframe", IconGUI::RENDER_WIRE, GUIType::Sizes::ICON_SIZE_MEDIUM,Config::get()->TRIANGLE_MODE_WIREFRAME, [&]() {
        GUI::Toggle(Config::get()->TRIANGLE_MODE_WIREFRAME);
    });
    ImGui::SameLine();
    GUI::DrawButton("Diffuse", IconGUI::RENDER_TEXTURE, GUIType::Sizes::ICON_SIZE_MEDIUM,Config::get()->TRIANGLE_MODE_TEXTURIZED, [&]() {
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