#ifndef UVCOORDSNODE_H
#define UVCOORDSNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class UVCoordsNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "UV Coords";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.3f, 0.6f, 0.3f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddOutputPin(node, "UV", PinType::Vector);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        std::string varName = "var" + std::to_string(node->id);
        code << "    vec2 " << varName << " = v_TexCoord;\n";
        return varName;
    }
};

#endif // UVCOORDSNODE_H