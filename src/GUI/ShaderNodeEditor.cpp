#include "ShaderNodeEditor.h"
#include "../../include/Components/Components.h"
#include "../../include/Render/Image.h"

// Incluir todos los tipos de nodo
#include "NodeTypes/UVCoordsNode.h"
#include "NodeTypes/ColorNode.h"
#include "NodeTypes/TimeNode.h"
#include "NodeTypes/ImageNode.h"
#include "NodeTypes/MixNode.h"
#include "NodeTypes/GradientNode.h"
#include "NodeTypes/MultiplyNode.h"
#include "NodeTypes/AddNode.h"
#include "NodeTypes/SubtractNode.h"
#include "NodeTypes/SinNode.h"
#include "NodeTypes/CosNode.h"
#include "NodeTypes/SmoothstepNode.h"
#include "NodeTypes/UVOffsetNode.h"
#include "NodeTypes/UVScaleNode.h"
#include "NodeTypes/UVRotateNode.h"
#include "NodeTypes/OutputNode.h"

#include <cmath>

ShaderNodeEditor::ShaderNodeEditor(ShaderCustomType type)
    : NodeEditorManager(type, nullptr)
    , m_ShaderProgram(0)
    , m_VAO(0)
    , m_VBO(0)
    , m_NeedsRecompile(true)
    , m_FirstFrame(true)
{
    LOG_MESSAGE("[ShaderNodeEditor] Initializing...");

    RegisterNodeTypes();

    SetupQuad();
    CreateDefaultNodes();

    LOG_MESSAGE("[ShaderNodeEditor] Compiling initial shader...");
    CompileShader();
    m_NeedsRecompile = false;
    LOG_MESSAGE("[ShaderNodeEditor] Initialization complete.");
}

ShaderNodeEditor::~ShaderNodeEditor()
{
    for (auto& pair : m_NodeTextures) {
        if (pair.second.image) {
            delete pair.second.image;
            pair.second.image = nullptr;
        }
    }
    m_NodeTextures.clear();

    // Limpiar tipos de nodo registrados
    for (auto& pair : m_NodeTypes) {
        delete pair.second;
    }
    m_NodeTypes.clear();

    if (m_ShaderProgram) glDeleteProgram(m_ShaderProgram);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
}

void ShaderNodeEditor::RegisterNodeTypes()
{
    // Registrar todos los tipos de nodo
    RegisterNodeType(new UVCoordsNode());
    RegisterNodeType(new ColorNode());
    RegisterNodeType(new TimeNode());
    RegisterNodeType(new ImageNode());
    RegisterNodeType(new MixNode());
    RegisterNodeType(new GradientNode());
    RegisterNodeType(new MultiplyNode());
    RegisterNodeType(new AddNode());
    RegisterNodeType(new SubtractNode());
    RegisterNodeType(new SinNode());
    RegisterNodeType(new CosNode());
    RegisterNodeType(new SmoothstepNode());
    RegisterNodeType(new UVOffsetNode());
    RegisterNodeType(new UVScaleNode());
    RegisterNodeType(new UVRotateNode());
    RegisterNodeType(new OutputNode());
}

void ShaderNodeEditor::RegisterNodeType(NodeType* nodeType)
{
    m_NodeTypes[nodeType->GetTypeName()] = nodeType;

    NodeDefinition def;
    def.name = nodeType->GetTypeName();
    def.color = nodeType->GetColor();
    def.onCreate = nullptr;
    def.onDelete = nullptr;
    def.renderUI = nullptr;
    def.generateCode = nullptr;

    m_NodeRegistry[nodeType->GetTypeName()] = def;

    LOG_MESSAGE("[ShaderNodeEditor] Registered node type: %s", nodeType->GetTypeName().c_str());
}

std::shared_ptr<Node> ShaderNodeEditor::CreateNodeOfType(const std::string& typeName)
{
    auto it = m_NodeTypes.find(typeName);
    if (it == m_NodeTypes.end()) {
        LOG_MESSAGE("[ShaderNodeEditor] Unknown node type: %s", typeName.c_str());
        return nullptr;
    }

    NodeType* nodeType = it->second;
    auto node = CreateNode(nodeType->GetTypeName(), nodeType->GetColor());
    node->type = nodeType->GetTypeName();

    // Inicializar el nodo
    nodeType->OnCreate(node);

    // Añadir pins
    nodeType->SetupPins(node, this);

    return node;
}

void ShaderNodeEditor::SetupQuad()
{
    float vertices[] = {
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void ShaderNodeEditor::CreateDefaultNodes()
{
    LOG_MESSAGE("[ShaderNodeEditor] Creating default nodes...");

    auto colorNode = CreateNodeOfType("Color");
    auto outputNode = CreateNodeOfType("Output");

    if (colorNode && outputNode) {
        auto link = CreateLink(colorNode->outputs[0]->id, outputNode->inputs[0]->id);
        if (link) {
            LOG_MESSAGE("  - Auto-connected Color to Output");
        }
    }

    LOG_MESSAGE("[ShaderNodeEditor] Default nodes created.");
}

void ShaderNodeEditor::SetInitialNodePositions()
{
    for (auto& node : GetNodes()) {
        if (node->name == "Color") {
            ed::SetNodePosition(node->id, ImVec2(50, 200));
        }
        else if (node->name == "Output") {
            ed::SetNodePosition(node->id, ImVec2(400, 200));
        }
    }
}

void ShaderNodeEditor::Render()
{
    ed::SetCurrentEditor(m_Context);
    ed::Begin("Node Editor", ImVec2(0, 0));

    if (m_FirstFrame) {
        SetInitialNodePositions();
        m_FirstFrame = false;
    }

    for (auto& node : m_Nodes) {
        RenderNode(node);
    }

    for (auto& link : m_Links) {
        ed::Link(link->id, link->startPinId, link->endPinId);
    }

    HandleLinkCreation();
    HandleDeletion();
    HandleContextMenus();

    ed::End();
    ed::SetCurrentEditor(nullptr);
}

void ShaderNodeEditor::RenderNode(std::shared_ptr<Node>& node)
{
    ed::PushStyleColor(ed::StyleColor_NodeBg, node->color);
    ed::BeginNode(node->id);

    ImGui::PushItemWidth(150);

    // Obtener el tipo de nodo
    auto it = m_NodeTypes.find(node->name);
    if (it != m_NodeTypes.end()) {
        NodeType* nodeType = it->second;

        // Mostrar prefijo según categoría
        if (node->inputs.empty() && !node->outputs.empty()) {
            ImGui::Text("[GEN] %s", node->name.c_str());
        } else if (node->outputs.empty() && !node->inputs.empty()) {
            ImGui::Text("[OUT] %s", node->name.c_str());
        } else {
            ImGui::Text("[PRO] %s", node->name.c_str());
        }

        // Renderizar UI personalizada del nodo
        nodeType->RenderUI(node, this);
    }

    ImGui::PopItemWidth();
    ImGui::Spacing();

    for (auto& pin : node->inputs) {
        ed::BeginPin(pin->id, ed::PinKind::Input);
        ImGui::Text("-> %s", pin->name.c_str());
        ed::EndPin();
    }

    ImGui::Spacing();

    for (auto& pin : node->outputs) {
        ed::BeginPin(pin->id, ed::PinKind::Output);
        ImGui::Text("%s ->", pin->name.c_str());
        ed::EndPin();
    }

    ed::EndNode();
    ed::PopStyleColor();
}

void ShaderNodeEditor::HandleLinkCreation()
{
    if (ed::BeginCreate()) {
        ed::PinId startPinId, endPinId;

        if (ed::QueryNewLink(&startPinId, &endPinId)) {
            if (startPinId && endPinId) {
                if (CanCreateLink(startPinId.Get(), endPinId.Get())) {
                    if (ed::AcceptNewItem()) {
                        CreateLink(startPinId.Get(), endPinId.Get());
                        m_NeedsRecompile = true;
                    }
                } else {
                    ed::RejectNewItem(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
                }
            }
        }
    }
    ed::EndCreate();
}

void ShaderNodeEditor::HandleDeletion()
{
    if (ed::BeginDelete()) {
        ed::LinkId deletedLinkId;
        while (ed::QueryDeletedLink(&deletedLinkId)) {
            if (ed::AcceptDeletedItem()) {
                DeleteLink(deletedLinkId.Get());
                m_NeedsRecompile = true;
            }
        }

        ed::NodeId deletedNodeId;
        while (ed::QueryDeletedNode(&deletedNodeId)) {
            if (ed::AcceptDeletedItem()) {
                auto node = FindNode(deletedNodeId.Get());
                if (node) {
                    // Llamar onDelete del tipo de nodo
                    auto it = m_NodeTypes.find(node->name);
                    if (it != m_NodeTypes.end()) {
                        it->second->OnDelete(node, this);
                    }
                }

                DeleteNode(deletedNodeId.Get());
                m_NeedsRecompile = true;
            }
        }
    }
    ed::EndDelete();
}

void ShaderNodeEditor::OnCreateNodeMenu()
{
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "GENERATORS");
    ImGui::Separator();

    if (ImGui::MenuItem("UV Coordinates")) CreateNodeOfType("UV Coords");
    if (ImGui::MenuItem("Color")) CreateNodeOfType("Color");
    if (ImGui::MenuItem("Time")) CreateNodeOfType("Time");
    if (ImGui::MenuItem("Image Texture")) CreateNodeOfType("Image");

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "MATH");
    ImGui::Separator();

    if (ImGui::MenuItem("Add")) CreateNodeOfType("Add");
    if (ImGui::MenuItem("Subtract")) CreateNodeOfType("Subtract");
    if (ImGui::MenuItem("Multiply")) CreateNodeOfType("Multiply");
    if (ImGui::MenuItem("Mix")) CreateNodeOfType("Mix");
    if (ImGui::MenuItem("Sin")) CreateNodeOfType("Sin");
    if (ImGui::MenuItem("Cos")) CreateNodeOfType("Cos");
    if (ImGui::MenuItem("Smoothstep")) CreateNodeOfType("Smoothstep");

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.5f, 1.0f), "UV");
    ImGui::Separator();

    if (ImGui::MenuItem("UV Offset")) CreateNodeOfType("UV Offset");
    if (ImGui::MenuItem("UV Scale")) CreateNodeOfType("UV Scale");
    if (ImGui::MenuItem("UV Rotate")) CreateNodeOfType("UV Rotate");

    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 1.0f, 1.0f), "EFFECTS");
    ImGui::Separator();

    if (ImGui::MenuItem("Gradient")) CreateNodeOfType("Gradient");
}

void ShaderNodeEditor::Update()
{
    if (m_NeedsRecompile) {
        CompileShader();
        m_NeedsRecompile = false;
    }
}

void ShaderNodeEditor::RenderEffect()
{
    if (!m_ShaderProgram) return;

    Components::get()->Render()->ChangeOpenGLFramebuffer(Components::get()->Window()->getSceneFramebuffer());
    glUseProgram(m_ShaderProgram);

    float time = (float)ImGui::GetTime();
    GLint timeLoc = glGetUniformLocation(m_ShaderProgram, "u_Time");
    if (timeLoc != -1) glUniform1f(timeLoc, time);

    int textureUnit = 0;
    for (auto& node : GetNodes()) {
        if (node->name == "Image" && m_NodeTextures.find(node->id) != m_NodeTextures.end()) {
            auto& tex = m_NodeTextures[node->id];
            if (tex.image) {
                glActiveTexture(GL_TEXTURE0 + textureUnit);
                glBindTexture(GL_TEXTURE_2D, tex.image->getOGLTextureID());

                std::string uniformName = "u_Texture" + std::to_string(node->id);
                GLint texLoc = glGetUniformLocation(m_ShaderProgram, uniformName.c_str());
                if (texLoc != -1) {
                    glUniform1i(texLoc, textureUnit);
                }

                textureUnit++;
            }
        }
    }

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    for (int i = 0; i < textureUnit; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glUseProgram(0);
}

GLuint ShaderNodeEditor::GetShaderProgram() const
{
    return m_ShaderProgram;
}

const std::string& ShaderNodeEditor::GetLastShaderCode() const
{
    return m_LastShaderCode;
}

void ShaderNodeEditor::LoadImageForNode(int nodeId, const std::string& filepath)
{
    DeleteImageForNode(nodeId);

    try {
        if (m_NodeTextures[nodeId].image == nullptr) {
            m_NodeTextures[nodeId].image = new Image(filepath);
            m_NodeTextures[nodeId].filepath = filepath;
        } else {
            m_NodeTextures[nodeId].image->setImage(filepath);
            m_NodeTextures[nodeId].filepath = filepath;
        }
    } catch (const std::exception& e) {
        LOG_MESSAGE("Exception loading texture: %s", e.what());
    }
}

void ShaderNodeEditor::DeleteImageForNode(int nodeId)
{
    auto it = m_NodeTextures.find(nodeId);
    if (it != m_NodeTextures.end()) {
        if (it->second.image) {
            delete it->second.image;
            it->second.image = nullptr;
            LOG_MESSAGE("Deleted texture for node %d", nodeId);
        }
        m_NodeTextures.erase(it);
    }
}

std::string ShaderNodeEditor::GenerateShaderCode()
{
    m_NodeOutputs.clear();

    std::stringstream code;
    code << "#version 330 core\n";
    code << "in vec2 v_TexCoord;\n";
    code << "out vec4 FragColor;\n";
    code << "uniform float u_Time;\n";

    for (auto& node : GetNodes()) {
        if (node->name == "Image" && m_NodeTextures.find(node->id) != m_NodeTextures.end()) {
            auto& tex = m_NodeTextures[node->id];
            if (tex.image) {
                code << "uniform sampler2D u_Texture" << node->id << ";\n";
            }
        }
    }

    code << "\n";

    std::shared_ptr<Node> outputNode = nullptr;
    for (auto& node : GetNodes()) {
        if (node->name == "Output") {
            outputNode = node;
            break;
        }
    }

    if (!outputNode || outputNode->inputs.empty()) {
        code << "void main() {\n";
        code << "    FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n";
        code << "}\n";
        return code.str();
    }

    code << "void main() {\n";

    std::string finalColor = TraverseNode(outputNode->inputs[0]->id, code);

    if (finalColor.empty()) {
        code << "    FragColor = vec4(0.0, 1.0, 1.0, 1.0);\n";
    } else {
        code << "    FragColor = vec4(" << finalColor << ", 1.0);\n";
    }

    code << "}\n";

    return code.str();
}

std::string ShaderNodeEditor::TraverseNode(int pinId, std::stringstream& code)
{
    std::shared_ptr<Link> connectedLink = nullptr;
    for (auto& link : GetLinks()) {
        if (link->endPinId == pinId) {
            connectedLink = link;
            break;
        }
    }

    if (!connectedLink) return "";

    auto sourcePin = FindPin(connectedLink->startPinId);
    if (!sourcePin) return "";

    auto sourceNode = FindNode(sourcePin->nodeId);
    if (!sourceNode) return "";

    if (m_NodeOutputs.find(sourceNode->id) != m_NodeOutputs.end()) {
        return m_NodeOutputs[sourceNode->id];
    }

    // Usar el generador de código del tipo de nodo
    auto it = m_NodeTypes.find(sourceNode->name);
    if (it != m_NodeTypes.end()) {
        std::string result = it->second->GenerateCode(sourceNode, sourcePin, code, this);
        if (!result.empty()) {
            m_NodeOutputs[sourceNode->id] = result;
            return result;
        }
    }

    return "";
}

void ShaderNodeEditor::CompileShader()
{
    if (m_ShaderProgram) {
        glDeleteProgram(m_ShaderProgram);
        m_ShaderProgram = 0;
    }

    std::string fragmentCode = GenerateShaderCode();
    m_LastShaderCode = fragmentCode;

    LOG_MESSAGE("========== GENERATED SHADER CODE ==========");
    LOG_MESSAGE(fragmentCode.c_str());
    LOG_MESSAGE("===========================================");

    const char* vertexCode = R"(
        #version 330 core
        layout (location = 0) in vec2 a_Position;
        layout (location = 1) in vec2 a_TexCoord;
        out vec2 v_TexCoord;
        void main() {
            v_TexCoord = a_TexCoord;
            gl_Position = vec4(a_Position, 0.0, 1.0);
        }
    )";

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexCode, nullptr);
    glCompileShader(vertexShader);

    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        LOG_MESSAGE("Vertex Shader Error: %s", infoLog);
        return;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragCode = fragmentCode.c_str();
    glShaderSource(fragmentShader, 1, &fragCode, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        LOG_MESSAGE("Fragment Shader Error: %s", infoLog);
        glDeleteShader(vertexShader);
        return;
    }

    m_ShaderProgram = glCreateProgram();
    glAttachShader(m_ShaderProgram, vertexShader);
    glAttachShader(m_ShaderProgram, fragmentShader);
    glLinkProgram(m_ShaderProgram);

    glGetProgramiv(m_ShaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_ShaderProgram, 512, nullptr, infoLog);
        LOG_MESSAGE("Shader Link Error: %s", infoLog);
        m_ShaderProgram = 0;
    } else {
        LOG_MESSAGE("Shader compiled successfully! ID: %u", m_ShaderProgram);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

cJSON* ShaderNodeEditor::SerializeNodeUserData(std::shared_ptr<Node> node) {
    cJSON* userData = cJSON_CreateObject();

    // Serializar texturas asociadas al nodo
    auto it = m_NodeTextures.find(node->id);
    if (it != m_NodeTextures.end() && !it->second.filepath.empty()) {
        cJSON_AddStringToObject(userData, "imagePath", it->second.filepath.c_str());
    }

    // Si hay otros datos específicos del tipo de nodo, añadirlos aquí
    // Por ejemplo, si tienes valores en userData específicos de cada NodeType

    return userData;
}

void ShaderNodeEditor::DeserializeNodeUserData(std::shared_ptr<Node> node, cJSON* userDataJson) {
    // Cargar imagen si existe
    cJSON* imagePath = cJSON_GetObjectItem(userDataJson, "imagePath");
    if (imagePath && imagePath->valuestring) {
        LoadImageForNode(node->id, imagePath->valuestring);
    }

    // Deserializar otros datos específicos si es necesario
}

void ShaderNodeEditor::SerializeCustomData(cJSON* root)
{
    // Guardar el último shader code generado (opcional)
    if (!m_LastShaderCode.empty()) {
        cJSON_AddStringToObject(root, "lastShaderCode", m_LastShaderCode.c_str());
    }

    // Podrías guardar otros datos globales aquí si los necesitas
}

void ShaderNodeEditor::DeserializeCustomData(cJSON* root) {
    // Verificar que es un shader editor
    cJSON* editorType = cJSON_GetObjectItem(root, "editorType");
    if (!editorType || strcmp(editorType->valuestring, "shader") != 0) {
        // Log warning o error
        return;
    }

    // Cargar shader code si existe (opcional)
    cJSON* shaderCode = cJSON_GetObjectItem(root, "lastShaderCode");
    if (shaderCode) {
        m_LastShaderCode = shaderCode->valuestring;
    }

    // Marcar para recompilar
    m_NeedsRecompile = true;
}

std::shared_ptr<Node> ShaderNodeEditor::CreateNodeFromJSON(cJSON* nodeJson) {
    std::string type = cJSON_GetObjectItem(nodeJson, "type")->valuestring;

    // Usar el sistema existente de CreateNodeOfType
    std::shared_ptr<Node> node = CreateNodeOfType(type);

    if (!node) {
        // Fallback a la implementación base si el tipo no existe
        return NodeEditorManager::CreateNodeFromJSON(nodeJson);
    }

    // El nodo ya tiene sus pins creados por CreateNodeOfType,
    // pero necesitamos actualizar los IDs para que coincidan con el JSON
    int id = cJSON_GetObjectItem(nodeJson, "id")->valueint;
    node->id = id;

    // Actualizar IDs de los pins
    cJSON* inputsArray = cJSON_GetObjectItem(nodeJson, "inputs");
    if (inputsArray) {
        int idx = 0;
        cJSON* pinJson = nullptr;
        cJSON_ArrayForEach(pinJson, inputsArray) {
            if (idx < node->inputs.size()) {
                int pinId = cJSON_GetObjectItem(pinJson, "id")->valueint;
                node->inputs[idx]->id = pinId;

                if (pinId >= m_NextId) {
                    m_NextId = pinId + 1;
                }
            }
            idx++;
        }
    }

    cJSON* outputsArray = cJSON_GetObjectItem(nodeJson, "outputs");
    if (outputsArray) {
        int idx = 0;
        cJSON* pinJson = nullptr;
        cJSON_ArrayForEach(pinJson, outputsArray) {
            if (idx < node->outputs.size()) {
                int pinId = cJSON_GetObjectItem(pinJson, "id")->valueint;
                node->outputs[idx]->id = pinId;

                if (pinId >= m_NextId) {
                    m_NextId = pinId + 1;
                }
            }
            idx++;
        }
    }

    if (id >= m_NextId) {
        m_NextId = id + 1;
    }

    return node;
}