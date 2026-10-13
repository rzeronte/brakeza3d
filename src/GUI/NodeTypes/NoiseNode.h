#ifndef NOISENODE_H
#define NOISENODE_H

#include "NodeType.h"
#include "../ShaderNodeEditorManager.h"
#include "../../../include/GUI/NodeEditorManager.h"

class NoiseNode : public NodeType {
public:
    std::string GetTypeName() const override {
        return "Noise";
    }

    ImVec4 GetColor() const override {
        return ImVec4(0.4f, 0.4f, 0.5f, 1.0f);
    }

    // userData: {scale, fractal}. `scale` va primero para poder pasarlo como uniform.
    struct Params {
        float scale = 8.0f;
        int fractal = 0;
    };

    void OnCreate(std::shared_ptr<Node>& node) override {
        node->userData = new Params();
    }

    void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        if (node->userData) {
            delete (Params*)node->userData;
            node->userData = nullptr;
        }
    }

    void SerializeParams(const std::shared_ptr<Node>& node, cJSON* out) const override {
        const Params* params = (const Params*)node->userData;
        if (!params) return;
        cJSON_AddNumberToObject(out, "scale", params->scale);
        cJSON_AddNumberToObject(out, "fractal", params->fractal);
    }

    void DeserializeParams(const std::shared_ptr<Node>& node, const cJSON* in) override {
        Params* params = (Params*)node->userData;
        if (!params) return;
        params->scale = NodeJson::Float(in, "scale", params->scale);
        params->fractal = NodeJson::Int(in, "fractal", params->fractal) != 0 ? 1 : 0;
    }

    void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        editor->AddInputPin(node, "UV", PinType::Vec2);
        editor->AddOutputPin(node, "Value", PinType::Float);
    }

    float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) override {
        Params* params = (Params*)node->userData;
        if (!params) return 0.f;

        ImGui::BeginGroup();
        ImGui::PushID(node->id);
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("Scale", &params->scale, 0.1f, 0.1f, 500.0f);   // uniform: sin recompilar
        bool fractal = params->fractal != 0;
        if (ImGui::Checkbox("Fractal (4 octaves)", &fractal)) {
            params->fractal = fractal ? 1 : 0;
            editor->SetNeedsRecompile();   // cambia la función GLSL usada
        }
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
        std::string uv = editor->TraverseNode(node->inputs[0]->id, code);
        if (uv.empty()) uv = "v_TexCoord";

        Params* params = (Params*)node->userData;
        const bool fractal = params && params->fractal != 0;
        std::string scale = params ? editor->AddUniform("u_p" + std::to_string(node->id) + "_scale", 1, &params->scale) : "8.0";

        // Funciones auxiliares (se declaran una sola vez aunque haya varios nodos Noise)
        editor->AddGlslHelper("noise_value",
            "float noiseHash(vec2 p) {\n"
            "    p = fract(p * vec2(123.34, 456.21));\n"
            "    p += dot(p, p + 45.32);\n"
            "    return fract(p.x * p.y);\n"
            "}\n"
            "float noiseValue(vec2 p) {\n"
            "    vec2 i = floor(p);\n"
            "    vec2 f = fract(p);\n"
            "    f = f * f * (3.0 - 2.0 * f);\n"
            "    float a = noiseHash(i);\n"
            "    float b = noiseHash(i + vec2(1.0, 0.0));\n"
            "    float c = noiseHash(i + vec2(0.0, 1.0));\n"
            "    float d = noiseHash(i + vec2(1.0, 1.0));\n"
            "    return mix(mix(a, b, f.x), mix(c, d, f.x), f.y);\n"
            "}\n");

        if (fractal) {
            editor->AddGlslHelper("noise_fractal",
                "float noiseFractal(vec2 p) {\n"
                "    float sum = 0.0;\n"
                "    float amplitude = 0.5;\n"
                "    for (int i = 0; i < 4; i++) {\n"
                "        sum += amplitude * noiseValue(p);\n"
                "        p *= 2.0;\n"
                "        amplitude *= 0.5;\n"
                "    }\n"
                "    return sum / 0.9375;\n"
                "}\n");
        }

        code << "    float " << varName << " = " << (fractal ? "noiseFractal" : "noiseValue")
             << "(" << uv << " * " << scale << ");\n";
        return varName;
    }
};

#endif // NOISENODE_H
