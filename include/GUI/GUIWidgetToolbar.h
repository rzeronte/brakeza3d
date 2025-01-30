#ifndef BRAKEZA3D_GUIWIDGETTOOLBAR_H
#define BRAKEZA3D_GUIWIDGETTOOLBAR_H

#include "imgui.h"
#include "../ComponentsManager.h"

struct GUIWidgetToolbar {
    TexturePackage &ImGuiTextures;
    ImVec4 offColor;
    ImVec4 onColor;
    ImVec4 luaColor;

    explicit GUIWidgetToolbar(TexturePackage &imGuiTextures)
:
    ImGuiTextures(imGuiTextures),
    offColor(ImVec4(0.2f, 0.2f, 0.2f, 1.0f)), // Gris oscuro
    onColor(ImVec4(0.7f, 0.7f, 0.7f, 1.0f)), // Gris claro
    luaColor(ImVec4(0.5f, 0.5f, 0.5f, 1.0f))  // Gris intermedio
{

}
    void draw() const
    {
        if (ImGui::Begin("MainToolBar")) {
            drawLUAStatusIcons();
            VerticalSeparator();
            drawLayoutIcons();
            VerticalSeparator();
            drawMouseOptionsIcons();
            VerticalSeparator();
            drawBulletOptionsIcons();
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
                   window->ImGuiConfigChanged == ImGUIConfigs::DEFAULT,
                   onColor,
                   [&]() { window->ImGuiConfigChanged = ImGUIConfigs::DEFAULT; });

        drawButton("layoutCodingIcon",
                   window->ImGuiConfigChanged == ImGUIConfigs::CODING,
                   onColor,
                   [&]() { window->ImGuiConfigChanged = ImGUIConfigs::CODING; });

        drawButton("layoutDesignIcon",
                   window->ImGuiConfigChanged == ImGUIConfigs::DESIGN,
                   onColor,
                   [&]() { window->ImGuiConfigChanged = ImGUIConfigs::DESIGN; });
    }

    void drawBulletOptionsIcons() const
    {
        drawButton("gravityIcon",
                   EngineSetup::get()->BULLET_STEP_SIMULATION,
                   onColor,
                   [&]() { EngineSetup::get()->BULLET_STEP_SIMULATION = !EngineSetup::get()->BULLET_STEP_SIMULATION; });

        drawButton("drawCollidersIcon",
                   EngineSetup::get()->BULLET_DEBUG_MODE,
                   onColor,
                   [&]() { EngineSetup::get()->BULLET_DEBUG_MODE = !EngineSetup::get()->BULLET_DEBUG_MODE; });
    }

    void drawMouseOptionsIcons() const
    {
        drawButton("clickIcon",
                   EngineSetup::get()->CLICK_SELECT_OBJECT3D,
                   onColor,
                   [&]() { EngineSetup::get()->CLICK_SELECT_OBJECT3D = !EngineSetup::get()->CLICK_SELECT_OBJECT3D; });

        drawButton("mouseLookIcon",
                   EngineSetup::get()->MOUSE_LOOK,
                   onColor,
                   [&]() { EngineSetup::get()->MOUSE_LOOK = !EngineSetup::get()->MOUSE_LOOK; });
    }

    void drawLUAStatusIcons() const
    {
        auto scripting = ComponentsManager::get()->getComponentScripting();

        if (scripting->getStateLUAScripts() == EngineSetup::LuaStateScripts::LUA_STOP) {
            drawFixedColorButton("playIcon", luaColor, [&]() { scripting->playLUAScripts(); });
        } else {
            drawFixedColorButton("stopIcon", luaColor, [&]() { scripting->stopLUAScripts(); });

        }
        drawFixedColorButton("reloadIcon", luaColor, [&]() { scripting->reloadLUAScripts(); });
        drawFixedColorButton("removeIcon", luaColor, [&]() { SceneLoader::clearScene(); });
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
