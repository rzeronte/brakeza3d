#ifndef NODETYPE_H
#define NODETYPE_H

#include <string>
#include <memory>
#include <sstream>
#include "../imgui/imgui.h"
#include "../../../include/Misc/cJSON.h"

class ShaderNodeEditorManager;
struct Node;
struct Pin;

class NodeType {
public:
    virtual ~NodeType() = default;

    // Información básica del tipo de nodo
    virtual std::string GetTypeName() const = 0;
    virtual ImVec4 GetColor() const = 0;

    // Callbacks del ciclo de vida
    virtual void OnCreate(std::shared_ptr<Node>& node) {}
    virtual void OnDelete(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) {}

    // Parámetros editables del nodo (lo que vive en node->userData). Se guardan dentro del "userData"
    // del JSON del shader y se restauran al cargarlo. Los nodos sin parámetros no tienen que tocar nada.
    virtual void SerializeParams(const std::shared_ptr<Node>& node, cJSON* out) const {}
    virtual void DeserializeParams(const std::shared_ptr<Node>& node, const cJSON* in) {}

    // Configuración de pins
    virtual void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) = 0;

    // Renderizado de UI personalizada
    virtual float RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) { return 0.f; }

    // Generación de código shader
    virtual std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor
    ) = 0;

    virtual GUIType::Sheet GetIcon() const { return IconGUI::NODE_TYPE_DEFAULT; }

    float MeasureUIWidth()
    {
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();
        return max.x - min.x;
    }
};

#endif // NODETYPE_H