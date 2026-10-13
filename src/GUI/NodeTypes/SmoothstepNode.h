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

    void SerializeParams(const std::shared_ptr<Node>& node, cJSON* out) const override {
        const ImVec2* edges = (const ImVec2*)node->userData;
        if (!edges) return;
        cJSON_AddNumberToObject(out, "edge0", edges->x);
        cJSON_AddNumberToObject(out, "edge1", edges->y);
    }

    void DeserializeParams(const std::shared_ptr<Node>& node, const cJSON* in) override {
        ImVec2* edges = (ImVec2*)node->userData;
        if (!edges) return;
        edges->x = NodeJson::Float(in, "edge0", edges->x);
        edges->y = NodeJson::Float(in, "edge1", edges->y);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "Value", PinType::Float);
        editor->AddOutputPin(node, "Output", PinType::Float);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        ImGui::BeginGroup();

        if (node->userData) {
            ImVec2* edges = (ImVec2*)node->userData;
            // uniform: sin recompilar. Rango amplio: los ejemplos usan bordes fuera de 0..1 (p. ej. -1..1)
            ImGui::DragFloat("Edge 0", &edges->x, 0.01f, -10.0f, 10.0f);
            ImGui::DragFloat("Edge 1", &edges->y, 0.01f, -10.0f, 10.0f);
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
        if (edges) {
            const std::string e = editor->AddUniform("u_p" + std::to_string(node->id) + "_edges", 2, (const float*)edges);
            code << "    float " << varName << " = smoothstep(" << e << ".x, " << e << ".y, " << value << ");\n";
        } else {
            code << "    float " << varName << " = smoothstep(0.0, 1.0, " << value << ");\n";
        }

        return varName;
    }
};

#endif // SMOOTHSTEPNODE_H