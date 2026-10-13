#ifndef FLOATVALUENODE_H
#define FLOATVALUENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class FloatValueNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "FloatValue";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.5f, 0.5f, 0.3f, 1.0f);
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

    void SerializeParams(const std::shared_ptr<Node>& node, cJSON* out) const override {
        const float* value = (const float*)node->userData;
        if (value) cJSON_AddNumberToObject(out, "value", *value);
    }

    void DeserializeParams(const std::shared_ptr<Node>& node, const cJSON* in) override {
        float* value = (float*)node->userData;
        if (value) *value = NodeJson::Float(in, "value", *value);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddOutputPin(node, "Value", PinType::Float);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        ImGui::BeginGroup();
        if (node->userData) {
            float* value = (float*)node->userData;
            std::string valueId = "##float" + std::to_string(node->id);
            ImGui::SetNextItemWidth(80);
            ImGui::DragFloat(valueId.c_str(), value, 0.01f, -100.0f, 100.0f, "%.3f");   // uniform: sin recompilar
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
        float* value = (float*)node->userData;

        if (value) {
            const std::string uniform = editor->AddUniform("u_p" + std::to_string(node->id) + "_value", 1, value);
            code << "    float " << varName << " = " << uniform << ";\n";
        } else {
            code << "    float " << varName << " = 1.0;\n";
        }

        return varName;
    }
};

#endif // FLOATVALUENODE_H
