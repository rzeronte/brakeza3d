#ifndef BRAKEZA3D_GUIWIDGETTOOLBAR_H
#define BRAKEZA3D_GUIWIDGETTOOLBAR_H

#include "imgui.h"
#include "../ComponentsManager.h"

struct GUIAddonToolbar {
    TexturePackage &ImGuiTextures;
    ImVec4 offColor;
    ImVec4 onColor;
    ImVec4 luaColor;
    ImVec4 playColor;

    explicit GUIAddonToolbar(TexturePackage &imGuiTextures)
:
    ImGuiTextures(imGuiTextures),
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
        }
        ImGui::End();
    }

    void drawButton(const char* iconName, bool isActive, const ImVec4& color, const std::function<void()>& onClick) const
    {
        ImGui::PushStyleColor(ImGuiCol_Button, isActive ? color : offColor);
        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, iconName), ImVec2(24, 24))) {
            onClick();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
    }

    void drawFixedColorButton(const char* iconName, const ImVec4& color, const std::function<void()>& onClick) const
    {
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, iconName), ImVec2(24, 24))) {
            onClick();
        }
        ImGui::PopStyleColor();
        ImGui::SameLine();
    }

    void drawLayoutIcons() const
    {
        auto *window = ComponentsManager::get()->getComponentWindow();

        drawButton("layoutDefaultIcon",
                   window->getImGuiConfig() == EngineSetup::ImGUIConfigs::DEFAULT,
                   onColor,
                   [&]() { window->setImGuiConfig(EngineSetup::ImGUIConfigs::DEFAULT); });
        ImGui::SetItemTooltip("Default layout");

        drawButton("layoutCodingIcon",
                   window->getImGuiConfig() == EngineSetup::ImGUIConfigs::CODING,
                   onColor,
                   [&]() { window->setImGuiConfig(EngineSetup::ImGUIConfigs::CODING); });
        ImGui::SetItemTooltip("Developer layout");

        drawButton("layoutDesignIcon",
                   window->getImGuiConfig() == EngineSetup::ImGUIConfigs::DESIGN,
                   onColor,
                   [&]() { window->setImGuiConfig(EngineSetup::ImGUIConfigs::DESIGN); });
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
        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "translateIcon"), ImVec2(24, 24))) {
            window->setGuiZmoOperation(ImGuizmo::OPERATION::TRANSLATE);
        }
        ImGui::SetItemTooltip("Translate selected item");
        ImGui::PopStyleColor();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, rotateColor);
        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "rotateIcon"), ImVec2(24, 24))) {
            window->setGuiZmoOperation(ImGuizmo::OPERATION::ROTATE);
        }
        ImGui::SetItemTooltip("Rotate selected item");
        ImGui::PopStyleColor();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, scaleColor);
        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "scaleIcon"), ImVec2(24, 24))) {
            window->setGuiZmoOperation(ImGuizmo::OPERATION::SCALE_X);
        }
        ImGui::SetItemTooltip("Scale selected item");
        ImGui::PopStyleColor();
    }

    void drawBulletOptionsIcons() const
    {
        drawButton("gravityIcon",
                   EngineSetup::get()->ENABLE_BULLET_STEP_SIMULATION,
                   onColor,
                   [&]() { EngineSetup::get()->ENABLE_BULLET_STEP_SIMULATION = !EngineSetup::get()->ENABLE_BULLET_STEP_SIMULATION; });
        ImGui::SetItemTooltip("Enable/Disable physic world");

        drawButton("drawCollidersIcon",
                   EngineSetup::get()->BULLET_DEBUG_MODE,
                   onColor,
                   [&]() {
                        EngineSetup::get()->BULLET_DEBUG_MODE = !EngineSetup::get()->BULLET_DEBUG_MODE;
                        ComponentsManager::get()->getComponentCollisions()->setEnableDebugMode(EngineSetup::get()->BULLET_DEBUG_MODE);
                   }
        );
        ImGui::SetItemTooltip("Draw collider AABB");
    }

    void drawMouseOptionsIcons() const
    {
        drawButton("clickIcon",
                   EngineSetup::get()->MOUSE_CLICK_SELECT_OBJECT3D,
                   onColor,
                   [&]() { EngineSetup::get()->MOUSE_CLICK_SELECT_OBJECT3D = !EngineSetup::get()->MOUSE_CLICK_SELECT_OBJECT3D; });
        ImGui::SetItemTooltip("Enable/Disable item click selection");

        drawButton("mouseLookIcon",
                   EngineSetup::get()->MOUSE_LOOK,
                   onColor,
                   [&]() { EngineSetup::get()->MOUSE_LOOK = !EngineSetup::get()->MOUSE_LOOK; });
        ImGui::SetItemTooltip("Enable/Disable mouse look");
    }

    void drawLUAStatusIcons() const
    {
        auto scripting = ComponentsManager::get()->getComponentScripting();

        if (scripting->getStateLUAScripts() == EngineSetup::LuaStateScripts::LUA_STOP) {
            drawFixedColorButton("playIcon", luaColor, [&]() { scripting->playLUAScripts(); });
            ImGui::SetItemTooltip("Run scripts");
        } else {
            drawFixedColorButton("stopIcon", playColor, [&]() { scripting->stopLUAScripts(); });
            ImGui::SetItemTooltip("Stop scripts");
        }
        drawFixedColorButton("reloadIcon", luaColor, [&]() { scripting->reloadLUAScripts(); });
        ImGui::SetItemTooltip("Reload scripts");

        drawFixedColorButton("removeIcon", luaColor, [&]() { SceneLoader::clearScene(); });
        ImGui::SetItemTooltip("Clear scene");
    }

    void drawGUIIcon() const
    {
        drawButton("guiIcon",
                   EngineSetup::get()->ENABLE_IMGUI,
                   onColor,
                   [&]() { EngineSetup::get()->ENABLE_IMGUI = !EngineSetup::get()->ENABLE_IMGUI; });
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
