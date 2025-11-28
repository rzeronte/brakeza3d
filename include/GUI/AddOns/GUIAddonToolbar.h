#ifndef BRAKEZA3D_GUIWIDGETTOOLBAR_H
#define BRAKEZA3D_GUIWIDGETTOOLBAR_H

#include "imgui.h"
#include "../../Components/ComponentsManager.h"

struct GUIAddonToolbar
{
    ImVec4 offColor;
    ImVec4 onColor;
    ImVec4 luaColor;
    ImVec4 playColor;

    explicit GUIAddonToolbar()
    :
        offColor(ImVec4(0.2f, 0.2f, 0.2f, 1.0f)), // Gris oscuro
        onColor(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)), // Gris claro
        luaColor(ImVec4(0.5f, 0.5f, 0.5f, 1.0f)),  // Gris intermedio
        playColor(ImVec4(0.3f, 0.9f, 0.3f, 1.0f))  // Gris intermedio
    {

    }

    void Draw() const
    {
        if (ImGui::Begin("MainToolBar")) {
            drawLUAStatusIcons();
            VerticalSeparator();
            drawGUIIcon();
            VerticalSeparator();
            drawLayoutIcons();
            VerticalSeparator();
            drawMouseOptionsIcons();
            VerticalSeparator();
            drawBulletOptionsIcons();
            VerticalSeparator();
            drawTransformationsToolsIcons();
            VerticalSeparator();
            drawRenderTriangleModes();
            VerticalSeparator();
            drawLightsOptions();
        }
        ImGui::End();
    }

    void drawButton(const char* iconName, bool isActive, const ImVec4& color, const std::function<void()>& onClick) const
    {
        ImGui::PushStyleColor(ImGuiCol_Button, isActive ? color : offColor);
        if (ImGui::ImageButton(FileSystemGUI::IconTag(iconName), ImVec2(24, 24))) {
            onClick();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
    }

    static void drawFixedColorButton(const char* iconName, const ImVec4& color, const std::function<void()>& onClick)
    {
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        if (ImGui::ImageButton(FileSystemGUI::IconTag(iconName), ImVec2(24, 24))) {
            onClick();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
    }

    void drawLayoutIcons() const
    {
        auto *window = ComponentsManager::get()->getComponentWindow();

        drawButton(IconsByGUI::LAYOUT_DEFAULT,
                   window->getImGuiConfig() == BrakezaSetup::ImGUIConfigs::DEFAULT,
                   onColor,
                   [&]() { window->setImGuiConfig(BrakezaSetup::ImGUIConfigs::DEFAULT); });
        ImGui::SetItemTooltip("Default layout");

        drawButton(IconsByGUI::LAYOUT_CODING,
                   window->getImGuiConfig() == BrakezaSetup::ImGUIConfigs::CODING,
                   onColor,
                   [&]() { window->setImGuiConfig(BrakezaSetup::ImGUIConfigs::CODING); });
        ImGui::SetItemTooltip("Developer layout");

        drawButton(IconsByGUI::LAYOUT_DESIGN,
                   window->getImGuiConfig() == BrakezaSetup::ImGUIConfigs::DESIGN,
                   onColor,
                   [&]() { window->setImGuiConfig(BrakezaSetup::ImGUIConfigs::DESIGN); });
        ImGui::SetItemTooltip("Designer layout");
    }

    void drawTransformationsToolsIcons() const
    {
        auto window = ComponentsManager::get()->getComponentWindow();
        auto operation = window->getGuiZmoOperation();

        ImVec4 translateColor = (operation == ImGuizmo::OPERATION::TRANSLATE) ? onColor : offColor;
        ImVec4 rotateColor = (operation == ImGuizmo::OPERATION::ROTATE) ? onColor : offColor;
        ImVec4 scaleColor = (operation == ImGuizmo::OPERATION::SCALE_X) ? onColor : offColor;

        ImGui::PushStyleColor(ImGuiCol_Button, translateColor);
        if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::TRANSLATE), ImVec2(24, 24))) {
            window->setGuiZmoOperation(ImGuizmo::OPERATION::TRANSLATE);
        }
        ImGui::SetItemTooltip("Translate selected item");
        ImGui::PopStyleColor();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, rotateColor);
        if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::ROTATE), ImVec2(24, 24))) {
            window->setGuiZmoOperation(ImGuizmo::OPERATION::ROTATE);
        }
        ImGui::SetItemTooltip("Rotate selected item");
        ImGui::PopStyleColor();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, scaleColor);
        if (ImGui::ImageButton(FileSystemGUI::IconTag(IconsByGUI::SCALE), ImVec2(24, 24))) {
            window->setGuiZmoOperation(ImGuizmo::OPERATION::SCALE_X);
        }
        ImGui::SetItemTooltip("Scale selected item");
        ImGui::PopStyleColor();
    }

    void drawBulletOptionsIcons() const
    {
        drawButton(IconsByGUI::GRAVITY,
                   BrakezaSetup::get()->ENABLE_BULLET_STEP_SIMULATION,
                   onColor,
                   [&]() { BrakezaSetup::get()->ENABLE_BULLET_STEP_SIMULATION = !BrakezaSetup::get()->ENABLE_BULLET_STEP_SIMULATION; });
        ImGui::SetItemTooltip("Enable/Disable physic world");

        drawButton(IconsByGUI::DRAW_COLLIDERS,
                   BrakezaSetup::get()->BULLET_DEBUG_MODE,
                   onColor,
                   [&]() {
                        BrakezaSetup::get()->BULLET_DEBUG_MODE = !BrakezaSetup::get()->BULLET_DEBUG_MODE;
                        ComponentsManager::get()->getComponentCollisions()->setEnableDebugMode(BrakezaSetup::get()->BULLET_DEBUG_MODE);
                   }
        );
        ImGui::SetItemTooltip("Draw collider AABB");
    }

    void drawMouseOptionsIcons() const
    {
        drawButton(IconsByGUI::CLICK,
                   BrakezaSetup::get()->MOUSE_CLICK_SELECT_OBJECT3D,
                   onColor,
                   [&]() { BrakezaSetup::get()->MOUSE_CLICK_SELECT_OBJECT3D = !BrakezaSetup::get()->MOUSE_CLICK_SELECT_OBJECT3D; });
        ImGui::SetItemTooltip("Enable/Disable item click selection");

        drawButton(IconsByGUI::MOUSE_LOOK,
                   BrakezaSetup::get()->MOUSE_LOOK,
                   onColor,
                   [&]() { BrakezaSetup::get()->MOUSE_LOOK = !BrakezaSetup::get()->MOUSE_LOOK; });
        ImGui::SetItemTooltip("Enable/Disable mouse look");
    }

    void drawLightsOptions() const
    {
        drawButton(IconsByGUI::LIGHT_SYSTEM,
                   BrakezaSetup::get()->ENABLE_LIGHTS,
                   onColor,
                   [&]() { BrakezaSetup::get()->ENABLE_LIGHTS = !BrakezaSetup::get()->ENABLE_LIGHTS; });
    }

    void drawLUAStatusIcons() const
    {
        auto scripting = ComponentsManager::get()->getComponentScripting();

        if (scripting->getStateLUAScripts() == BrakezaSetup::LuaStateScripts::LUA_STOP) {
            drawFixedColorButton(IconsByGUI::PLAY, luaColor, [&]() { scripting->playLUAScripts(); });
            ImGui::SetItemTooltip("Run scripts");
        } else {
            drawFixedColorButton(IconsByGUI::STOP, playColor, [&]() { scripting->stopLUAScripts(); });
            ImGui::SetItemTooltip("Stop scripts");
        }
        drawFixedColorButton(IconsByGUI::RELOAD, luaColor, [&]() { scripting->reloadLUAScripts(); });
        ImGui::SetItemTooltip("Reload scripts");

        drawFixedColorButton(IconsByGUI::REMOVE, luaColor, [&]() { SceneLoader::ClearScene(); });
        ImGui::SetItemTooltip("Clear scene");
    }

    void drawRenderTriangleModes() const
    {
        drawButton(IconsByGUI::RENDER_PIXELS,
                   BrakezaSetup::get()->TRIANGLE_MODE_PIXELS,
                   onColor,
                   [&]() { BrakezaSetup::get()->TRIANGLE_MODE_PIXELS = !BrakezaSetup::get()->TRIANGLE_MODE_PIXELS; });
        ImGui::SetItemTooltip("Pixels");

        drawButton(IconsByGUI::RENDER_SHADING,
                   BrakezaSetup::get()->TRIANGLE_MODE_SHADING,
                   onColor,
                   [&]() { BrakezaSetup::get()->TRIANGLE_MODE_SHADING = !BrakezaSetup::get()->TRIANGLE_MODE_SHADING; });
        ImGui::SetItemTooltip("Shading");

        drawButton(IconsByGUI::RENDER_WIRE,
                   BrakezaSetup::get()->TRIANGLE_MODE_WIREFRAME,
                   onColor,
                   [&]() { BrakezaSetup::get()->TRIANGLE_MODE_WIREFRAME = !BrakezaSetup::get()->TRIANGLE_MODE_WIREFRAME; });
        ImGui::SetItemTooltip("Wireframe");

        drawButton(IconsByGUI::RENDER_TEXTURE,
                   BrakezaSetup::get()->TRIANGLE_MODE_TEXTURIZED,
                   onColor,
                   [&]() { BrakezaSetup::get()->TRIANGLE_MODE_TEXTURIZED = !BrakezaSetup::get()->TRIANGLE_MODE_TEXTURIZED; });
        ImGui::SetItemTooltip("Diffuse");
    }

    void drawGUIIcon() const
    {
        drawButton(IconsByGUI::GUI,
                   BrakezaSetup::get()->ENABLE_IMGUI,
                   onColor,
                   [&]() { BrakezaSetup::get()->ENABLE_IMGUI = !BrakezaSetup::get()->ENABLE_IMGUI; });
        ImGui::SetItemTooltip("Enable/Disable UI");
    }

    static void VerticalSeparator()
    {
        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
        ImGui::Text("|");
        ImGui::SameLine();
    }
};

#endif //BRAKEZA3D_GUIWIDGETTOOLBAR_H
