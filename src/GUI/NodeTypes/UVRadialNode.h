#ifndef UVRADIALNODE_H
#define UVRADIALNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class UVRadialNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "UVRadial";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.6f, 0.7f, 0.4f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "UV", PinType::Vec2);
        editor->AddOutputPin(node, "Distance", PinType::Float);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        std::string varName = "var" + std::to_string(node->id);
        std::string uv = editor->TraverseNode(node->inputs[0]->id, code);
        if (uv.empty()) uv = "v_TexCoord";

        // 0 en el centro de la pantalla/textura, 1 en el punto medio de cada borde
        code << "    float " << varName << " = length(" << uv << " - vec2(0.5)) * 2.0;\n";
        return varName;
    }
};

#endif // UVRADIALNODE_H
