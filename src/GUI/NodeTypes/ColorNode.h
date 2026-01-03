#ifndef COLORNODE_H
#define COLORNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditor.h"
#include "../NodeEditorManager.h"

class ColorNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Color";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.8f, 0.4f, 0.2f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        node->userData = new ImVec4(1.0f, 0.5f, 0.2f, 1.0f);
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditor* editor) override {
        if (node->userData) {
            delete (ImVec4*)node->userData;
            node->userData = nullptr;
        }
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditor* editor) override {
        editor->AddOutputPin(node, "RGB", PinType::Vector);
    }

    void RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditor* editor) override {
        if (node->userData) {
            ImVec4* color = (ImVec4*)node->userData;
            std::string colorId = "##color" + std::to_string(node->id);
            if (ImGui::ColorEdit3(colorId.c_str(), (float*)color)) {
                editor->SetNeedsRecompile();
            }
        }
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditor* editor) override
    {
        std::string varName = "var" + std::to_string(node->id);
        ImVec4* color = (ImVec4*)node->userData;

        if (color) {
            code << "    vec3 " << varName << " = vec3("
                 << color->x << ", " << color->y << ", " << color->z << ");\n";
        } else {
            code << "    vec3 " << varName << " = vec3(1.0, 1.0, 1.0);\n";
        }

        return varName;
    }
};

#endif // COLORNODE_H