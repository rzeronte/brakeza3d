/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef GRAYSCALENODE_H
#define GRAYSCALENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class GrayscaleNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Grayscale";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "Color", PinType::Vector);
        editor->AddOutputPin(node, "Gray", PinType::Float);
        editor->AddOutputPin(node, "RGB", PinType::Vector);
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

        // Luminance weights (Rec. 709)
        const std::string luminance = "dot(" + inputColor + ", vec3(0.2126, 0.7152, 0.0722))";

        // El gestor pide cada salida por separado y cachea por pin: se genera solo la solicitada
        if (sourcePin && sourcePin->name == "RGB") {
            const std::string varNameRGB = varName + "_rgb";
            code << "    vec3 " << varNameRGB << " = vec3(" << luminance << ");\n";
            return varNameRGB;
        }

        code << "    float " << varName << " = " << luminance << ";\n";
        return varName;
    }
};

#endif // GRAYSCALENODE_H
