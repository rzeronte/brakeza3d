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
        editor->AddInputPin(node, "UV", PinType::Vec2);
        editor->AddInputPin(node, "Angle", PinType::Float);
        editor->AddOutputPin(node, "UV", PinType::Vec2);
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

        // Variables propias del nodo: dos UV Rotate en el mismo grafo no deben redeclararse
        const std::string centered = varName + "_centered";
        const std::string s = varName + "_sin";
        const std::string c = varName + "_cos";

        code << "    vec2 " << centered << " = " << uv << ".xy - vec2(0.5, 0.5);\n";
        code << "    float " << s << " = sin(" << angle << ");\n";
        code << "    float " << c << " = cos(" << angle << ");\n";
        code << "    vec2 " << varName << " = vec2(\n";
        code << "        " << centered << ".x * " << c << " - " << centered << ".y * " << s << ",\n";
        code << "        " << centered << ".x * " << s << " + " << centered << ".y * " << c << "\n";
        code << "    ) + vec2(0.5, 0.5);\n";
        return varName;
    }
};

#endif // UVROTATENODE_H