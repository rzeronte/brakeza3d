#ifndef NODETYPE_H
#define NODETYPE_H

#include <string>
#include <memory>
#include <sstream>
#include "../imgui/imgui.h"

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

    // Configuración de pins
    virtual void SetupPins(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) = 0;

    // Renderizado de UI personalizada
    virtual void RenderUI(std::shared_ptr<Node>& node, ShaderNodeEditorManager* editor) {}

    // Generación de código shader
    virtual std::string GenerateCode(
        std::shared_ptr<Node>& node,
        std::shared_ptr<Pin>& sourcePin,
        std::stringstream& code,
        ShaderNodeEditorManager* editor
    ) = 0;
};

#endif // NODETYPE_H