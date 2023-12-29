//
// Created by edu on 29/12/23.
//

#ifndef BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
#define BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H

#include "imgui.h"
#include "../ComponentsManager.h"
#include "GUIWidgetObject3DProperties.h"

struct GUIWidgetProjectSettings {
    TexturePackage &ImGuiTextures;
    ScriptEditableManager &scriptEditableManager;

    GUIWidgetProjectSettings(TexturePackage &imGuiTextures,ScriptEditableManager &scriptEditableManager)
    :
        ImGuiTextures(imGuiTextures),
        scriptEditableManager(scriptEditableManager)
    {
    }

    void draw()
    {
        if (ImGui::Begin("Project")) {
            auto componentRender = ComponentsManager::get()->getComponentRender();
            static char name[256];

            strncpy(name, EngineSetup::get()->ENGINE_TITLE.c_str(), sizeof(name));

            ImGui::InputText("Project name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll);
            if (ImGui::IsItemEdited()) {
                EngineSetup::get()->ENGINE_TITLE = name;
            }
            ImGui::Separator();

            if (ImGui::TreeNode("Global illumination")) {
                const float range_illumination_min_settings = -1.0f;
                const float range_illumination_max_settings = 1.0f;
                const float sens_illumination_settings = 0.01f;
                auto dirLight = ComponentsManager::get()->getComponentWindow()->getShaderOGLRender()->getDirectionalLight();
                if (ImGui::TreeNode("Direction")) {
                    ImGui::DragScalar("x", ImGuiDataType_Float, &dirLight->direction.x, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("y", ImGuiDataType_Float, &dirLight->direction.y, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("z", ImGuiDataType_Float, &dirLight->direction.z, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Ambient")) {
                    ImGui::DragScalar("x", ImGuiDataType_Float, &dirLight->ambient.x, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("y", ImGuiDataType_Float, &dirLight->ambient.y, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("z", ImGuiDataType_Float, &dirLight->ambient.z, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("Diffuse")) {
                    ImGui::DragScalar("x", ImGuiDataType_Float, &dirLight->diffuse.x, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("y", ImGuiDataType_Float, &dirLight->diffuse.y, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("z", ImGuiDataType_Float, &dirLight->diffuse.z, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Specular")) {
                    ImGui::DragScalar("x", ImGuiDataType_Float, &dirLight->specular.x, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("y", ImGuiDataType_Float, &dirLight->specular.y, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::DragScalar("z", ImGuiDataType_Float, &dirLight->specular.z, sens_illumination_settings,&range_illumination_min_settings, &range_illumination_max_settings, "%f", 1.0f);
                    ImGui::TreePop();
                }
                ImGui::TreePop();
            }

            ImGui::Separator();
            if (ImGui::TreeNode("DOF Settings")) {
                const float focalMinValues = 0;
                const float focalMaxValues = 1;
                const float focalValueSens = 0.001;

                const float depthMinValues = 0;
                const float depthMaxValues = 100;
                const float depthValueSens = 0.1;

                const int minBlurRadius = 0;
                const int maxBlurRadius = 10;

                ImGui::Checkbox("Enable DOF", &EngineSetup::get()->ENABLE_DEPTH_OF_FIELD);

                if (EngineSetup::get()->ENABLE_DEPTH_OF_FIELD) {
                    ImGui::DragScalar("Focal range", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->focalRange, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
                    ImGui::DragScalar("Focal distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->focalDistance, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
                    ImGui::DragScalar("Blur radius", ImGuiDataType_S32, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->blurRadius,1.0f, &minBlurRadius, &maxBlurRadius, "%d", 1.0f);
                    ImGui::DragScalar("Intensity", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->intensity, focalValueSens, &focalMinValues, &focalMaxValues, "%f", 1.0f);
                    ImGui::DragScalar("Far Plane", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLDOF()->farPlane, depthValueSens, &depthMinValues, &depthMaxValues, "%f", 1.0f);
                }
                ImGui::TreePop();
            }
            ImGui::Separator();
            if (ImGui::TreeNode("FOG Settings")) {
                ImGui::Checkbox("Enable FOG", &EngineSetup::get()->ENABLE_FOG);

                if (EngineSetup::get()->ENABLE_FOG) {
                    const float rangeFogSens = 0.1;
                    const float rangeFogMin = 0;
                    const float rangeFogMax = 0;

                    ImGui::DragScalar("FOG min distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLFOG()->fogMinDist, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);
                    ImGui::DragScalar("FOG max distance", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLFOG()->fogMaxDist, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);
                    ImGui::DragScalar("FOG intensity", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLFOG()->intensity, rangeFogSens, &rangeFogMin, &rangeFogMax, "%f", 1.0f);

                    if (ImGui::TreeNode("FOG Color")) {
                        const float fogColorSend = 0.01;
                        const float fogColorMin = 0;
                        const float fogColorMax = 1;

                        ImGui::DragScalar("x", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLFOG()->fogColor.r, fogColorSend,&fogColorMin, &fogColorMax, "%f", 1.0f);
                        ImGui::DragScalar("y", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLFOG()->fogColor.g, fogColorSend,&fogColorMin, &fogColorMax, "%f", 1.0f);
                        ImGui::DragScalar("z", ImGuiDataType_Float, &ComponentsManager::get()->getComponentWindow()->getShaderOGLFOG()->fogColor.b, fogColorSend,&fogColorMin, &fogColorMax, "%f", 1.0f);
                        ImGui::TreePop();
                    }
                }
                ImGui::TreePop();
            }
            ImGui::Separator();
            ImGui::Checkbox("Show Depth Map", &EngineSetup::get()->SHOW_DEPTH_OF_FIELD);

            auto scripts = componentRender->getLUAScripts();
            for (int i = 0; i < (int) scripts.size(); i++) {
                auto currentScript = scripts[i];
                ImGui::PushID(i);
                ImGui::Image((ImTextureID)ImGuiTextures.getTextureByLabel("scriptIcon")->getOGLTextureID(), ImVec2(24, 24));
                ImGui::SameLine(48);
                std::string optionText = std::to_string(i + 1) + ") " + currentScript->scriptFilename;
                if (ImGui::Button(optionText.c_str())) {
                    scriptEditableManager.selectedScriptFilename = currentScript->scriptFilename;
                    delete scriptEditableManager.script;
                    scriptEditableManager.script = new ScriptLUA(scriptEditableManager.selectedScriptFilename, ScriptLUA::dataTypesFileFor(scriptEditableManager.selectedScriptFilename));
                    strcpy(scriptEditableManager.editableSource, scriptEditableManager.script->content);
                }
                ImGui::SameLine();

                if (currentScript->isPaused()) {
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("unlockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                        currentScript->setPaused(false);
                    }
                } else {
                    if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("lockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                        currentScript->setPaused(true);
                    }
                }
                ImGui::SameLine();
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures.getTextureByLabel("removeIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                    componentRender->removeScript(currentScript);
                }

                ImGui::PopID();
            }

            ImGui::Separator();

            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Drop here LUA scripts...");
            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCRIPT_ITEM")) {
                    Logging::Message("Dropping script (%s) in global space", payload->Data);
                    componentRender->addLUAScript(new ScriptLUA(
                            std::string((char*) payload->Data),
                            ScriptLUA::dataTypesFileFor(std::string((char *)payload->Data)))
                    );
                }
                ImGui::EndDragDropTarget();
            }
            ImGui::Separator();
        }
        ImGui::End();
    }
};

#endif //BRAKEZA3D_GUIWIDGETPROJECTSETTINGS_H
