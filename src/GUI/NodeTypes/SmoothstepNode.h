#ifndef SMOOTHSTEPNODE_H
#define SMOOTHSTEPNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class SmoothstepNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Smoothstep";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.5f, 0.7f, 0.4f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        // userData = {edge0, edge1}
        node->userData = new ImVec2(0.0f, 1.0f);
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete (ImVec2*)node->userData;
            node->userData = nullptr;
        }
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "Value", PinType::Float);
        editor->AddOutputPin(node, "Output", PinType::Float);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        ImGui::BeginGroup();

        if (node->userData) {
            ImVec2* edges = (ImVec2*)node->userData;
            if (ImGui::DragFloat("Edge 0", &edges->x, 0.01f, 0.0f, 1.0f)) {
                editor->SetNeedsRecompile();
            }
            if (ImGui::DragFloat("Edge 1", &edges->y, 0.01f, 0.0f, 1.0f)) {
                editor->SetNeedsRecompile();
            }
        }
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

        std::string value = editor->TraverseNode(node->inputs[0]->id, code);
        if (value.empty()) value = "0.0";

        ImVec2* edges = (ImVec2*)node->userData;
        float edge0 = edges ? edges->x : 0.0f;
        float edge1 = edges ? edges->y : 1.0f;

        code << "    float " << varName << " = smoothstep(" << edge0 << ", " << edge1 << ", " << value << ");\n";

        return varName;
    }
};

#endif // SMOOTHSTEPNODE_H