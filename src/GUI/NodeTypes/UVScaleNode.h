#ifndef UVSCALENODE_H
#define UVSCALENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class UVScaleNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "UV Scale";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.5f, 0.6f, 0.7f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "UV", PinType::Vector);
        editor->AddInputPin(node, "Offset", PinType::Vector);
        editor->AddOutputPin(node, "UV", PinType::Vector);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        std::string varName = "var" + std::to_string(node->id);

        std::string uv = editor->TraverseNode(node->inputs[0]->id, code);
        std::string scale = editor->TraverseNode(node->inputs[1]->id, code);

        if (uv.empty()) uv = "v_TexCoord";
        if (scale.empty()) scale = "vec2(1.0)";

        code << "    vec2 " << varName << " = " << uv << ".xy * " << scale << ".xy;\n";

        return varName;
    }
};

#endif // UVSCALENODE_H