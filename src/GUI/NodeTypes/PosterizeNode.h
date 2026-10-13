#ifndef POSTERIZENODE_H
#define POSTERIZENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class PosterizeNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Posterize";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.6f, 0.4f, 0.5f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        node->userData = new float(4.0f);   // niveles por canal
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete (float*)node->userData;
            node->userData = nullptr;
        }
    }

    void SerializeParams(const std::shared_ptr<Node>& node, cJSON* out) const override {
        const float* levels = (const float*)node->userData;
        if (levels) cJSON_AddNumberToObject(out, "levels", *levels);
    }

    void DeserializeParams(const std::shared_ptr<Node>& node, const cJSON* in) override {
        float* levels = (float*)node->userData;
        if (levels) *levels = NodeJson::Float(in, "levels", *levels);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "Color", PinType::Vector);
        editor->AddOutputPin(node, "Result", PinType::Vector);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        float* levels = (float*)node->userData;
        if (!levels) return 0.f;

        ImGui::BeginGroup();
        ImGui::PushID(node->id);
        ImGui::SetNextItemWidth(100);
        ImGui::SliderFloat("Levels", levels, 2.0f, 32.0f, "%.0f");   // uniform: sin recompilar
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
        std::string input = editor->TraverseNode(node->inputs[0]->id, code);
        if (input.empty()) input = "vec3(0.5)";

        float* levels = (float*)node->userData;
        std::string l = levels ? editor->AddUniform("u_p" + std::to_string(node->id) + "_levels", 1, levels) : "4.0";

        code << "    vec3 " << varName << " = floor(" << input << " * max(" << l << ", 1.0)) / max(" << l << ", 1.0);\n";
        return varName;
    }
};

#endif // POSTERIZENODE_H
