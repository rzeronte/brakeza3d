#ifndef SCALECOLORNODE_H
#define SCALECOLORNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class ScaleColorNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "ScaleColor";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.4f, 0.4f, 0.7f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        node->userData = new float(1.0f);   // factor si la entrada Factor no está conectada
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete (float*)node->userData;
            node->userData = nullptr;
        }
    }

    void SerializeParams(const std::shared_ptr<Node>& node, cJSON* out) const override {
        const float* factor = (const float*)node->userData;
        if (factor) cJSON_AddNumberToObject(out, "factor", *factor);
    }

    void DeserializeParams(const std::shared_ptr<Node>& node, const cJSON* in) override {
        float* factor = (float*)node->userData;
        if (factor) *factor = NodeJson::Float(in, "factor", *factor);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "Color", PinType::Vector);
        editor->AddInputPin(node, "Factor", PinType::Float);
        editor->AddOutputPin(node, "Result", PinType::Vector);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        float* factor = (float*)node->userData;
        if (!factor) return 0.f;

        ImGui::BeginGroup();
        ImGui::PushID(node->id);
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("Factor", factor, 0.01f, 0.0f, 10.0f);   // uniform: sin recompilar
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
        std::string color = editor->TraverseNode(node->inputs[0]->id, code);
        std::string factor = editor->TraverseNode(node->inputs[1]->id, code);
        if (color.empty()) color = "vec3(1.0)";
        if (factor.empty()) {
            float* f = (float*)node->userData;
            factor = f ? editor->AddUniform("u_p" + std::to_string(node->id) + "_factor", 1, f) : "1.0";
        }

        code << "    vec3 " << varName << " = " << color << " * " << factor << ";\n";
        return varName;
    }
};

#endif // SCALECOLORNODE_H
