#ifndef MESHTEXTURENODE_H
#define MESHTEXTURENODE_H

#include "NodeType.h"
#include "../../../include/GUI/ShaderNodeEditorManager.h"

class MeshTextureNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Mesh Texture";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.9f, 0.5f, 0.3f, 1.0f); // Naranja para texturas de mesh
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        // Almacenar el tipo de textura (por defecto: Diffuse)
        node->userData = new MeshTextureType(MeshTextureType::Diffuse);
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete (MeshTextureType*)node->userData;
            node->userData = nullptr;
        }
        editor->DeleteImageForNode(node->id);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "UV", PinType::Vector);
        editor->AddOutputPin(node, "Color", PinType::Vector);
        editor->AddOutputPin(node, "Alpha", PinType::Float);

        editor->SetExternalTextureForNode(node->id, 0);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        MeshTextureType* texType = (MeshTextureType*)node->userData;
        if (!texType) return 0.f;

        ImGui::BeginGroup();

        ImGui::PushID(node->id);
        ImGui::PushItemWidth(200);

        // Texto indicando tipo actual
        ImGui::Text("Texture Type:");

        // Botones para cambiar tipo
        const char* typeNames[] = { "Diffuse", "Specular", "Normal", "Roughness", "AO" };
        ImVec4 activeColor = ImVec4(0.9f, 0.5f, 0.3f, 1.0f);

        // Primera fila: Diffuse, Specular
        for (int i = 0; i < 2; i++) {
            if (i > 0) ImGui::SameLine();

            bool isActive = ((int)*texType == i);
            if (isActive) {
                ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
            }

            if (ImGui::Button(typeNames[i], ImVec2(95, 0))) {
                *texType = (MeshTextureType)i;
                editor->SetNeedsRecompile();
            }

            if (isActive) {
                ImGui::PopStyleColor();
            }
        }

        // Segunda fila: Normal, Roughness, AO (deshabilitados por ahora)
        ImGui::BeginDisabled();
        for (int i = 2; i < 5; i++) {
            if (i > 2) ImGui::SameLine();
            ImGui::Button(typeNames[i], ImVec2(63, 0));
        }
        ImGui::EndDisabled();

        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled)) {
            ImGui::SetTooltip("Coming soon...");
        }

        ImGui::Spacing();

        // Mostrar preview de la textura
        auto& textures = editor->GetNodeTextures();
        if (textures.find(node->id) != textures.end()) {
            auto& tex = textures[node->id];
            if (tex.image && tex.image->getOGLTextureID() != 0) {
                ImGui::Text("Preview:");
                float previewSize = 200.0f;
                float aspect = (float)tex.image->width() / (float)tex.image->height();
                ImVec2 imageSize;

                if (aspect > 1.0f) {
                    imageSize = ImVec2(previewSize, previewSize / aspect);
                } else {
                    imageSize = ImVec2(previewSize * aspect, previewSize);
                }

                // Invertir Y para mostrar correctamente
                ImGui::Image(
                    tex.image->getOGLImTexture(),
                    imageSize,
                    ImVec2(0, 1),
                    ImVec2(1, 0)
                );
            } else {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No texture bound");
            }
        }

        ImGui::PopItemWidth();
        ImGui::PopID();
        ImGui::EndGroup();

        return MeasureUIWidth();
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        std::string varName = "var" + std::to_string(node->id);
        auto& textures = editor->GetNodeTextures();

        if (textures.find(node->id) != textures.end() && textures[node->id].image) {
            std::string uvCoords = "v_TexCoord";

            if (node->inputs.size() > 0) {
                std::string customUV = editor->TraverseNode(node->inputs[0]->id, code);
                if (!customUV.empty()) {
                    uvCoords = customUV;
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

#endif // MESHTEXTURENODE_H