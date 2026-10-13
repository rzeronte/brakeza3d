//
// Created by Eduardo on 01/01/2026.
//

#include "../../include/GUI/NodeEditorManager.h"
#include <algorithm>
#include <set>
#include <cstring>
#include <imgui_node_editor.h>
#include <fstream>

#include "../../include/Misc/Logging.h"
#include "../../include/Misc/Tools.h"
#include "../../include/OpenGL/Base/ShaderBaseCustom.h"

NodeEditorManager::NodeEditorManager(ShaderCustomType type, const char* settingsFile)
    : m_Context(nullptr)
    , m_NextId(1)
    , m_ContextNodeId(0)
    , m_ContextLinkId(0)
    , m_ContextPinId(0)
    , m_ShowCreateMenu(false)
    , m_CreateMenuPos(0, 0)
    , type(type)
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

    // Guardar siempre Output -> Input, arrastres el enlace desde el lado que arrastres:
    // el recorrido del grafo busca el enlace por su pin de entrada (endPinId)
    auto startPin = FindPin(startPinId);
    if (startPin && startPin->kind == ed::PinKind::Input) {
        std::swap(startPinId, endPinId);
    }

    // Una entrada solo admite un enlace: el nuevo sustituye al anterior
    m_Links.erase(
        std::remove_if(m_Links.begin(), m_Links.end(),
            [endPinId](const std::shared_ptr<Link>& link) { return link->endPinId == endPinId; }),
        m_Links.end()
    );

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

    // ===== NUEVA VALIDACIÓN: FORZAR DIRECCIÓN OUTPUT → INPUT =====
    // Si el usuario arrastra desde Input hacia Output, intercambiar
    if (startPin->kind == ed::PinKind::Input && endPin->kind == ed::PinKind::Output) {
        std::swap(startPin, endPin);
        std::swap(startPinId, endPinId);
    }

    // Ahora startPin DEBE ser Output y endPin DEBE ser Input
    if (startPin->kind != ed::PinKind::Output || endPin->kind != ed::PinKind::Input) {
        return false;
    }

    // No conectar pines del mismo nodo
    if (startPin->nodeId == endPin->nodeId)
        return false;

    // Verificar tipos compatibles
    if (startPin->type != PinType::Any && endPin->type != PinType::Any) {
        if (startPin->type != endPin->type)
            return false;
    }

    // Un ciclo (A -> B -> A) haria que el recorrido del grafo recursara sin fin
    if (WouldCreateCycle(startPin->nodeId, endPin->nodeId))
        return false;

    // No permitir enlaces duplicados
    for (auto& link : m_Links) {
        if ((link->startPinId == startPinId && link->endPinId == endPinId) ||
            (link->startPinId == endPinId && link->endPinId == startPinId))
            return false;
    }

    return true;
}

bool NodeEditorManager::WouldCreateCycle(int sourceNodeId, int targetNodeId) const
{
    auto nodeOfPin = [this](int pinId) {
        for (const auto& pin : m_Pins) {
            if (pin->id == pinId) return pin->nodeId;
        }
        return -1;
    };

    // ¿Se llega a sourceNode siguiendo los enlaces desde targetNode?
    std::vector<int> pending{targetNodeId};
    std::set<int> visited;
    while (!pending.empty()) {
        const int current = pending.back();
        pending.pop_back();

        if (current == sourceNodeId) return true;
        if (!visited.insert(current).second) continue;

        for (const auto& link : m_Links) {
            if (nodeOfPin(link->startPinId) == current) {
                const int next = nodeOfPin(link->endPinId);
                if (next >= 0) pending.push_back(next);
            }
        }
    }
    return false;
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

cJSON* NodeEditorManager::getJSONTypes()
{
    ed::SetCurrentEditor(m_Context); // Necesito guardar el contexto

    cJSON* root = cJSON_CreateObject();

    // Versión del formato
    cJSON_AddStringToObject(root, "version", "1.0");
    cJSON_AddStringToObject(root, "editorType", "base");
    auto typeString = ShaderBaseCustom::getShaderTypeString(getType());
    cJSON_AddStringToObject(root, "type", typeString.c_str());

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

bool NodeEditorManager::LoadFromJSON(cJSON* json)
{
    LOG_MESSAGE("[NodeEditorManager] LoadFromJSON");

    if (!json) return false;

    // Verificar versión ANTES de tocar el estado: un fichero inválido no debe vaciar el editor
    cJSON* version = cJSON_GetObjectItem(json, "version");
    if (!version || !cJSON_IsString(version) || strcmp(version->valuestring, "1.0") != 0) {
        LOG_ERROR("[NodeEditorManager] Unsupported or missing 'version'");
        return false;
    }

    // Limpiar estado actual
    Clear();

    cJSON* nodesArray = cJSON_GetObjectItem(json, "nodes");
    cJSON* linksArray = cJSON_GetObjectItem(json, "links");

    // Reservar los ids por encima de TODOS los del fichero. Las clases hijas crean primero el nodo
    // con ids provisionales y luego les ponen los del fichero: así un id provisional nunca coincide
    // con el de un nodo, pin o enlace que todavía no se ha cargado.
    int maxId = 0;
    cJSON* it = nullptr;
    cJSON_ArrayForEach(it, nodesArray) {
        maxId = std::max(maxId, NodeJson::Int(it, "id"));
        for (const char* key : {"inputs", "outputs"}) {
            cJSON* pinJson = nullptr;
            cJSON_ArrayForEach(pinJson, cJSON_GetObjectItem(it, key)) {
                maxId = std::max(maxId, NodeJson::Int(pinJson, "id"));
            }
        }
    }
    cJSON_ArrayForEach(it, linksArray) {
        maxId = std::max(maxId, NodeJson::Int(it, "id"));
    }
    m_NextId = maxId + 1;

    // ========== CARGAR NODOS ==========
    if (nodesArray) {
        cJSON* nodeJson = nullptr;
        cJSON_ArrayForEach(nodeJson, nodesArray) {
            // Permitir que la clase hija cree el nodo (importante para ShaderNodeEditor)
            std::shared_ptr<Node> node = CreateNodeFromJSON(nodeJson);
            if (!node) {
                LOG_ERROR("[NodeEditorManager] Node skipped (invalid or unknown type '%s')", NodeJson::String(nodeJson, "type").c_str());
                continue;
            }

            LOG_MESSAGE("[NodeEditorManager] Creating NODE: %s - %s", node->name.c_str(), node->type.c_str());

            cJSON* posObj = cJSON_GetObjectItem(nodeJson, "position");
            if (posObj) {
                ed::SetNodePosition(node->id, ImVec2(NodeJson::Float(posObj, "x"), NodeJson::Float(posObj, "y")));
            }

            cJSON* userData = cJSON_GetObjectItem(nodeJson, "userData");
            if (userData) {
                DeserializeNodeUserData(node, userData);
            }
        }
    } else {
        LOG_MESSAGE("[NodeEditorManager] There aren't nodes");
    }

    // ========== CARGAR LINKS ==========
    cJSON* linkJson = nullptr;
    cJSON_ArrayForEach(linkJson, linksArray) {
        int startPinId = NodeJson::Int(linkJson, "startPinId", -1);
        int endPinId = NodeJson::Int(linkJson, "endPinId", -1);

        auto startPin = FindPin(startPinId);
        auto endPin = FindPin(endPinId);
        if (!startPin || !endPin) {
            LOG_ERROR("[NodeEditorManager] Link skipped: pin not found (%d -> %d)", startPinId, endPinId);
            continue;
        }

        // Ficheros guardados con la dirección invertida (Input -> Output): normalizar
        if (startPin->kind == ed::PinKind::Input && endPin->kind == ed::PinKind::Output) {
            std::swap(startPin, endPin);
            std::swap(startPinId, endPinId);
        }

        // Validación mínima: sin tipos (los ficheros antiguos no distinguían vec2/vec3), pero sin
        // ciclos ni dos enlaces en la misma entrada
        if (startPin->kind != ed::PinKind::Output || endPin->kind != ed::PinKind::Input ||
            startPin->nodeId == endPin->nodeId ||
            WouldCreateCycle(startPin->nodeId, endPin->nodeId) ||
            IsPinConnected(endPinId)) {
            LOG_ERROR("[NodeEditorManager] Link skipped: invalid connection (%d -> %d)", startPinId, endPinId);
            continue;
        }

        m_Links.push_back(std::make_shared<Link>(NodeJson::Int(linkJson, "id", m_NextId++), startPinId, endPinId));
    }

    // ========== DATOS CUSTOM ==========
    DeserializeCustomData(json);

    return true;
}

// Implementación por defecto (puede ser sobrescrita)
std::shared_ptr<Node> NodeEditorManager::CreateNodeFromJSON(cJSON* nodeJson) {
    if (!nodeJson || !cJSON_GetObjectItem(nodeJson, "id")) return nullptr;

    int id = NodeJson::Int(nodeJson, "id");
    std::string name = NodeJson::String(nodeJson, "name");
    std::string type = NodeJson::String(nodeJson, "type", "base");

    // Color
    cJSON* colorObj = cJSON_GetObjectItem(nodeJson, "color");
    ImVec4 color(
        NodeJson::Float(colorObj, "r", 0.2f),
        NodeJson::Float(colorObj, "g", 0.2f),
        NodeJson::Float(colorObj, "b", 0.2f),
        NodeJson::Float(colorObj, "a", 1.0f)
    );

    // Crear nodo base
    auto node = std::make_shared<Node>(id, name, type, color);

    // Actualizar m_NextId
    if (id >= m_NextId) {
        m_NextId = id + 1;
    }

    auto loadPins = [&](const char* key, ed::PinKind kind, std::vector<std::shared_ptr<Pin>>& dest) {
        cJSON* pinJson = nullptr;
        cJSON_ArrayForEach(pinJson, cJSON_GetObjectItem(nodeJson, key)) {
            int pinId = NodeJson::Int(pinJson, "id");
            PinType pinType = static_cast<PinType>(NodeJson::Int(pinJson, "type", static_cast<int>(PinType::Any)));

            auto pin = std::make_shared<Pin>(pinId, node->id, kind, pinType, NodeJson::String(pinJson, "name"));
            dest.push_back(pin);
            m_Pins.push_back(pin);

            if (pinId >= m_NextId) {
                m_NextId = pinId + 1;
            }
        }
    };
    loadPins("inputs", ed::PinKind::Input, node->inputs);
    loadPins("outputs", ed::PinKind::Output, node->outputs);

    m_Nodes.push_back(node);
    return node;
}

// Métodos virtuales por defecto (vacíos)
cJSON* NodeEditorManager::SerializeNodeUserData(std::shared_ptr<Node> node)
{
    return nullptr;
}

void NodeEditorManager::DeserializeNodeUserData(std::shared_ptr<Node> node, cJSON* userDataJson)
{
    // Por defecto no hace nada
}

void NodeEditorManager::SerializeCustomData(cJSON* root)
{
    // Por defecto no añade nada
}

void NodeEditorManager::DeserializeCustomData(cJSON* root)
{
    // Por defecto no hace nada
}

// Utilidades para guardar/cargar archivos
bool NodeEditorManager::SaveToFile(const char* filename)
{
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

bool NodeEditorManager::LoadFromFile(const char* filename)
{
    ed::SetCurrentEditor(m_Context);

    LOG_MESSAGE("[NodeEditorManager] Loading file '%s'", filename);

    std::string content = Tools::ReadFile(filename);

    cJSON* json = cJSON_Parse(content.c_str());

    if (!json) {
        LOG_ERROR("[NodeEditorManager] No valid JSON into '%s'", filename);
        return false;
    }

    cJSON* types = cJSON_GetObjectItem(json, "types");

    bool result = LoadFromJSON(types);

    cJSON_Delete(json);

    return result;
}

void NodeEditorManager::Autofit() const
{
    ed::SetCurrentEditor(m_Context);
    ed::NavigateToContent(0.0f);
}

bool NodeEditorManager::IsPinConnected(int pinId) const
{
    for (const auto& link : m_Links) {
        if (link->startPinId == pinId || link->endPinId == pinId) {
            return true;
        }
    }
    return false;
}
