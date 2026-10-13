#ifndef VEC3COMPOSENODE_H
#define VEC3COMPOSENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class Vec3ComposeNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Vec3Compose";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.5f, 0.6f, 0.4f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "R", PinType::Float);
        editor->AddInputPin(node, "G", PinType::Float);
        editor->AddInputPin(node, "B", PinType::Float);
        editor->AddOutputPin(node, "RGB", PinType::Vector);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        std::string varName = "var" + std::to_string(node->id);
        std::string r = editor->TraverseNode(node->inputs[0]->id, code);
        std::string g = editor->TraverseNode(node->inputs[1]->id, code);
        std::string b = editor->TraverseNode(node->inputs[2]->id, code);
        if (r.empty()) r = "0.0";
        if (g.empty()) g = "0.0";
        if (b.empty()) b = "0.0";
        code << "    vec3 " << varName << " = vec3(" << r << ", " << g << ", " << b << ");\n";
        return varName;
    }
};

#endif // VEC3COMPOSENODE_H
