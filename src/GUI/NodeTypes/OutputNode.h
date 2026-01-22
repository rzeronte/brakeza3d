#ifndef OUTPUTNODE_H
#define OUTPUTNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class OutputNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Output";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
    }

    GUIType::Sheet GetIcon() const override { return IconGUI::NODE_TYPE_OUTPUT; }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "Color", PinType::Vector);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        // El nodo Output no genera código directamente,
        // ya que es el punto de entrada del traversal en GenerateShaderCode()
        return "";
    }
};

#endif // OUTPUTNODE_H