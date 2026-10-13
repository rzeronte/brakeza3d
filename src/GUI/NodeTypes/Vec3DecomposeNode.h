#ifndef VEC3DECOMPOSENODE_H
#define VEC3DECOMPOSENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class Vec3DecomposeNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Vec3Decompose";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.5f, 0.6f, 0.4f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "RGB", PinType::Vector);
        editor->AddOutputPin(node, "R", PinType::Float);
        editor->AddOutputPin(node, "G", PinType::Float);
        editor->AddOutputPin(node, "B", PinType::Float);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        std::string input = editor->TraverseNode(node->inputs[0]->id, code);
        if (input.empty()) input = "vec3(0.0)";

        // Cada salida se genera por separado (el gestor cachea por pin)
        std::string channel = "r";
        if (sourcePin && sourcePin->name == "G") channel = "g";
        else if (sourcePin && sourcePin->name == "B") channel = "b";

        const std::string varName = "var" + std::to_string(node->id) + "_" + channel;
        code << "    float " << varName << " = (" << input << ")." << channel << ";\n";
        return varName;
    }
};

#endif // VEC3DECOMPOSENODE_H
