//
// Created by Eduardo on 01/01/2026.
//

#include "NodeEditorManager.h"
#include <algorithm>
#include <imgui_node_editor.h>
#include <fstream>

#include "../../include/Misc/Tools.h"

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
    auto node = std::make_shared<Node>(m_NextId++, name, "base", color);
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

cJSON* NodeEditorManager::getJSONTypes() {
    cJSON* root = cJSON_CreateObject();

    // Versión del formato
    cJSON_AddStringToObject(root, "version", "1.0");
    cJSON_AddStringToObject(root, "editorType", "base");

    // ========== NODOS ==========
    cJSON* nodesArray = cJSON_CreateArray();
    for (auto& node : m_Nodes) {
        cJSON* nodeObj = cJSON_CreateObject();

        cJSON_AddNumberToObject(nodeObj, "id", node->id);
        cJSON_AddStringToObject(nodeObj, "name", node->name.c_str());
        cJSON_AddStringToObject(nodeObj, "type", node->type.c_str());

        // Posición del nodo
        ImVec2 pos = ed::GetNodePosition(node->id);
        cJSON* posObj = cJSON_CreateObject();
        cJSON_AddNumberToObject(posObj, "x", pos.x);
        cJSON_AddNumberToObject(posObj, "y", pos.y);
        cJSON_AddItemToObject(nodeObj, "position", posObj);

        // Color
        cJSON* colorObj = cJSON_CreateObject();
        cJSON_AddNumberToObject(colorObj, "r", node->color.x);
        cJSON_AddNumberToObject(colorObj, "g", node->color.y);
        cJSON_AddNumberToObject(colorObj, "b", node->color.z);
        cJSON_AddNumberToObject(colorObj, "a", node->color.w);
        cJSON_AddItemToObject(nodeObj, "color", colorObj);

        // Inputs
        cJSON* inputsArray = cJSON_CreateArray();
        for (auto& pin : node->inputs) {
            cJSON* pinObj = cJSON_CreateObject();
            cJSON_AddNumberToObject(pinObj, "id", pin->id);
            cJSON_AddStringToObject(pinObj, "name", pin->name.c_str());
            cJSON_AddNumberToObject(pinObj, "type", static_cast<int>(pin->type));
            cJSON_AddItemToArray(inputsArray, pinObj);
        }
        cJSON_AddItemToObject(nodeObj, "inputs", inputsArray);

        // Outputs
        cJSON* outputsArray = cJSON_CreateArray();
        for (auto& pin : node->outputs) {
            cJSON* pinObj = cJSON_CreateObject();
            cJSON_AddNumberToObject(pinObj, "id", pin->id);
            cJSON_AddStringToObject(pinObj, "name", pin->name.c_str());
            cJSON_AddNumberToObject(pinObj, "type", static_cast<int>(pin->type));
            cJSON_AddItemToArray(outputsArray, pinObj);
        }
        cJSON_AddItemToObject(nodeObj, "outputs", outputsArray);

        // UserData (virtual, para que las hijas lo extiendan)
        cJSON* userData = SerializeNodeUserData(node);
        if (userData) {
            cJSON_AddItemToObject(nodeObj, "userData", userData);
        }

        cJSON_AddItemToArray(nodesArray, nodeObj);
    }
    cJSON_AddItemToObject(root, "nodes", nodesArray);

    // ========== LINKS ==========
    cJSON* linksArray = cJSON_CreateArray();
    for (auto& link : m_Links) {
        cJSON* linkObj = cJSON_CreateObject();
        cJSON_AddNumberToObject(linkObj, "id", link->id);
        cJSON_AddNumberToObject(linkObj, "startPinId", link->startPinId);
        cJSON_AddNumberToObject(linkObj, "endPinId", link->endPinId);
        cJSON_AddItemToArray(linksArray, linkObj);
    }
    cJSON_AddItemToObject(root, "links", linksArray);

    // ========== DATOS CUSTOM (para clases hijas) ==========
    SerializeCustomData(root);

    return root;
}

bool NodeEditorManager::loadFromJSON(cJSON* json) {
    if (!json) return false;

    // Limpiar estado actual
    Clear();

    // Verificar versión
    cJSON* version = cJSON_GetObjectItem(json, "version");
    if (!version || strcmp(version->valuestring, "1.0") != 0) {
        return false;
    }

    // ========== CARGAR NODOS ==========
    cJSON* nodesArray = cJSON_GetObjectItem(json, "nodes");
    if (nodesArray) {
        cJSON* nodeJson = nullptr;
        cJSON_ArrayForEach(nodeJson, nodesArray) {
            // Permitir que la clase hija cree el nodo (importante para ShaderNodeEditor)
            std::shared_ptr<Node> node = CreateNodeFromJSON(nodeJson);

            if (node) {
                // Establecer posición
                cJSON* posObj = cJSON_GetObjectItem(nodeJson, "position");
                if (posObj) {
                    float x = cJSON_GetObjectItem(posObj, "x")->valuedouble;
                    float y = cJSON_GetObjectItem(posObj, "y")->valuedouble;
                    ed::SetNodePosition(node->id, ImVec2(x, y));
                }

                // UserData (virtual)
                cJSON* userData = cJSON_GetObjectItem(nodeJson, "userData");
                if (userData) {
                    DeserializeNodeUserData(node, userData);
                }
            }
        }
    }

    // ========== CARGAR LINKS ==========
    cJSON* linksArray = cJSON_GetObjectItem(json, "links");
    if (linksArray) {
        cJSON* linkJson = nullptr;
        cJSON_ArrayForEach(linkJson, linksArray) {
            int id = cJSON_GetObjectItem(linkJson, "id")->valueint;
            int startPin = cJSON_GetObjectItem(linkJson, "startPinId")->valueint;
            int endPin = cJSON_GetObjectItem(linkJson, "endPinId")->valueint;

            // Crear el link
            auto link = std::make_shared<Link>(id, startPin, endPin);
            m_Links.push_back(link);

            // Actualizar m_NextId si es necesario
            if (id >= m_NextId) {
                m_NextId = id + 1;
            }
        }
    }

    // ========== DATOS CUSTOM ==========
    DeserializeCustomData(json);

    return true;
}

// Implementación por defecto (puede ser sobrescrita)
std::shared_ptr<Node> NodeEditorManager::CreateNodeFromJSON(cJSON* nodeJson) {
    int id = cJSON_GetObjectItem(nodeJson, "id")->valueint;
    std::string name = cJSON_GetObjectItem(nodeJson, "name")->valuestring;
    std::string type = cJSON_GetObjectItem(nodeJson, "type")->valuestring;

    // Color
    cJSON* colorObj = cJSON_GetObjectItem(nodeJson, "color");
    ImVec4 color(
        cJSON_GetObjectItem(colorObj, "r")->valuedouble,
        cJSON_GetObjectItem(colorObj, "g")->valuedouble,
        cJSON_GetObjectItem(colorObj, "b")->valuedouble,
        cJSON_GetObjectItem(colorObj, "a")->valuedouble
    );

    // Crear nodo base
    auto node = std::make_shared<Node>(id, name, type, color);

    // Actualizar m_NextId
    if (id >= m_NextId) {
        m_NextId = id + 1;
    }

    // Recrear inputs
    cJSON* inputsArray = cJSON_GetObjectItem(nodeJson, "inputs");
    if (inputsArray) {
        cJSON* pinJson = nullptr;
        cJSON_ArrayForEach(pinJson, inputsArray) {
            int pinId = cJSON_GetObjectItem(pinJson, "id")->valueint;
            std::string pinName = cJSON_GetObjectItem(pinJson, "name")->valuestring;
            PinType pinType = static_cast<PinType>(cJSON_GetObjectItem(pinJson, "type")->valueint);

            auto pin = std::make_shared<Pin>(pinId, node->id, ed::PinKind::Input, pinType, pinName);
            node->inputs.push_back(pin);
            m_Pins.push_back(pin);

            if (pinId >= m_NextId) {
                m_NextId = pinId + 1;
            }
        }
    }

    // Recrear outputs
    cJSON* outputsArray = cJSON_GetObjectItem(nodeJson, "outputs");
    if (outputsArray) {
        cJSON* pinJson = nullptr;
        cJSON_ArrayForEach(pinJson, outputsArray) {
            int pinId = cJSON_GetObjectItem(pinJson, "id")->valueint;
            std::string pinName = cJSON_GetObjectItem(pinJson, "name")->valuestring;
            PinType pinType = static_cast<PinType>(cJSON_GetObjectItem(pinJson, "type")->valueint);

            auto pin = std::make_shared<Pin>(pinId, node->id, ed::PinKind::Output, pinType, pinName);
            node->outputs.push_back(pin);
            m_Pins.push_back(pin);

            if (pinId >= m_NextId) {
                m_NextId = pinId + 1;
            }
        }
    }

    m_Nodes.push_back(node);
    return node;
}

// Métodos virtuales por defecto (vacíos)
cJSON* NodeEditorManager::SerializeNodeUserData(std::shared_ptr<Node> node) {
    return nullptr;
}

void NodeEditorManager::DeserializeNodeUserData(std::shared_ptr<Node> node, cJSON* userDataJson) {
    // Por defecto no hace nada
}

void NodeEditorManager::SerializeCustomData(cJSON* root) {
    // Por defecto no añade nada
}

void NodeEditorManager::DeserializeCustomData(cJSON* root) {
    // Por defecto no hace nada
}

// Utilidades para guardar/cargar archivos
bool NodeEditorManager::SaveToFile(const char* filename) {
    cJSON* json = getJSONTypes();
    if (!json) return false;

    char* jsonString = cJSON_Print(json);

    std::ofstream file(filename);
    if (!file.is_open()) {
        cJSON_Delete(json);
        free(jsonString);
        return false;
    }

    file << jsonString;
    file.close();

    cJSON_Delete(json);
    free(jsonString);

    return true;
}

bool NodeEditorManager::LoadFromFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string content = Tools::ReadFile(filename);

    cJSON* json = cJSON_Parse(content.c_str());
    if (!json) return false;

    bool result = loadFromJSON(json);
    cJSON_Delete(json);

    return result;
}