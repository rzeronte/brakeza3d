#ifndef PIXELATENODE_H
#define PIXELATENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class PixelateNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Pixelate";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.6f, 0.7f, 0.5f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        node->userData = new float(64.0f);   // celdas por unidad de UV
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete (float*)node->userData;
            node->userData = nullptr;
        }
    }

    void SerializeParams(const std::shared_ptr<Node>& node, cJSON* out) const override {
        const float* cells = (const float*)node->userData;
        if (cells) cJSON_AddNumberToObject(out, "cells", *cells);
    }

    void DeserializeParams(const std::shared_ptr<Node>& node, const cJSON* in) override {
        float* cells = (float*)node->userData;
        if (cells) *cells = NodeJson::Float(in, "cells", *cells);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "UV", PinType::Vec2);
        editor->AddOutputPin(node, "UV", PinType::Vec2);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        float* cells = (float*)node->userData;
        if (!cells) return 0.f;

        ImGui::BeginGroup();
        ImGui::PushID(node->id);
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("Cells", cells, 0.5f, 1.0f, 2048.0f, "%.0f");   // uniform: sin recompilar
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
        std::string uv = editor->TraverseNode(node->inputs[0]->id, code);
        if (uv.empty()) uv = "v_TexCoord";

        float* cells = (float*)node->userData;
        std::string c = cells ? editor->AddUniform("u_p" + std::to_string(node->id) + "_cells", 1, cells) : "64.0";

        // Centro de cada celda: todas las UV de la celda muestrean el mismo punto
        code << "    vec2 " << varName << " = (floor(" << uv << " * max(" << c << ", 1.0)) + 0.5) / max(" << c << ", 1.0);\n";
        return varName;
    }
};

#endif // PIXELATENODE_H
