#ifndef SATURATIONNODE_H
#define SATURATIONNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class SaturationNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Saturation";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.7f, 0.5f, 0.3f, 1.0f);
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
        float* saturation = (float*)node->userData;
        if (!saturation) return 0.f;

        ImGui::BeginGroup();
        ImGui::PushID(node->id);

        ImGui::SetNextItemWidth(100);
        if (ImGui::SliderFloat("Saturation", saturation, 0.0f, 3.0f)) {
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

        float* saturation = (float*)node->userData;
        float s = saturation ? *saturation : 1.0f;

        code << "    float " << varName << "_gray = dot(" << inputColor << ", vec3(0.2126, 0.7152, 0.0722));\n";
        code << "    vec3 " << varName << " = mix(vec3(" << varName << "_gray), " << inputColor << ", " << s << ");\n";

        return varName;
    }
};

#endif // SATURATIONNODE_H
