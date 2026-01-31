#ifndef ADDFLOATNODE_H
#define ADDFLOATNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class AddFloatNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "AddFloat";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.4f, 0.6f, 0.5f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "A", PinType::Float);
        editor->AddInputPin(node, "B", PinType::Float);
        editor->AddOutputPin(node, "Result", PinType::Float);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        if (node->inputs.size() < 2) {
            return "";
        }

        std::string varName = "var" + std::to_string(node->id);

        std::string a = editor->TraverseNode(node->inputs[0]->id, code);
        std::string b = editor->TraverseNode(node->inputs[1]->id, code);

        if (a.empty()) a = "0.0";
        if (b.empty()) b = "0.0";

        code << "    float " << varName << " = " << a << " + " << b << ";\n";

        return varName;
    }
};

#endif // ADDFLOATNODE_H
