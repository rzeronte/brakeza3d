#ifndef SHADERNODEEDITOR_H
#define SHADERNODEEDITOR_H

#include "NodeEditorManager.h"
#include <GL/glew.h>
#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <functional>
#include <glm/fwd.hpp>

#include "../Render/Image.h"

class ShaderNodeEditorManager;
class NodeType;

struct ImageTexture {
    Image* image = nullptr;
    std::string filepath;
};

// Parámetro de un nodo expuesto al shader como uniform: se lee del userData del nodo en cada dibujado,
// así que editarlo no obliga a recompilar el programa.
struct ShaderUniform {
    std::string name;
    int components = 1;
    const float* data = nullptr;
    GLint location = -1;
};

// Definición de un tipo de nodo (mantenida por compatibilidad)
struct NodeDefinition {
    std::string name;
    ImVec4 color;

    // Callbacks
    std::function<void(std::shared_ptr<Node>&)> onCreate;
    std::function<void(std::shared_ptr<Node>&)> onDelete;
    std::function<void(std::shared_ptr<Node>&, ShaderNodeEditorManager*)> renderUI;
    std::function<std::string(std::shared_ptr<Node>&, std::shared_ptr<Pin>&, std::stringstream&, ShaderNodeEditorManager*)> generateCode;
};

// Antes de la clase ShaderNodeEditorManager
enum class InternalTextureType {
    Color = 0,
    Depth = 1,
    // Normal = 2,    // Para futuro
    // Velocity = 3   // Para futuro
};

enum class MeshTextureType {
    Diffuse = 0,
    Specular = 1,
    Normal = 2,      // Para futuro
    Roughness = 3,   // Para futuro
    AO = 4           // Para futuro
};

class ShaderNodeEditorManager : public NodeEditorManager {
private:
    GLuint m_ShaderProgram;
    GLuint m_VAO;
    GLuint m_VBO;
    bool m_NeedsRecompile;
    bool m_FirstFrame;
    bool m_GLReady = false;         // el quad se crea en Update(), ya en el hilo principal
    bool m_DefaultLayout = false;   // true solo para el grafo por defecto recién creado
    std::string m_LastShaderCode;
    std::string m_LastCompileError;

    // Nombre de la "imagen" de los nodos Internal/Mesh Texture: la textura es del motor, no del nodo
    static constexpr const char* kSystemTexture = "[System Texture]";

    std::map<int, std::string> m_NodeOutputs;   // clave: id del pin de salida ya generado
    std::set<int> m_Traversing;                 // nodos en curso durante el recorrido (anti-ciclos)
    std::vector<ShaderUniform> m_Uniforms;
    // Funciones GLSL que piden los nodos (clave, código), EN EL ORDEN EN QUE SE PIDEN: GLSL exige declarar
    // antes de usar, y una función auxiliar puede depender de otra pedida antes (noiseFractal -> noiseValue)
    std::vector<std::pair<std::string, std::string>> m_GlslHelpers;
    std::map<int, ImageTexture> m_NodeTextures;

    // Sistema de registro de nodos
    std::map<std::string, NodeDefinition> m_NodeRegistry;
    std::map<std::string, NodeType*> m_NodeTypes;

    void SetupQuad();
    void SetInitialNodePositions();
    void HandleLinkCreation() override;
    void HandleDeletion() override;

    void CompileShader();

    void ReleaseAllNodes();
    void ReleaseNodeImage(ImageTexture& tex);
    int BindNodeTextures();
    void UnbindNodeTextures(int textureUnits);
    void UploadUniforms();
    void DuplicateNode(const std::shared_ptr<Node>& original);
    void AddNodeAtMenuPos(const std::string& typeName);

protected:
    void OnNodeContextMenu(int nodeId) override;
    void OnLinkContextMenu(int linkId) override;

private:

    // Métodos para registrar nodos
    std::shared_ptr<Node> CreateNodeOfType(const std::string& typeName);

public:
    ShaderNodeEditorManager(ShaderCustomType type);
    ~ShaderNodeEditorManager() override;

    void CreateDefaultNodes();
    std::string GenerateShaderCode();
    std::string TraverseNode(int pinId, std::stringstream& code);
    void RegisterNodeTypes();
    void RegisterNodeType(NodeType* nodeType);
    void Render() override;
    void DeleteNode(int nodeId) override;

    // Parámetro de nodo como uniform (devuelve el nombre GLSL a usar en el código generado).
    // `data` debe seguir vivo mientras exista el nodo (normalmente su userData).
    std::string AddUniform(const std::string& name, int components, const float* data);

    // Función GLSL auxiliar (se declara una sola vez, antes de main(), aunque la pidan varios nodos)
    void AddGlslHelper(const std::string& key, const std::string& glslCode);

    [[nodiscard]] const std::string& GetLastCompileError() const { return m_LastCompileError; }

    void DrawNodeHeader(std::shared_ptr<Node> &node);

    void RenderNode(std::shared_ptr<Node>& node) override;
    void Update();
    void RenderEffect(GLuint fb);
    void OnCreateNodeMenu() override;
    void Clear() override;  // ← Añadir esta línea

    [[nodiscard]] GLuint GetShaderProgram() const;
    [[nodiscard]] const std::string& GetLastShaderCode() const;

    // Helpers públicos para callbacks y nodos
    void SetNeedsRecompile() { m_NeedsRecompile = true; }
    std::map<int, ImageTexture>& GetNodeTextures() { return m_NodeTextures; }

    // Métodos para gestión de imágenes (usados por ImageNode)
    void LoadImageForNode(int nodeId, const std::string& filepath);
    void DeleteImageForNode(int nodeId);

    // Sobrescribir métodos de serialización
    cJSON* SerializeNodeUserData(std::shared_ptr<Node> node) override;
    void DeserializeNodeUserData(std::shared_ptr<Node> node, cJSON* userDataJson) override;
    void SerializeCustomData(cJSON* root) override;
    void DeserializeCustomData(cJSON* root) override;
    std::shared_ptr<Node> CreateNodeFromJSON(cJSON* nodeJson) override;

    void SetExternalTextureForNode(int nodeId, GLuint textureId);
    void PlugSystemTexture(std::shared_ptr<Node> &node, GLuint systemTextureId);
    // Actualizar textura de un nodo específico (cada frame)
    void UpdateExternalTextureForNode(int nodeId, GLuint textureId);
    // O mejor: actualizar automáticamente todos los ChainOutput
    void UpdateChainOutputTexture(GLuint textureId);
    void UpdateInternalTextures(GLuint colorTexture, GLuint depthTexture);

    void UpdateMeshTextures(GLuint diffuseTexture, GLuint specularTexture);

    void RenderMesh(
        GLuint fb,
        GLuint vertexBuffer,
        GLuint uvBuffer,
        GLuint normalBuffer,
        int vertexCount,
        const glm::mat4& model,
        const glm::mat4& view,
        const glm::mat4& projection,
        GLuint indexBuffer = 0,
        GLsizei indexCount = 0
    );

    void RenderShaderDebugPanel();
};

#endif //SHADERNODEEDITOR_H