//
// Created by Eduardo on 24/11/2025.
//

#ifndef BRAKEZA3D_IMAGE3DDRAWERGUI_H
#define BRAKEZA3D_IMAGE3DDRAWERGUI_H

#include "../../3D/Object3D.h"

class ScriptLuaGUI
{
public:
    static void drawImGuiProperties(ScriptLUA *o)
    {
        ImGui::SeparatorText("LUA variables");

        if (o->dataTypes.empty()) {
            ImGui::Text("No variables found");
            return;
        }

        int i = 0;
        for (auto&  type: o->dataTypes) {
            ImGui::PushID(i);
            switch (BrakezaSetup::get()->LUADataTypesMapping[type.type]) {
                case BrakezaSetup::LUADataType::INT: {
                    int valueInt = std::get<int>(type.value);
                    if (ImGui::InputInt(type.name.c_str(), &valueInt)) {
                        type.value = valueInt;
                    }
                    break;
                }
                case BrakezaSetup::LUADataType::STRING: {
                    std::string valueString = std::get<const char*>(type.value);
                    static char name[256];
                    strncpy(name, valueString.c_str(), sizeof(name));
                    ImGui::InputText(type.name.c_str(), name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AlwaysOverwrite);
                    if (ImGui::IsItemEdited()) {
                        type.value = name;
                    }

                    break;
                }
                case BrakezaSetup::LUADataType::FLOAT: {
                    float valueFloat = std::get<float>(type.value);
                    if (ImGui::InputFloat(type.name.c_str(), &valueFloat, 0.01f, 1.0f, "%.3f")) {
                        type.value = valueFloat;
                    }
                    break;
                }
                case BrakezaSetup::LUADataType::VERTEX3D: {
                    Vertex3D valueVertex = std::get<Vertex3D>(type.value);
                    float vec4f[4];
                    valueVertex.toFloat(vec4f);
                    if (ImGui::DragFloat3(type.name.c_str(), vec4f, 0.01f, 0.0f, 1.0f)) {
                        valueVertex.x = vec4f[0];
                        valueVertex.y = vec4f[1];
                        valueVertex.z = vec4f[2];
                        type.value = valueVertex;
                    }
                    break;
                }
                default:
                    std::cerr << "Unknown data type." << std::endl;
            }
            i++;
            ImGui::PopID();
        }
    }
};

#endif //BRAKEZA3D_IMAGE3DDRAWERGUI_H