#ifndef IMAGENODE_H
#define IMAGENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditor.h"
#include "../NodeEditorManager.h"
#include <cmath>

class ImageNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Image";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.7f, 0.3f, 0.7f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        node->userData = new std::string("");
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditor* editor) override {
        if (node->userData) {
            delete (std::string*)node->userData;
            node->userData = nullptr;
        }
        editor->DeleteImageForNode(node->id);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditor* editor) override {
        editor->AddInputPin(node, "UV", PinType::Vector);
        editor->AddOutputPin(node, "Color", PinType::Vector);
        editor->AddOutputPin(node, "Alpha", PinType::Float);
    }

    void RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditor* editor) override {
        std::string* filepath = (std::string*)node->userData;
        if (!filepath) return;

        ImGui::PushItemWidth(200);

        auto& textures = editor->GetNodeTextures();

        if (textures.find(node->id) != textures.end()) {
            auto& tex = textures[node->id];
            if (tex.image) {
                ImGui::Text("Loaded: %dx%d", tex.image->width(), tex.image->height());
                float previewSize = 200.0f;
                float aspect = (float)tex.image->width() / (float)tex.image->height();
                ImVec2 imageSize;

                if (aspect > 1.0f) {
                    imageSize = ImVec2(previewSize, previewSize / aspect);
                } else {
                    imageSize = ImVec2(previewSize * aspect, previewSize);
                }

                ImGui::Image(tex.image->getOGLImTexture(), imageSize);

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM)) {
                        auto selection = static_cast<char*>(payload->Data);
                        auto fullPath = Config::get()->IMAGES_FOLDER + selection;
                        *filepath = fullPath;
                        editor->LoadImageForNode(node->id, fullPath);
                    }
                    ImGui::EndDragDropTarget();
                }

                if (ImGui::Button("Remove Image", ImVec2(200, 0))) {
                    editor->DeleteImageForNode(node->id);
                    *filepath = "";
                    editor->SetNeedsRecompile();
                }
            }
        } else {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No image loaded");
            ImGui::Spacing();

            ImVec2 dropZoneSize(200, 150);
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            ImGui::InvisibleButton("##dropzone", dropZoneSize);

            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImU32 borderColor = ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 0.5f));

            if (ImGui::IsItemHovered() && ImGui::GetDragDropPayload()) {
                borderColor = ImGui::GetColorU32(ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
            }

            for (int i = 0; i < 4; i++) {
                ImVec2 p1, p2;
                switch(i) {
                    case 0: p1 = cursorPos; p2 = ImVec2(cursorPos.x + dropZoneSize.x, cursorPos.y); break;
                    case 1: p1 = ImVec2(cursorPos.x + dropZoneSize.x, cursorPos.y);
                            p2 = ImVec2(cursorPos.x + dropZoneSize.x, cursorPos.y + dropZoneSize.y); break;
                    case 2: p1 = ImVec2(cursorPos.x + dropZoneSize.x, cursorPos.y + dropZoneSize.y);
                            p2 = ImVec2(cursorPos.x, cursorPos.y + dropZoneSize.y); break;
                    case 3: p1 = ImVec2(cursorPos.x, cursorPos.y + dropZoneSize.y); p2 = cursorPos; break;
                }

                float len = sqrtf(powf(p2.x - p1.x, 2) + powf(p2.y - p1.y, 2));
                int segments = (int)(len / 10.0f);
                for (int j = 0; j < segments; j += 2) {
                    float t1 = (float)j / (float)segments;
                    float t2 = (float)(j + 1) / (float)segments;
                    ImVec2 segStart = ImVec2(p1.x + (p2.x - p1.x) * t1, p1.y + (p2.y - p1.y) * t1);
                    ImVec2 segEnd = ImVec2(p1.x + (p2.x - p1.x) * t2, p1.y + (p2.y - p1.y) * t2);
                    drawList->AddLine(segStart, segEnd, borderColor, 2.0f);
                }
            }

            const char* dropText = "Drop image here";
            ImVec2 textSize = ImGui::CalcTextSize(dropText);
            ImVec2 textPos = ImVec2(
                cursorPos.x + (dropZoneSize.x - textSize.x) * 0.5f,
                cursorPos.y + (dropZoneSize.y - textSize.y) * 0.5f
            );
            drawList->AddText(textPos, ImGui::GetColorU32(ImVec4(0.6f, 0.6f, 0.6f, 1.0f)), dropText);

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM)) {
                    auto selection = static_cast<char*>(payload->Data);
                    auto fullPath = Config::get()->IMAGES_FOLDER + selection;
                    *filepath = fullPath;
                    editor->LoadImageForNode(node->id, fullPath);
                }
                ImGui::EndDragDropTarget();
            }
        }

        if (!filepath->empty()) {
            ImGui::Spacing();
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 200);
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Path:");
            ImGui::TextWrapped("%s", filepath->c_str());
            ImGui::PopTextWrapPos();
        }

        ImGui::PopItemWidth();
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditor* editor) override
    {
        std::string varName = "var" + std::to_string(node->id);
        auto& textures = editor->GetNodeTextures();

        if (textures.find(node->id) != textures.end() && textures[node->id].image) {
            // Obtener las coordenadas UV (si están conectadas)
            std::string uvCoords = "v_TexCoord";  // Por defecto

            if (node->inputs.size() > 0) {
                std::string customUV = editor->TraverseNode(node->inputs[0]->id, code);
                if (!customUV.empty()) {
                    uvCoords = customUV;  // Usar UV modificadas
                }
            }

            if (sourcePin->name == "Alpha") {
                code << "    float " << varName << " = texture(u_Texture" << node->id
                     << ", " << uvCoords << ").a;\n";
            } else {
                code << "    vec3 " << varName << " = texture(u_Texture" << node->id
                     << ", " << uvCoords << ").rgb;\n";
            }
        } else {
            code << "    vec3 " << varName << " = vec3(1.0, 0.0, 1.0);\n";
        }

        return varName;
    }
};

#endif // IMAGENODE_H