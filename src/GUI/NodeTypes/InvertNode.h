/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef INVERTNODE_H
#define INVERTNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class InvertNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Invert";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.7f, 0.3f, 0.7f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "Color", PinType::Vector);
        editor->AddOutputPin(node, "Result", PinType::Vector);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        if (node->inputs.empty()) return "";

        std::string varName = "var" + std::to_string(node->id);

        std::string inputColor = editor->TraverseNode(node->inputs[0]->id, code);
        if (inputColor.empty()) inputColor = "vec3(0.0)";

        code << "    vec3 " << varName << " = vec3(1.0) - " << inputColor << ";\n";

        return varName;
    }
};

#endif // INVERTNODE_H
