//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETTOOLBAR_H
#define BRAKEZA3D_GUIWIDGETTOOLBAR_H

#include "imgui.h"
#include "../ComponentsManager.h"

struct GUIWidgetToolbar {
    TexturePackage &ImGuiTextures;
    ImVec4 offColor;
    ImVec4 onColor;
    explicit GUIWidgetToolbar(TexturePackage &imGuiTextures)
        :
        ImGuiTextures(imGuiTextures),
        offColor(ImVec4(0.8f, 0.4f, 0.4f, 1.0f)),
        onColor(ImVec4(0.4f, 0.8f, 0.4f, 1.0f))
    {
    }

    void draw() const
    {
        if (ImGui::Begin("MainToolBar")) {
            auto render = ComponentsManager::get()->getComponentRender();

            drawLUAStatusIcons(render);
            GUIWidgetToolbar::VerticalSeparator();
            drawMouseOptionsIcons();
            GUIWidgetToolbar::VerticalSeparator();
            drawBulletOptionsIcons();

        }
        ImGui::End();
    }

    void drawBulletOptionsIcons() const
    {
        // BULLET ON/OF
        bool wasBulletStepSimulationEnabled = EngineSetup::get()->BULLET_STEP_SIMULATION;
        if (!wasBulletStepSimulationEnabled) {
            ImGui::PushStyleColor(ImGuiCol_Button, offColor);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, onColor);
        }
        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "gravityIcon"), ImVec2(24, 24))) {
            EngineSetup::get()->BULLET_STEP_SIMULATION = !EngineSetup::get()->BULLET_STEP_SIMULATION;
        }

        ImGui::PopStyleColor();

        ImGui::SameLine();

        // DEBUG DRAW ON/OF
        bool wasDrawDebugEnabled = EngineSetup::get()->BULLET_DEBUG_MODE;
        if (!wasDrawDebugEnabled) {
            ImGui::PushStyleColor(ImGuiCol_Button, offColor);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, onColor);
        }

        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "drawCollidersIcon"), ImVec2(24, 24))) {
            EngineSetup::get()->BULLET_DEBUG_MODE = !EngineSetup::get()->BULLET_DEBUG_MODE;
        }
        ImGui::PopStyleColor();

        ImGui::SameLine();
    }

    void drawMouseOptionsIcons() const
    {
        // CLICK OBJECTS TO SELECT ON/OFF
        ImGui::SameLine();
        bool wasClickSelectorEnabled = EngineSetup::get()->CLICK_SELECT_OBJECT3D;
        if (!wasClickSelectorEnabled) {
            ImGui::PushStyleColor(ImGuiCol_Button, offColor);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, onColor);
        }

        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "clickIcon"), ImVec2(24, 24))) {
            EngineSetup::get()->CLICK_SELECT_OBJECT3D = !EngineSetup::get()->CLICK_SELECT_OBJECT3D;
        }

        ImGui::PopStyleColor();

        ImGui::SameLine();

        // MOUSE LOOK
        ImGui::SameLine();
        bool wasMouseLookEnabled = EngineSetup::get()->MOUSE_LOOK;
        if (!wasMouseLookEnabled) {
            ImGui::PushStyleColor(ImGuiCol_Button, offColor);
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, onColor);
        }

        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "mouseLookIcon"), ImVec2(24, 24))) {
            EngineSetup::get()->MOUSE_LOOK = !EngineSetup::get()->MOUSE_LOOK;
        }

        ImGui::PopStyleColor();
    }

    void drawLUAStatusIcons(ComponentRender *render) const
    {
        if (render->getStateLUAScripts() == EngineSetup::LUA_STOP) {
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "playIcon"), ImVec2(24, 24))) {
                render->playLUAScripts();
            }
            ImGui::SameLine();
        } else {
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "stopIcon"), ImVec2(24, 24))) {
                render->stopLUAScripts();
            }
            ImGui::SameLine();
        }

        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "reloadIcon"), ImVec2(24, 24))) {
            render->reloadLUAScripts();
        }

        ImGui::SameLine();
        if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "removeIcon"), ImVec2(24, 24))) {
            SceneLoader::clearScene();
        }
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
