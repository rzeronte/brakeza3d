#ifndef COSNODE_H
#define COSNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditor.h"
#include "../NodeEditorManager.h"

class CosNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Cos";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.3f, 0.8f, 0.7f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditor* editor) override {
        editor->AddInputPin(node, "Input", PinType::Float);
        editor->AddOutputPin(node, "Output", PinType::Float);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditor* editor) override
    {
        std::string varName = "var" + std::to_string(node->id);

        std::string input = editor->TraverseNode(node->inputs[0]->id, code);
        if (input.empty()) input = "0.0";

        code << "    float " << varName << " = cos(" << input << ");\n";

        return varName;
    }
};

#endif // COSNODE_H