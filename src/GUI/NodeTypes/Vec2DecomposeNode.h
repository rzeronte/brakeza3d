#ifndef VEC2DECOMPOSENODE_H
#define VEC2DECOMPOSENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class Vec2DecomposeNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Vec2Decompose";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.5f, 0.6f, 0.4f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "Vec2", PinType::Vec2);
        editor->AddOutputPin(node, "X", PinType::Float);
        editor->AddOutputPin(node, "Y", PinType::Float);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        if (node->inputs.empty() || node->outputs.size() < 2) {
            return "";
        }

        std::string inputVar = editor->TraverseNode(node->inputs[0]->id, code);
        if (inputVar.empty()) inputVar = "vec2(0.0)";

        // El gestor pide cada salida por separado y cachea por pin: se genera solo la solicitada
        const bool wantsX = sourcePin && sourcePin->id == node->outputs[0]->id;
        const std::string varName = "var" + std::to_string(node->id) + (wantsX ? "_x" : "_y");

        code << "    float " << varName << " = " << inputVar << (wantsX ? ".x" : ".y") << ";\n";
        return varName;
    }
};

#endif // VEC2DECOMPOSENODE_H
