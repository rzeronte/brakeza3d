/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef ABSNODE_H
#define ABSNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class AbsNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Abs";
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
        code << "    float " << varName << " = abs(" << value << ");\n";
        return varName;
    }
};

#endif // ABSNODE_H
