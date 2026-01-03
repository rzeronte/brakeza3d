//
// Created by Eduardo on 31/12/2025.
//

#ifndef BRAKEZA3D_NODEEDITORMANAGER_H
#define BRAKEZA3D_NODEEDITORMANAGER_H

#include <imgui.h>
#include <imgui_node_editor.h>
#include <vector>
#include <string>
#include <memory>

#include "../../include/Misc/cJSON.h"

namespace ed = ax::NodeEditor;

// Enums y estructuras
enum class PinType {
    Flow,
    Float,
    Int,
    Bool,
    String,
    Vector,
    Any
};

struct Pin {
    int id;
    int nodeId;
    ed::PinKind kind;
    PinType type;
    std::string name;

    Pin(
        int _id,
        int _nodeId,
        ed::PinKind _kind,
        PinType _type,
        const std::string& _name
    )
    :
        id(_id), nodeId(_nodeId), kind(_kind), type(_type), name(_name)
    {}
};

struct Node {
    int id;
    std::string name;
    std::string type;
    ImVec4 color;
    ImVec2 size;
    std::vector<std::shared_ptr<Pin>> inputs;
    std::vector<std::shared_ptr<Pin>> outputs;
    void* userData; // Para datos personalizados

    Node(
        int _id,
        const std::string& _name,
        const std::string& _type = "base",
        ImVec4 _color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f))
    :
        id(_id), name(_name), type(_type), color(_color), size(ImVec2(0, 0)), userData(nullptr)
    {}
};

struct Link {
    int id;
    int startPinId;
    int endPinId;

    Link(int _id, int _start, int _end)
        : id(_id), startPinId(_start), endPinId(_end) {}
};

class NodeEditorManager {
protected:
    ed::EditorContext* m_Context;
    std::vector<std::shared_ptr<Node>> m_Nodes;
    std::vector<std::shared_ptr<Link>> m_Links;
    std::vector<std::shared_ptr<Pin>> m_Pins;
    int m_NextId;

    // Estado de UI
    ed::NodeId m_ContextNodeId;
    ed::LinkId m_ContextLinkId;
    ed::PinId m_ContextPinId;
    bool m_ShowCreateMenu;
    ImVec2 m_CreateMenuPos;

public:
    NodeEditorManager(const char* settingsFile = "NodeEditor.json");
    virtual ~NodeEditorManager();

    // ==================== Gestión de Nodos ====================
    std::shared_ptr<Node> CreateNode(const std::string& name, ImVec4 color = ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
    std::shared_ptr<Pin> AddInputPin(std::shared_ptr<Node> node, const std::string& name, PinType type = PinType::Any);
    std::shared_ptr<Pin> AddOutputPin(std::shared_ptr<Node> node, const std::string& name, PinType type = PinType::Any);
    virtual void DeleteNode(int nodeId);
    // ==================== Gestión de Enlaces ====================
    std::shared_ptr<Link> CreateLink(int startPinId, int endPinId);
    void DeleteLink(int linkId);
    bool CanCreateLink(int startPinId, int endPinId);
    // ==================== Búsqueda ====================
    std::shared_ptr<Node> FindNode(int nodeId);
    std::shared_ptr<Pin> FindPin(int pinId);
    std::shared_ptr<Link> FindLink(int linkId);
    // ==================== Renderizado ====================
    virtual void Render();
protected:
    virtual void RenderNode(std::shared_ptr<Node>& node);
    virtual void HandleLinkCreation();
    virtual void HandleDeletion();
    void HandleContextMenus();
    // ==================== Callbacks Virtuales ====================
    virtual void OnCreateNodeMenu();
    virtual void OnNodeContextMenu(int nodeId);
    virtual void OnLinkContextMenu(int linkId);
    // ==================== Estilo ====================
    void SetupDefaultStyle();
public:
    // ==================== Getters ====================
    [[nodiscard]] const std::vector<std::shared_ptr<Node>>& GetNodes() const      { return m_Nodes; }
    [[nodiscard]] const std::vector<std::shared_ptr<Link>>& GetLinks() const      { return m_Links; }
    [[nodiscard]] const std::vector<std::shared_ptr<Pin>>& GetPins() const        { return m_Pins; }
    // ==================== Utilidades ====================
    void Clear();
    void NavigateToContent();
    void NavigateToSelection();
    ImVec2 GetNodePosition(int nodeId);
    void SetNodePosition(int nodeId, ImVec2 pos);

    // Serialización
    virtual cJSON* getJSONTypes();
    virtual bool loadFromJSON(cJSON* json);
    bool SaveToFile(const char* filename);
    bool LoadFromFile(const char* filename);

    // Hooks virtuales para clases hijas
    virtual cJSON* SerializeNodeUserData(std::shared_ptr<Node> node);
    virtual void DeserializeNodeUserData(std::shared_ptr<Node> node, cJSON* userDataJson);
    virtual void SerializeCustomData(cJSON* root);
    virtual void DeserializeCustomData(cJSON* root);

    // Hook post-carga (para recrear nodos con CreateNodeOfType, etc)
    virtual std::shared_ptr<Node> CreateNodeFromJSON(cJSON* nodeJson);
};

#endif //BRAKEZA3D_NODEEDITORMANAGER_H