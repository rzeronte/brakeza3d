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
            if (ComponentsManager::get()->getComponentRender()->getStateLUAScripts() == EngineSetup::LuaStateScripts::LUA_STOP) {
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("playIcon")->getOGLTextureID(), ImVec2(24, 24))) {
                    ComponentsManager::get()->getComponentRender()->playLUAScripts();
                }
                ImGui::SameLine();
            } else {
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("stopIcon")->getOGLTextureID(), ImVec2(24, 24))) {
                    ComponentsManager::get()->getComponentRender()->stopLUAScripts();
                }
                ImGui::SameLine();
            }
            if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("reloadIcon")->getOGLTextureID(), ImVec2(24, 24))) {
                ComponentsManager::get()->getComponentRender()->reloadLUAScripts();
            }
            ImGui::SameLine();
            if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("removeIcon")->getOGLTextureID(), ImVec2(24, 24))) {
                ComponentsManager::get()->getComponentRender()->getSceneLoader().clearScene();
            }
        }
        ImGui::End();
    }
};
#endif //BRAKEZA3D_GUIWIDGETTOOLBAR_H
