#ifndef UVOFFSETNODE_H
#define UVOFFSETNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditor.h"
#include "../NodeEditorManager.h"

class UVOffsetNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "UV Offset";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.4f, 0.7f, 0.6f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditor* editor) override {
        editor->AddInputPin(node, "UV", PinType::Vector);
        editor->AddInputPin(node, "Offset", PinType::Vector);
        editor->AddOutputPin(node, "UV", PinType::Vector);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditor* editor) override
    {
        std::string varName = "var" + std::to_string(node->id);

        std::string uv = editor->TraverseNode(node->inputs[0]->id, code);
        std::string offset = editor->TraverseNode(node->inputs[1]->id, code);

        if (uv.empty()) uv = "v_TexCoord";
        if (offset.empty()) offset = "vec2(0.0)";

        code << "    vec2 " << varName << " = " << uv << " + " << offset << ";\n";

        return varName;
    }
};

#endif // UVOFFSETNODE_H