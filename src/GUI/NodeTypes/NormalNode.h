/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef NORMALNODE_H
#define NORMALNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class NormalNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Normal";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.3f, 0.5f, 0.6f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddOutputPin(node, "Normal", PinType::Vector);
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        // v_Normal solo existe en los shaders de objeto; el menú únicamente ofrece este nodo ahí
        std::string varName = "var" + std::to_string(node->id);
        code << "    vec3 " << varName << " = normalize(v_Normal);\n";
        return varName;
    }
};

#endif // NORMALNODE_H
