#ifndef INTERNALTEXTURENODE_H
#define INTERNALTEXTURENODE_H

#include "NodeType.h"
#include "../../../include/GUI/ShaderNodeEditorManager.h"

class InternalTextureNode : public NodeType {
public:
    std::string GetTypeName() const override
    {
        return "Internal Texture";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.3f, 0.7f, 0.9f, 1.0f); // Azul para texturas internas
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        // Almacenar el tipo de textura (por defecto: Color)
        node->userData = new InternalTextureType(InternalTextureType::Color);
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete (InternalTextureType*)node->userData;
            node->userData = nullptr;
        }
        editor->DeleteImageForNode(node->id);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "UV", PinType::Vector);
        editor->AddOutputPin(node, "Color", PinType::Vector);
        editor->AddOutputPin(node, "Alpha", PinType::Float);

        // Inicializar la textura (se actualizará cada frame)
        std::cout << "⭐ SetupPins called for Internal Texture node " << node->id << std::endl;
        editor->SetExternalTextureForNode(node->id, 0);
        std::cout << "⭐ SetExternalTextureForNode finished for node " << node->id << std::endl;
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        InternalTextureType* texType = (InternalTextureType*)node->userData;
        if (!texType) return 0.f;

        ImGui::BeginGroup();
        ImGui::PushID(node->id);
        ImGui::PushItemWidth(200);

        // Texto indicando tipo actual
        ImGui::Text("Texture Type:");

        // Botones para cambiar tipo
        const char* typeNames[] = { "Color", "Depth" };
        ImVec4 activeColor = ImVec4(0.3f, 0.8f, 0.3f, 1.0f);

        for (int i = 0; i < 2; i++) {
            if (i > 0) ImGui::SameLine();

            bool isActive = ((int)*texType == i);
            if (isActive) {
                ImGui::PushStyleColor(ImGuiCol_Button, activeColor);
            }

            if (ImGui::Button(typeNames[i], ImVec2(95, 0))) {
                *texType = (InternalTextureType)i;
                editor->SetNeedsRecompile();
            }

            if (isActive) {
                ImGui::PopStyleColor();
            }
        }

        ImGui::Spacing();

        // Preview de la textura
        ImGui::Text("Preview");
        auto& textures = editor->GetNodeTextures();
        if (textures.find(node->id) != textures.end()) {
            auto& tex = textures[node->id];
            if (tex.image && tex.image->getOGLTextureID() != 0) {
                float previewSize = 200.0f;
                float aspect = (float)tex.image->width() / (float)tex.image->height();
                ImVec2 imageSize;

                if (aspect > 1.0f) {
                    imageSize = ImVec2(previewSize, previewSize / aspect);
                } else {
                    imageSize = ImVec2(previewSize * aspect, previewSize);
                }

                ImGui::Image(
                    tex.image->getOGLImTexture(),
                    imageSize,
                    ImVec2(0, 1),  // UV inicio (esquina inferior izquierda)
                    ImVec2(1, 0)   // UV final (esquina superior derecha)
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

#endif // INTERNALTEXTURENODE_H