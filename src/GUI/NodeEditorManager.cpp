//
// Created by Eduardo on 01/01/2026.
//
#include "NodeEditorManager.h"
#include <algorithm>
#include <imgui_node_editor.h>

NodeEditorManager::NodeEditorManager(const char* settingsFile)
    : m_Context(nullptr)
    , m_NextId(1)
    , m_ContextNodeId(0)
    , m_ContextLinkId(0)
    , m_ContextPinId(0)
    , m_ShowCreateMenu(false)
    , m_CreateMenuPos(0, 0)
{
    ed::Config config;
    config.SettingsFile = settingsFile;
    m_Context = ed::CreateEditor(&config);

    // Configurar estilo por defecto
    //SetupDefaultStyle();
}

// ==================== Gestión de Nodos ====================
std::shared_ptr<Node> NodeEditorManager::CreateNode(const std::string& name, ImVec4 color)
{
    auto node = std::make_shared<Node>(m_NextId++, name, color);
    m_Nodes.push_back(node);
    return node;
}

std::shared_ptr<Pin> NodeEditorManager::AddInputPin(std::shared_ptr<Node> node, const std::string& name, PinType type)
{
    auto pin = std::make_shared<Pin>(m_NextId++, node->id, ed::PinKind::Input, type, name);
    node->inputs.push_back(pin);
    m_Pins.push_back(pin);
    return pin;
}

std::shared_ptr<Pin> NodeEditorManager::AddOutputPin(std::shared_ptr<Node> node, const std::string& name, PinType type)
{
    auto pin = std::make_shared<Pin>(m_NextId++, node->id, ed::PinKind::Output, type, name);
    node->outputs.push_back(pin);
    m_Pins.push_back(pin);
    return pin;
}

void NodeEditorManager::DeleteNode(int nodeId)
{
    // Eliminar enlaces conectados
    m_Links.erase(
        std::remove_if(m_Links.begin(), m_Links.end(),
            [this, nodeId](const std::shared_ptr<Link>& link) {
                auto startPin = FindPin(link->startPinId);
                auto endPin = FindPin(link->endPinId);
                return (startPin && startPin->nodeId == nodeId) ||
                       (endPin && endPin->nodeId == nodeId);
            }),
        m_Links.end()
    );

    // Eliminar pines del nodo
    m_Pins.erase(
        std::remove_if(m_Pins.begin(), m_Pins.end(),
            [nodeId](const std::shared_ptr<Pin>& pin) {
                return pin->nodeId == nodeId;
            }),
        m_Pins.end()
    );

    // Eliminar nodo
    m_Nodes.erase(
        std::remove_if(m_Nodes.begin(), m_Nodes.end(),
            [nodeId](const std::shared_ptr<Node>& node) {
                return node->id == nodeId;
            }),
        m_Nodes.end()
    );
}

// ==================== Gestión de Enlaces ====================
std::shared_ptr<Link> NodeEditorManager::CreateLink(int startPinId, int endPinId)
{
    if (!CanCreateLink(startPinId, endPinId))
        return nullptr;

    auto link = std::make_shared<Link>(m_NextId++, startPinId, endPinId);
    m_Links.push_back(link);
    return link;
}

void NodeEditorManager::DeleteLink(int linkId)
{
    m_Links.erase(
        std::remove_if(m_Links.begin(), m_Links.end(),
            [linkId](const std::shared_ptr<Link>& link) {
                return link->id == linkId;
            }),
        m_Links.end()
    );
}

bool NodeEditorManager::CanCreateLink(int startPinId, int endPinId)
{
    if (startPinId == endPinId)
        return false;

    auto startPin = FindPin(startPinId);
    auto endPin = FindPin(endPinId);

    if (!startPin || !endPin)
        return false;

    // No conectar pines del mismo tipo (Input-Input u Output-Output)
    if (startPin->kind == endPin->kind)
        return false;

    // No conectar pines del mismo nodo
    if (startPin->nodeId == endPin->nodeId)
        return false;

    // Verificar tipos compatibles
    if (startPin->type != PinType::Any && endPin->type != PinType::Any) {
        if (startPin->type != endPin->type)
            return false;
    }

    // No permitir enlaces duplicados
    for (auto& link : m_Links) {
        if ((link->startPinId == startPinId && link->endPinId == endPinId) ||
            (link->startPinId == endPinId && link->endPinId == startPinId))
            return false;
    }

    return true;
}

// ==================== Búsqueda ====================
std::shared_ptr<Node> NodeEditorManager::FindNode(int nodeId)
{
    for (auto& node : m_Nodes) {
        if (node->id == nodeId)
            return node;
    }
    return nullptr;
}

std::shared_ptr<Pin> NodeEditorManager::FindPin(int pinId)
{
    for (auto& pin : m_Pins) {
        if (pin->id == pinId)
            return pin;
    }
    return nullptr;
}

std::shared_ptr<Link> NodeEditorManager::FindLink(int linkId)
{
    for (auto& link : m_Links) {
        if (link->id == linkId)
            return link;
    }
    return nullptr;
}

// ==================== Renderizado ====================
void NodeEditorManager::Render()
{
    ed::SetCurrentEditor(m_Context);
    ed::Begin("Node Editor", ImVec2(0, 0));

    // Renderizar nodos
    for (auto& node : m_Nodes) {
        RenderNode(node);
    }

    // Renderizar enlaces
    for (auto& link : m_Links) {
        ed::Link(link->id, link->startPinId, link->endPinId);
    }

    // Manejar creación de enlaces
    HandleLinkCreation();

    // Manejar eliminación
    HandleDeletion();

    // Manejar menús contextuales
    HandleContextMenus();

    ed::End();
    ed::SetCurrentEditor(nullptr);
}

void NodeEditorManager::RenderNode(std::shared_ptr<Node>& node)
{
    ed::PushStyleColor(ed::StyleColor_NodeBg, node->color);
    ed::BeginNode(node->id);

    ImGui::Text("%s", node->name.c_str());
    ImGui::Spacing();

    // Renderizar inputs
    for (auto& pin : node->inputs) {
        ed::BeginPin(pin->id, ed::PinKind::Input);
        ImGui::Text("-> %s", pin->name.c_str());
        ed::EndPin();
    }

    ImGui::Spacing();

    // Renderizar outputs
    for (auto& pin : node->outputs) {
        ed::BeginPin(pin->id, ed::PinKind::Output);
        ImGui::Text("%s ->", pin->name.c_str());
        ed::EndPin();
    }

    ed::EndNode();
    ed::PopStyleColor();
}

void NodeEditorManager::HandleLinkCreation()
{
    if (ed::BeginCreate()) {
        ed::PinId startPinId, endPinId;

        if (ed::QueryNewLink(&startPinId, &endPinId)) {
            if (startPinId && endPinId) {
                if (CanCreateLink(startPinId.Get(), endPinId.Get())) {
                    if (ed::AcceptNewItem()) {
                        CreateLink(startPinId.Get(), endPinId.Get());
                    }
                } else {
                    ed::RejectNewItem(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                }
            }
        }
    }
    ed::EndCreate();
}

void NodeEditorManager::HandleDeletion()
{
    if (ed::BeginDelete()) {
        // Eliminar enlaces
        ed::LinkId deletedLinkId;
        while (ed::QueryDeletedLink(&deletedLinkId)) {
            if (ed::AcceptDeletedItem()) {
                DeleteLink(deletedLinkId.Get());
            }
        }

        // Eliminar nodos
        ed::NodeId deletedNodeId;
        while (ed::QueryDeletedNode(&deletedNodeId)) {
            if (ed::AcceptDeletedItem()) {
                DeleteNode(deletedNodeId.Get());
            }
        }
    }
    ed::EndDelete();
}

void NodeEditorManager::HandleContextMenus()
{
    ed::Suspend();

    // Menú contextual de fondo (crear nodos)
    if (ed::ShowBackgroundContextMenu()) {
        ImGui::OpenPopup("Create Node Menu");
        m_CreateMenuPos = ImGui::GetMousePos();
    }

    // Menú contextual de nodo
    if (ed::ShowNodeContextMenu(&m_ContextNodeId)) {
        ImGui::OpenPopup("Node Context Menu");
    }

    // Menú contextual de enlace
    if (ed::ShowLinkContextMenu(&m_ContextLinkId)) {
        ImGui::OpenPopup("Link Context Menu");
    }

    // Renderizar popup de creación
    if (ImGui::BeginPopup("Create Node Menu")) {
        OnCreateNodeMenu();
        ImGui::EndPopup();
    }

    // Renderizar popup de nodo
    if (ImGui::BeginPopup("Node Context Menu")) {
        OnNodeContextMenu(m_ContextNodeId.Get());
        ImGui::EndPopup();
    }

    // Renderizar popup de enlace
    if (ImGui::BeginPopup("Link Context Menu")) {
        OnLinkContextMenu(m_ContextLinkId.Get());
        ImGui::EndPopup();
    }

    ed::Resume();
}

// ==================== Callbacks Virtuales ====================
void NodeEditorManager::OnCreateNodeMenu()
{
    if (ImGui::MenuItem("Input Node")) {
        auto node = CreateNode("Input", ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
        AddOutputPin(node, "Output", PinType::Float);
    }

    if (ImGui::MenuItem("Process Node")) {
        auto node = CreateNode("Process", ImVec4(0.2f, 0.4f, 0.8f, 1.0f));
        AddInputPin(node, "Input", PinType::Float);
        AddOutputPin(node, "Output", PinType::Float);
    }

    if (ImGui::MenuItem("Output Node")) {
        auto node = CreateNode("Output", ImVec4(0.8f, 0.3f, 0.2f, 1.0f));
        AddInputPin(node, "Input", PinType::Float);
    }
}

void NodeEditorManager::OnNodeContextMenu(int nodeId)
{
    if (ImGui::MenuItem("Delete Node")) {
        DeleteNode(nodeId);
    }

    if (ImGui::MenuItem("Duplicate Node")) {
        auto original = FindNode(nodeId);
        if (original) {
            auto copy = CreateNode(original->name + " Copy", original->color);

            // Copiar pines
            for (auto& pin : original->inputs) {
                AddInputPin(copy, pin->name, pin->type);
            }
            for (auto& pin : original->outputs) {
                AddOutputPin(copy, pin->name, pin->type);
            }
        }
    }
}

void NodeEditorManager::OnLinkContextMenu(int linkId)
{
    if (ImGui::MenuItem("Delete Link")) {
        DeleteLink(linkId);
    }
}

// ==================== Estilo ====================
void NodeEditorManager::SetupDefaultStyle()
{
    auto& style = ed::GetStyle();

    style.Colors[ed::StyleColor_Bg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ed::StyleColor_NodeBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    style.Colors[ed::StyleColor_NodeBorder] = ImVec4(0.4f, 0.4f, 0.4f, 1.0f);
    style.Colors[ed::StyleColor_PinRect] = ImVec4(0.3f, 0.6f, 0.9f, 1.0f);

    style.NodePadding = ImVec4(8, 4, 8, 4);
    style.NodeRounding = 4.0f;
    style.NodeBorderWidth = 2.0f;
    style.LinkStrength = 100.0f;
}

// ==================== Utilidades ====================
void NodeEditorManager::Clear()
{
    m_Nodes.clear();
    m_Links.clear();
    m_Pins.clear();
    m_NextId = 1;
}

void NodeEditorManager::NavigateToContent()
{
    ed::NavigateToContent();
}

void NodeEditorManager::NavigateToSelection()
{
    ed::NavigateToSelection();
}

ImVec2 NodeEditorManager::GetNodePosition(int nodeId)
{
    return ed::GetNodePosition(nodeId);
}

void NodeEditorManager::SetNodePosition(int nodeId, ImVec2 pos)
{
    ed::SetNodePosition(nodeId, pos);
}

NodeEditorManager::~NodeEditorManager()
{
    if (m_Context) {
        ed::DestroyEditor(m_Context);
        m_Context = nullptr;
    }
}
