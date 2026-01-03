#ifndef MIXNODE_H
#define MIXNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditor.h"
#include "../NodeEditorManager.h"

class MixNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Mix";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.4f, 0.4f, 0.7f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        node->userData = new float(0.5f);
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditor* editor) override {
        if (node->userData) {
            delete (float*)node->userData;
            node->userData = nullptr;
        }
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditor* editor) override {
        editor->AddInputPin(node, "A", PinType::Vector);
        editor->AddInputPin(node, "B", PinType::Vector);
        editor->AddInputPin(node, "Factor", PinType::Float);
        editor->AddOutputPin(node, "Result", PinType::Vector);
    }

    void RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditor* editor) override {
        if (node->userData) {
            float* factor = (float*)node->userData;
            if (ImGui::SliderFloat("Factor", factor, 0.0f, 1.0f)) {
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
        if (node->inputs.size() < 3) {
            return "";
        }

        std::string varName = "var" + std::to_string(node->id);

        std::string a = editor->TraverseNode(node->inputs[0]->id, code);
        std::string b = editor->TraverseNode(node->inputs[1]->id, code);
        std::string factor = editor->TraverseNode(node->inputs[2]->id, code);

        if (a.empty()) a = "vec3(0.0)";
        if (b.empty()) b = "vec3(1.0)";
        if (factor.empty()) {
            float* f = (float*)node->userData;
            factor = std::to_string(f ? *f : 0.5f);
        }

        code << "    vec3 " << varName << " = mix(" << a << ", " << b << ", " << factor << ");\n";

        return varName;
    }
};

#endif // MIXNODE_H