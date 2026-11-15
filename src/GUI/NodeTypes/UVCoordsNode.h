/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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
        editor->AddOutputPin(node, "UV", PinType::Vec2);
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