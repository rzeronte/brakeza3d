#ifndef CONTRASTNODE_H
#define CONTRASTNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class ContrastNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Contrast";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.6f, 0.4f, 0.3f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        node->userData = new float(1.0f);
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete (float*)node->userData;
            node->userData = nullptr;
        }
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "Color", PinType::Vector);
        editor->AddOutputPin(node, "Result", PinType::Vector);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        float* contrast = (float*)node->userData;
        if (!contrast) return 0.f;

        ImGui::BeginGroup();
        ImGui::PushID(node->id);

        ImGui::SetNextItemWidth(100);
        if (ImGui::SliderFloat("Contrast", contrast, 0.0f, 3.0f)) {
            editor->SetNeedsRecompile();
        }

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
        if (node->inputs.empty()) return "";

        std::string varName = "var" + std::to_string(node->id);

        std::string inputColor = editor->TraverseNode(node->inputs[0]->id, code);
        if (inputColor.empty()) inputColor = "vec3(0.5)";

        float* contrast = (float*)node->userData;
        float c = contrast ? *contrast : 1.0f;

        code << "    vec3 " << varName << " = (" << inputColor << " - 0.5) * " << c << " + 0.5;\n";

        return varName;
    }
};

#endif // CONTRASTNODE_H
