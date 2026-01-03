#ifndef SHADERNODEEDITOR_H
#define SHADERNODEEDITOR_H

#include "NodeEditorManager.h"
#include <GL/glew.h>
#include <string>
#include <sstream>
#include <map>
#include <functional>

#include "../../include/Render/Image.h"

class ShaderNodeEditor;
class NodeType;

struct ImageTexture {
    Image* image = nullptr;
    std::string filepath;
};

// Definición de un tipo de nodo (mantenida por compatibilidad)
struct NodeDefinition {
    std::string name;
    ImVec4 color;

    // Callbacks
    std::function<void(std::shared_ptr<Node>&)> onCreate;
    std::function<void(std::shared_ptr<Node>&)> onDelete;
    std::function<void(std::shared_ptr<Node>&, ShaderNodeEditor*)> renderUI;
    std::function<std::string(std::shared_ptr<Node>&, std::shared_ptr<Pin>&, std::stringstream&, ShaderNodeEditor*)> generateCode;
};

class ShaderNodeEditor : public NodeEditorManager {
private:
    GLuint m_ShaderProgram;
    GLuint m_VAO;
    GLuint m_VBO;
    bool m_NeedsRecompile;
    bool m_FirstFrame;
    std::string m_LastShaderCode;

    std::map<int, std::string> m_NodeOutputs;
    std::map<int, ImageTexture> m_NodeTextures;

    // Sistema de registro de nodos
    std::map<std::string, NodeDefinition> m_NodeRegistry;
    std::map<std::string, NodeType*> m_NodeTypes;

    void SetupQuad();
    void CreateDefaultNodes();
    void SetInitialNodePositions();
    void HandleLinkCreation();
    void HandleDeletion();

    void CompileShader();

    // Métodos para registrar nodos
    std::shared_ptr<Node> CreateNodeOfType(const std::string& typeName);

public:
    ShaderNodeEditor();
    ~ShaderNodeEditor() override;

    std::string GenerateShaderCode();
    std::string TraverseNode(int pinId, std::stringstream& code);
    void RegisterNodeTypes();
    void RegisterNodeType(NodeType* nodeType);
    void Render() override;
    void RenderNode(std::shared_ptr<Node>& node) override;
    void Update();
    void RenderEffect();
    void OnCreateNodeMenu() override;

    [[nodiscard]] GLuint GetShaderProgram() const;
    [[nodiscard]] const std::string& GetLastShaderCode() const;

    // Helpers públicos para callbacks y nodos
    void SetNeedsRecompile() { m_NeedsRecompile = true; }
    std::map<int, ImageTexture>& GetNodeTextures() { return m_NodeTextures; }

    // Métodos para gestión de imágenes (usados por ImageNode)
    void LoadImageForNode(int nodeId, const std::string& filepath);
    void DeleteImageForNode(int nodeId);
};

#endif //SHADERNODEEDITOR_H