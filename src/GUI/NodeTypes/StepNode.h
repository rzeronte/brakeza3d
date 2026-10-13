#ifndef STEPNODE_H
#define STEPNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class StepNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Step";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.5f, 0.6f, 0.8f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        node->userData = new float(0.5f);   // edge
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete (float*)node->userData;
            node->userData = nullptr;
        }
    }

    void SerializeParams(const std::shared_ptr<Node>& node, cJSON* out) const override {
        const float* edge = (const float*)node->userData;
        if (edge) cJSON_AddNumberToObject(out, "edge", *edge);
    }

    void DeserializeParams(const std::shared_ptr<Node>& node, const cJSON* in) override {
        float* edge = (float*)node->userData;
        if (edge) *edge = NodeJson::Float(in, "edge", *edge);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "Value", PinType::Float);
        editor->AddOutputPin(node, "Result", PinType::Float);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        float* edge = (float*)node->userData;
        if (!edge) return 0.f;

        ImGui::BeginGroup();
        ImGui::PushID(node->id);
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("Edge", edge, 0.01f, -10.0f, 10.0f);   // uniform: sin recompilar
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
        std::string value = editor->TraverseNode(node->inputs[0]->id, code);
        if (value.empty()) value = "0.0";

        float* edge = (float*)node->userData;
        std::string e = edge ? editor->AddUniform("u_p" + std::to_string(node->id) + "_edge", 1, edge) : "0.5";

        // 0 si value < edge, 1 si value >= edge
        code << "    float " << varName << " = step(" << e << ", " << value << ");\n";
        return varName;
    }
};

#endif // STEPNODE_H
