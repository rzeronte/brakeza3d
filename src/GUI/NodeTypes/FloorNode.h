#ifndef FLOORNODE_H
#define FLOORNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class FloorNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Floor";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.5f, 0.6f, 0.8f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "Value", PinType::Float);
        editor->AddOutputPin(node, "Result", PinType::Float);
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
        code << "    float " << varName << " = floor(" << value << ");\n";
        return varName;
    }
};

#endif // FLOORNODE_H
