#ifndef VEC2COMPOSENODE_H
#define VEC2COMPOSENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class Vec2ComposeNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Vec2Compose";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.4f, 0.6f, 0.5f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "X", PinType::Float);
        editor->AddInputPin(node, "Y", PinType::Float);
        editor->AddOutputPin(node, "Vec2", PinType::Vector);
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

        std::string x = editor->TraverseNode(node->inputs[0]->id, code);
        std::string y = editor->TraverseNode(node->inputs[1]->id, code);

        if (x.empty()) x = "0.0";
        if (y.empty()) y = "0.0";

        code << "    vec2 " << varName << " = vec2(" << x << ", " << y << ");\n";

        return varName;
    }
};

#endif // VEC2COMPOSENODE_H
