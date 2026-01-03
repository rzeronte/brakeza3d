#ifndef MULTIPLYNODE_H
#define MULTIPLYNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditor.h"
#include "../NodeEditorManager.h"

class MultiplyNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Multiply";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.6f, 0.5f, 0.3f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditor* editor) override {
        editor->AddInputPin(node, "A", PinType::Vector);
        editor->AddInputPin(node, "B", PinType::Vector);
        editor->AddOutputPin(node, "Result", PinType::Vector);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditor* editor) override
    {
        if (node->inputs.size() < 2) {
            return "";
        }

        std::string varName = "var" + std::to_string(node->id);

        std::string a = editor->TraverseNode(node->inputs[0]->id, code);
        std::string b = editor->TraverseNode(node->inputs[1]->id, code);

        if (a.empty()) a = "vec3(1.0)";
        if (b.empty()) b = "vec3(1.0)";

        code << "    vec3 " << varName << " = " << a << " * " << b << ";\n";

        return varName;
    }
};

#endif // MULTIPLYNODE_H