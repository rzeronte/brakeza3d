#ifndef FLOATVALUENODE_H
#define FLOATVALUENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class FloatValueNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "FloatValue";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.5f, 0.5f, 0.3f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        node->userData = new float(1.0f);
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete (float*)node->userData;
            node->userData = nullptr;
        }
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddOutputPin(node, "Value", PinType::Float);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        ImGui::BeginGroup();
        if (node->userData) {
            float* value = (float*)node->userData;
            std::string valueId = "##float" + std::to_string(node->id);
            ImGui::SetNextItemWidth(80);
            if (ImGui::DragFloat(valueId.c_str(), value, 0.01f, -100.0f, 100.0f, "%.3f")) {
                editor->SetNeedsRecompile();
            }
        }
        ImGui::EndGroup();
        return MeasureUIWidth();
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        std::string varName = "var" + std::to_string(node->id);
        float* value = (float*)node->userData;

        if (value) {
            code << "    float " << varName << " = " << *value << ";\n";
        } else {
            code << "    float " << varName << " = 1.0;\n";
        }

        return varName;
    }
};

#endif // FLOATVALUENODE_H
