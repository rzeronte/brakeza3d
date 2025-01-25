//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETTOOLBAR_H
#define BRAKEZA3D_GUIWIDGETTOOLBAR_H

#include "imgui.h"
#include "../ComponentsManager.h"

struct GUIWidgetToolbar {
    TexturePackage &ImGuiTextures;

    explicit GUIWidgetToolbar(TexturePackage &imGuiTextures)
        :
        ImGuiTextures(imGuiTextures)
    {

    }

    void draw()
    {
        if (ImGui::Begin("MainToolBar")) {
            auto render = ComponentsManager::get()->getComponentRender();

            if (render->getStateLUAScripts() == EngineSetup::LuaStateScripts::LUA_STOP) {
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

            ImGui::SameLine();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
            ImGui::Text("|");
            ImGui::SameLine();

            // CLICK OBJECTS TO SELECT ON/OFF
            ImGui::SameLine();
            bool wasClickSelectorEnabled = EngineSetup::get()->CLICK_SELECT_OBJECT3D;
            if (!wasClickSelectorEnabled) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            }
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "clickIcon"), ImVec2(24, 24))) {
                EngineSetup::get()->CLICK_SELECT_OBJECT3D = !EngineSetup::get()->CLICK_SELECT_OBJECT3D;
            }
            if (!wasClickSelectorEnabled) {
                ImGui::PopStyleColor();
            }

            // FREE LOOKON/OFF
            ImGui::SameLine();

            bool wasFreeLookEnabled = ComponentsManager::get()->getComponentCamera()->freeLook;
            if (!wasFreeLookEnabled) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            }
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "freelookIcon"), ImVec2(24, 24))) {
                ComponentsManager::get()->getComponentCamera()->freeLook = !ComponentsManager::get()->getComponentCamera()->freeLook;
            }
            if (!wasFreeLookEnabled) {
                ImGui::PopStyleColor();
            }

            ImGui::SameLine();
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10.0f);
            ImGui::Text("|");
            ImGui::SameLine();


            ImGui::SameLine();
            // BULLET ON/OF
            bool wasBulletStepSimulationEnabled = EngineSetup::get()->BULLET_STEP_SIMULATION;
            if (!wasBulletStepSimulationEnabled) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            }
            if (ImGui::ImageButton(TexturePackage::getOGLTextureID(ImGuiTextures, "gravityIcon"), ImVec2(24, 24))) {
                EngineSetup::get()->BULLET_STEP_SIMULATION = !EngineSetup::get()->BULLET_STEP_SIMULATION;
            }
            if (!wasBulletStepSimulationEnabled) {
                ImGui::PopStyleColor();
            }

        }
        ImGui::End();
    }
};
#endif //BRAKEZA3D_GUIWIDGETTOOLBAR_H
