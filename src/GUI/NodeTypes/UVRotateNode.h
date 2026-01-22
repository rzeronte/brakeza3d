#ifndef UVROTATENODE_H
#define UVROTATENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class UVRotateNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "UV Rotate";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.6f, 0.7f, 0.5f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "UV", PinType::Vector);
        editor->AddInputPin(node, "Angle", PinType::Float);
        editor->AddOutputPin(node, "UV", PinType::Vector);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        std::string varName = "var" + std::to_string(node->id);

        std::string uv = editor->TraverseNode(node->inputs[0]->id, code);
        std::string angle = editor->TraverseNode(node->inputs[1]->id, code);

        if (uv.empty()) uv = "v_TexCoord";
        if (angle.empty()) angle = "0.0";

        code << "    vec2 center = vec2(0.5, 0.5);\n";
        code << "    vec2 uv_centered = " << uv << " - center;\n";
        code << "    float s = sin(" << angle << ");\n";
        code << "    float c = cos(" << angle << ");\n";
        code << "    vec2 " << varName << " = vec2(\n";
        code << "        uv_centered.x * c - uv_centered.y * s,\n";
        code << "        uv_centered.x * s + uv_centered.y * c\n";
        code << "    ) + center;\n";

        return varName;
    }
};

#endif // UVROTATENODE_H