#ifndef GRADIENTNODE_H
#define GRADIENTNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class GradientNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Gradient";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.5f, 0.3f, 0.6f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "UV", PinType::Vector);
        editor->AddOutputPin(node, "Color", PinType::Vector);
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

        code << "    vec3 " << varName << " = vec3(" << uv << ".x, " << uv << ".y, 0.5);\n";

        return varName;
    }
};

#endif // GRADIENTNODE_H