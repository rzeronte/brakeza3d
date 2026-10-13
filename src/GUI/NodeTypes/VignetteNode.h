#ifndef VIGNETTENODE_H
#define VIGNETTENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class VignetteNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Vignette";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.4f, 0.3f, 0.5f, 1.0f);
    }

    void OnCreate(std::shared_ptr<Node>& node) override {
        // Default: intensity=0.5, smoothness=0.5
        float* params = new float[2]{0.5f, 0.5f};
        node->userData = params;
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete[] (float*)node->userData;
            node->userData = nullptr;
        }
    }

    void SerializeParams(const std::shared_ptr<Node>& node, cJSON* out) const override {
        const float* params = (const float*)node->userData;
        if (!params) return;
        cJSON_AddNumberToObject(out, "intensity", params[0]);
        cJSON_AddNumberToObject(out, "smoothness", params[1]);
    }

    void DeserializeParams(const std::shared_ptr<Node>& node, const cJSON* in) override {
        float* params = (float*)node->userData;
        if (!params) return;
        params[0] = NodeJson::Float(in, "intensity", params[0]);
        params[1] = NodeJson::Float(in, "smoothness", params[1]);
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "UV", PinType::Vec2);
        editor->AddOutputPin(node, "Mask", PinType::Float);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        float* params = (float*)node->userData;
        if (!params) return 0.f;

        ImGui::BeginGroup();
        ImGui::PushID(node->id);

        ImGui::SetNextItemWidth(100);
        ImGui::SliderFloat("Intensity", &params[0], 0.0f, 2.0f);   // uniform: sin recompilar

        ImGui::SetNextItemWidth(100);
        ImGui::SliderFloat("Smoothness", &params[1], 0.0f, 1.0f);

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
        std::string varName = "var" + std::to_string(node->id);

        std::string uvCoords = "v_TexCoord";
        if (!node->inputs.empty()) {
            std::string customUV = editor->TraverseNode(node->inputs[0]->id, code);
            if (!customUV.empty()) {
                uvCoords = customUV + ".xy";
            }
        }

        float* params = (float*)node->userData;
        // x = intensity, y = smoothness
        const std::string p = params ? editor->AddUniform("u_p" + std::to_string(node->id) + "_vignette", 2, params) : "vec2(0.5, 0.5)";

        code << "    vec2 " << varName << "_uv = " << uvCoords << " - 0.5;\n";
        code << "    float " << varName << "_dist = length(" << varName << "_uv);\n";
        code << "    float " << varName << " = 1.0 - smoothstep((1.0 - (" << p << ").y) * 0.5, 0.5 + (" << p << ").y * 0.2, "
             << varName << "_dist * (" << p << ").x * 2.0);\n";

        return varName;
    }
};

#endif // VIGNETTENODE_H
