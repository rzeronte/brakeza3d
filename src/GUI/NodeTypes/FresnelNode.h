#ifndef FRESNELNODE_H
#define FRESNELNODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class FresnelNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Fresnel";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.3f, 0.6f, 0.6f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        node->userData = new float(3.0f);   // potencia
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete (float*)node->userData;
            node->userData = nullptr;
        }
    }

    void SerializeParams(const std::shared_ptr<Node>& node, cJSON* out) const override {
        const float* power = (const float*)node->userData;
        if (power) cJSON_AddNumberToObject(out, "power", *power);
    }

    void DeserializeParams(const std::shared_ptr<Node>& node, const cJSON* in) override {
        float* power = (float*)node->userData;
        if (power) *power = NodeJson::Float(in, "power", *power);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddOutputPin(node, "Fresnel", PinType::Float);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        float* power = (float*)node->userData;
        if (!power) return 0.f;

        ImGui::BeginGroup();
        ImGui::PushID(node->id);
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("Power", power, 0.05f, 0.1f, 16.0f);   // uniform: sin recompilar
        ImGui::PopID();
        ImGui::EndGroup();
        return MeasureUIWidth();
    }

    std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor) override
    {
        // 0 mirando la superficie de frente, 1 en el borde de la silueta. Solo shaders de objeto
        // (usa v_Normal, v_FragPos y u_ViewPos).
        std::string varName = "var" + std::to_string(node->id);
        float* power = (float*)node->userData;
        std::string p = power ? editor->AddUniform("u_p" + std::to_string(node->id) + "_power", 1, power) : "3.0";

        code << "    float " << varName << " = pow(1.0 - max(dot(normalize(v_Normal), normalize(u_ViewPos - v_FragPos)), 0.0), max(" << p << ", 0.01));\n";
        return varName;
    }
};

#endif // FRESNELNODE_H
