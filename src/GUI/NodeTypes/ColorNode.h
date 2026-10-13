#ifndef COLORNODE_H
#define COLORNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class ColorNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Color";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.8f, 0.4f, 0.2f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        node->userData = new ImVec4(1.0f, 0.5f, 0.2f, 1.0f);
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete (ImVec4*)node->userData;
            node->userData = nullptr;
        }
    }

    void SerializeParams(const std::shared_ptr<Node>& node, cJSON* out) const override {
        const ImVec4* color = (const ImVec4*)node->userData;
        if (!color) return;
        cJSON_AddNumberToObject(out, "r", color->x);
        cJSON_AddNumberToObject(out, "g", color->y);
        cJSON_AddNumberToObject(out, "b", color->z);
    }

    void DeserializeParams(const std::shared_ptr<Node>& node, const cJSON* in) override {
        ImVec4* color = (ImVec4*)node->userData;
        if (!color) return;
        color->x = NodeJson::Float(in, "r", color->x);
        color->y = NodeJson::Float(in, "g", color->y);
        color->z = NodeJson::Float(in, "b", color->z);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddOutputPin(node, "RGB", PinType::Vector);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        ImGui::BeginGroup();
        if (node->userData) {
            ImVec4* color = (ImVec4*)node->userData;
            std::string colorId = "##color" + std::to_string(node->id);
            ImGui::ColorEdit3(colorId.c_str(), (float*)color);   // uniform: no hace falta recompilar
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
        ImVec4* color = (ImVec4*)node->userData;

        if (color) {
            const std::string uniform = editor->AddUniform("u_p" + std::to_string(node->id) + "_color", 3, (const float*)color);
            code << "    vec3 " << varName << " = " << uniform << ";\n";
        } else {
            code << "    vec3 " << varName << " = vec3(1.0, 1.0, 1.0);\n";
        }

        return varName;
    }
};

#endif // COLORNODE_H