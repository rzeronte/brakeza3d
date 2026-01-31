#include "../../include/GUI/ShaderNodeEditorManager.h"
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
#include "NodeTypes/FloatValueNode.h"
#include "NodeTypes/MultiplyFloatNode.h"
#include "NodeTypes/AddFloatNode.h"
#include "NodeTypes/SubtractFloatNode.h"
#include "NodeTypes/Vec2ComposeNode.h"
#include "NodeTypes/Vec2DecomposeNode.h"
#include "NodeTypes/GrayscaleNode.h"
#include "NodeTypes/InvertNode.h"
#include "NodeTypes/VignetteNode.h"
#include "NodeTypes/ContrastNode.h"
#include "NodeTypes/SaturationNode.h"

#include <cmath>

#include "../../include/GUI/ImGuiLayoutHelpers.h"
#include "../../include/GUI/Objects/FileSystemGUI.h"
#include "NodeTypes/InternalTextureNode.h"
#include "NodeTypes/MeshTextureNode.h"

ShaderNodeEditorManager::ShaderNodeEditorManager(ShaderCustomType type)
    : NodeEditorManager(type, nullptr)
    , m_ShaderProgram(0)
    , m_VAO(0)
    , m_VBO(0)
    , m_NeedsRecompile(true)
    , m_FirstFrame(true)
{
    LOG_MESSAGE("[ShaderNodeEditorManager] Initializing...");

    RegisterNodeTypes();

    SetupQuad();
    CreateDefaultNodes();

    LOG_MESSAGE("[ShaderNodeEditorManager] Compiling initial shader...");
    CompileShader();
    m_NeedsRecompile = false;
    LOG_MESSAGE("[ShaderNodeEditorManager] Initialization complete.");
}

ShaderNodeEditorManager::~ShaderNodeEditorManager()
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

void ShaderNodeEditorManager::RegisterNodeTypes()
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
    RegisterNodeType(new InternalTextureNode());
    RegisterNodeType(new MeshTextureNode());
    RegisterNodeType(new OutputNode());
    RegisterNodeType(new FloatValueNode());
    RegisterNodeType(new MultiplyFloatNode());
    RegisterNodeType(new AddFloatNode());
    RegisterNodeType(new SubtractFloatNode());
    RegisterNodeType(new Vec2ComposeNode());
    RegisterNodeType(new Vec2DecomposeNode());
    RegisterNodeType(new GrayscaleNode());
    RegisterNodeType(new InvertNode());
    RegisterNodeType(new VignetteNode());
    RegisterNodeType(new ContrastNode());
    RegisterNodeType(new SaturationNode());
}

void ShaderNodeEditorManager::RegisterNodeType(NodeType* nodeType)
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

    LOG_MESSAGE("[ShaderNodeEditorManager] Registered node type: %s", nodeType->GetTypeName().c_str());
}

std::shared_ptr<Node> ShaderNodeEditorManager::CreateNodeOfType(const std::string& typeName)
{
    auto it = m_NodeTypes.find(typeName);
    if (it == m_NodeTypes.end()) {
        LOG_MESSAGE("[ShaderNodeEditorManager] Unknown node type: %s", typeName.c_str());
        return nullptr;
    }

    NodeType* nodeType = it->second;
    auto node = CreateNode(nodeType->GetTypeName(), nodeType->GetColor());
    node->type = nodeType->GetTypeName();
    node->icon = nodeType->GetIcon();

    // Inicializar el nodo
    nodeType->OnCreate(node);

    // Añadir pins
    nodeType->SetupPins(node, this);

    return node;
}

void ShaderNodeEditorManager::SetupQuad()
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

void ShaderNodeEditorManager::CreateDefaultNodes()
{
    LOG_MESSAGE("[ShaderNodeEditorManager] Creating default nodes...");

    auto colorNode = CreateNodeOfType("Color");
    auto outputNode = CreateNodeOfType("Output");

    if (colorNode && outputNode) {
        auto link = CreateLink(colorNode->outputs[0]->id, outputNode->inputs[0]->id);
        if (link) {
            LOG_MESSAGE("  - Auto-connected Color to Output");
        }
    }

    LOG_MESSAGE("[ShaderNodeEditorManager] Default nodes created.");
}

void ShaderNodeEditorManager::SetInitialNodePositions()
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

void ShaderNodeEditorManager::Render()
{
    ed::SetCurrentEditor(m_Context);
    ed::Begin("Node Editor", ImVec2(0, 0));

    if (m_FirstFrame) {
        SetInitialNodePositions();
        Autofit();
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

void ShaderNodeEditorManager::DrawNodeHeader(std::shared_ptr<Node> &node)
{
    ImGui::Image(FileSystemGUI::Icon(node->icon), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
    ImGui::SameLine();
    std::string upperName = node->name;
    std::transform(upperName.begin(), upperName.end(), upperName.begin(), ::toupper);
    ImGui::TextColored(ImVec4(0.0f, 0.0f, 0.0f, 1.0f), "%s", upperName.c_str());
}

void ShaderNodeEditorManager::RenderNode(std::shared_ptr<Node>& node)
{
    ed::PushStyleColor(ed::StyleColor_NodeBg, node->color);
    ed::BeginNode(node->id);

    ImGui::PushItemWidth(150);

    float uiContentWidth = 0.0f;

    // Obtener el tipo de nodo
    auto it = m_NodeTypes.find(node->name);
    if (it != m_NodeTypes.end()) {
        NodeType* nodeType = it->second;
        DrawNodeHeader(node);
        uiContentWidth = nodeType->RenderUI(node, this);
    }

    ImGui::PopItemWidth();
    ImGui::Spacing();

    // ===== PASADA 1: CALCULAR ANCHOS =====
    float maxInputWidth = 0.0f;
    float maxOutputWidth = 0.0f;

    float iconWidth = 16.0f;
    float iconSpacing = ImGui::GetStyle().ItemSpacing.x;

    for (auto& pin : node->inputs) {
        // Ancho = icono + spacing + texto
        float textWidth = ImGui::CalcTextSize(pin->name.c_str()).x;
        float width = iconWidth + iconSpacing + textWidth;
        maxInputWidth = std::max(maxInputWidth, width);
    }

    for (auto& pin : node->outputs) {
        // Ancho = texto + spacing + icono
        float textWidth = ImGui::CalcTextSize(pin->name.c_str()).x;
        float width = textWidth + iconSpacing + iconWidth;
        maxOutputWidth = std::max(maxOutputWidth, width);
    }

    // Ancho total del área de pins
    float minSpacing = 40.0f;
    float pinAreaWidth = maxInputWidth + minSpacing + maxOutputWidth;
    float totalNodeWidth = std::max(uiContentWidth, pinAreaWidth);
    totalNodeWidth = std::max(totalNodeWidth, 150.0f);

    size_t maxPins = std::max(node->inputs.size(), node->outputs.size());

    // ===== PASADA 2: RENDERIZAR =====
    ImTextureID iconConnected = FileSystemGUI::Icon(IconGUI::NODE_EDITOR_PIN_IN);
    ImTextureID iconDisconnected = FileSystemGUI::Icon(IconGUI::NODE_EDITOR_PIN_OUT);

    for (size_t i = 0; i < maxPins; ++i) {
        float rowStartX = ImGui::GetCursorPosX();

        // Input pin (columna izquierda)
        if (i < node->inputs.size()) {
            auto& pin = node->inputs[i];
            bool isConnected = IsPinConnected(pin->id);
            ImTextureID icon = isConnected ? iconConnected : iconDisconnected;

            ed::BeginPin(pin->id, ed::PinKind::Input);
            ImGui::Image(icon, GUIType::Sizes::ICONS_LOG);
            ImGui::SameLine();
            ImGui::Text("%s", pin->name.c_str());
            ed::EndPin();
        } else {
            ImGui::Dummy(ImVec2(1, ImGui::GetTextLineHeight()));
        }

        // Output pin (columna derecha)
        if (i < node->outputs.size()) {
            float outputX = rowStartX + totalNodeWidth - maxOutputWidth;
            ImGui::SameLine();
            ImGui::SetCursorPosX(outputX);

            auto& pin = node->outputs[i];
            bool isConnected = IsPinConnected(pin->id);
            ImTextureID icon = isConnected ? iconConnected : iconDisconnected;

            ed::BeginPin(pin->id, ed::PinKind::Output);
            ImGui::Text("%s", pin->name.c_str());
            ImGui::SameLine();
            ImGui::Image(icon, GUIType::Sizes::ICONS_LOG);
            ed::EndPin();
        }
    }

    ed::EndNode();
    ed::PopStyleColor();
}

void ShaderNodeEditorManager::HandleLinkCreation()
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

void ShaderNodeEditorManager::HandleDeletion()
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

void ShaderNodeEditorManager::OnCreateNodeMenu()
{
    // === INPUTS ===
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "INPUTS");
    ImGui::Separator();

    if (ImGui::MenuItem("Time (float)")) CreateNodeOfType("Time");
    if (ImGui::MenuItem("Float Value (float)")) CreateNodeOfType("FloatValue");
    if (ImGui::MenuItem("UV Coords (vec2)")) CreateNodeOfType("UV Coords");
    if (ImGui::MenuItem("Color (vec3)")) CreateNodeOfType("Color");

    // === TEXTURES ===
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.3f, 1.0f), "TEXTURES");
    ImGui::Separator();

    if (ImGui::MenuItem("Image Texture (vec3)")) CreateNodeOfType("Image");
    if (getType() == SHADER_NODE_OBJECT) {
        if (ImGui::MenuItem("Mesh Texture (vec3)")) CreateNodeOfType("Mesh Texture");
    } else if (getType() == SHADER_NODE_POSTPROCESSING) {
        if (ImGui::MenuItem("Internal Texture (vec3)")) CreateNodeOfType("Internal Texture");
    }

    // === MATH FLOAT ===
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "MATH (float)");
    ImGui::Separator();

    if (ImGui::MenuItem("Add (float)")) CreateNodeOfType("AddFloat");
    if (ImGui::MenuItem("Subtract (float)")) CreateNodeOfType("SubtractFloat");
    if (ImGui::MenuItem("Multiply (float)")) CreateNodeOfType("MultiplyFloat");
    if (ImGui::MenuItem("Sin (float)")) CreateNodeOfType("Sin");
    if (ImGui::MenuItem("Cos (float)")) CreateNodeOfType("Cos");
    if (ImGui::MenuItem("Smoothstep (float)")) CreateNodeOfType("Smoothstep");

    // === MATH VEC3 ===
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "MATH (vec3)");
    ImGui::Separator();

    if (ImGui::MenuItem("Add (vec3)")) CreateNodeOfType("Add");
    if (ImGui::MenuItem("Subtract (vec3)")) CreateNodeOfType("Subtract");
    if (ImGui::MenuItem("Multiply (vec3)")) CreateNodeOfType("Multiply");
    if (ImGui::MenuItem("Mix (vec3)")) CreateNodeOfType("Mix");

    // === CONVERTERS ===
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.5f, 1.0f), "CONVERTERS");
    ImGui::Separator();

    if (ImGui::MenuItem("Vec2 Compose (float->vec2)")) CreateNodeOfType("Vec2Compose");
    if (ImGui::MenuItem("Vec2 Decompose (vec2->float)")) CreateNodeOfType("Vec2Decompose");

    // === UV OPERATIONS ===
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.5f, 1.0f), "UV (vec2)");
    ImGui::Separator();

    if (ImGui::MenuItem("UV Offset (vec2)")) CreateNodeOfType("UV Offset");
    if (ImGui::MenuItem("UV Scale (vec2)")) CreateNodeOfType("UV Scale");
    if (ImGui::MenuItem("UV Rotate (vec2)")) CreateNodeOfType("UV Rotate");

    // === EFFECTS ===
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 1.0f, 1.0f), "EFFECTS");
    ImGui::Separator();

    if (ImGui::MenuItem("Gradient (vec3)")) CreateNodeOfType("Gradient");
    if (ImGui::MenuItem("Grayscale (vec3->float)")) CreateNodeOfType("Grayscale");
    if (ImGui::MenuItem("Invert (vec3)")) CreateNodeOfType("Invert");
    if (ImGui::MenuItem("Contrast (vec3)")) CreateNodeOfType("Contrast");
    if (ImGui::MenuItem("Saturation (vec3)")) CreateNodeOfType("Saturation");
    if (ImGui::MenuItem("Vignette (float)")) CreateNodeOfType("Vignette");
}

void ShaderNodeEditorManager::Clear()
{
    // Limpiar texturas primero
    for (auto& pair : m_NodeTextures) {
        if (pair.second.image) {
            delete pair.second.image;
            pair.second.image = nullptr;
        }
    }
    m_NodeTextures.clear();

    // Llamar al Clear de la clase base para limpiar nodos/links/pins
    NodeEditorManager::Clear();

    // Recompilar shader después de limpiar
    m_NeedsRecompile = true;
}

void ShaderNodeEditorManager::Update()
{
    if (m_NeedsRecompile) {
        CompileShader();
        m_NeedsRecompile = false;
    }
}

void ShaderNodeEditorManager::RenderEffect(GLuint fb)
{
    if (!m_ShaderProgram) return;

    Components::get()->Render()->ChangeOpenGLFramebuffer(fb);
    Components::get()->Render()->ChangeOpenGLProgram(m_ShaderProgram);

    float time = (float)ImGui::GetTime();
    GLint timeLoc = glGetUniformLocation(m_ShaderProgram, "u_Time");
    if (timeLoc != -1) glUniform1f(timeLoc, time);

    int textureUnit = 0;
    for (auto& node : GetNodes()) {
        if ((node->name == "Image" ||
             node->name == "Internal Texture" ||
             node->name == "Mesh Texture") &&
            m_NodeTextures.find(node->id) != m_NodeTextures.end()) {

            auto& tex = m_NodeTextures[node->id];

            if (tex.image) {
                GLuint texID = tex.image->getOGLTextureID();

                glActiveTexture(GL_TEXTURE0 + textureUnit);
                glBindTexture(GL_TEXTURE_2D, texID);

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

GLuint ShaderNodeEditorManager::GetShaderProgram() const
{
    return m_ShaderProgram;
}

const std::string& ShaderNodeEditorManager::GetLastShaderCode() const
{
    return m_LastShaderCode;
}

void ShaderNodeEditorManager::LoadImageForNode(int nodeId, const std::string& filepath)
{
    DeleteImageForNode(nodeId);

    if (filepath == "[System Texture]") return;

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

void ShaderNodeEditorManager::DeleteImageForNode(int nodeId)
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

std::string ShaderNodeEditorManager::GenerateShaderCode()
{
    m_NodeOutputs.clear();

    std::stringstream code;
    code << "#version 330 core\n";

    if (getType() == SHADER_NODE_POSTPROCESSING) {
        code << "in vec2 v_TexCoord;\n";
        code << "out vec4 FragColor;\n";
    } else if (getType() == SHADER_NODE_OBJECT) {
        code << "in vec3 v_FragPos;\n";
        code << "in vec3 v_Normal;\n";
        code << "in vec2 v_TexCoord;\n";
        // GBuffer outputs
        code << "layout (location = 0) out vec3 gPosition;\n";
        code << "layout (location = 1) out vec3 gNormal;\n";
        code << "layout (location = 2) out vec4 gAlbedoSpec;\n";
    }
    code << "uniform float u_Time;\n";

    // Declarar uniforms de texturas
    for (auto& node : GetNodes()) {
        if ((node->name == "Image" ||
             node->name == "Internal Texture" ||
             node->name == "Mesh Texture") &&
            m_NodeTextures.find(node->id) != m_NodeTextures.end()) {
            auto& tex = m_NodeTextures[node->id];
            if (tex.image) {
                code << "uniform sampler2D u_Texture" << node->id << ";\n";
            }
            }
    }

    code << "\n";

    // Buscar nodo Output
    std::shared_ptr<Node> outputNode = nullptr;
    for (auto& node : GetNodes()) {
        if (node->name == "Output") {
            outputNode = node;
            break;
        }
    }

    if (!outputNode || outputNode->inputs.empty()) {
        code << "void main() {\n";
        if (getType() == SHADER_NODE_OBJECT) {
            code << "    gPosition = v_FragPos;\n";
            code << "    gNormal = normalize(v_Normal);\n";
            code << "    gAlbedoSpec = vec4(1.0, 0.0, 1.0, 1.0);\n";
        } else {
            code << "    FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n";
        }
        code << "}\n";
        return code.str();
    }

    code << "void main() {\n";

    std::string finalColor = TraverseNode(outputNode->inputs[0]->id, code);

    if (getType() == SHADER_NODE_OBJECT) {
        code << "    gPosition = v_FragPos;\n";
        code << "    gNormal = normalize(v_Normal);\n";
        if (finalColor.empty()) {
            code << "    gAlbedoSpec = vec4(0.0, 1.0, 1.0, 1.0);\n";
        } else {
            code << "    gAlbedoSpec = vec4(" << finalColor << ", 1.0);\n";
        }
    } else {
        if (finalColor.empty()) {
            code << "    FragColor = vec4(0.0, 1.0, 1.0, 1.0);\n";
        } else {
            code << "    FragColor = vec4(" << finalColor << ", 1.0);\n";
        }
    }

    code << "}\n";

    return code.str();
}

std::string ShaderNodeEditorManager::TraverseNode(int pinId, std::stringstream& code)
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

void ShaderNodeEditorManager::CompileShader()
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

    // ✅ Vertex shader según el tipo de shader
    const char* vertexCode;

    if (getType() == SHADER_NODE_OBJECT) {
        // Vertex shader para meshes 3D
        vertexCode = R"(
            #version 330 core
            layout (location = 0) in vec4 aPos;
            layout (location = 1) in vec3 aNormal;
            layout (location = 2) in vec2 aTexCoords;

            out vec3 v_FragPos;
            out vec3 v_Normal;
            out vec2 v_TexCoord;

            uniform mat4 u_Projection;
            uniform mat4 u_View;
            uniform mat4 u_Model;

            void main() {
                vec4 worldPos = u_Model * aPos;
                v_FragPos = worldPos.xyz;
                v_Normal = mat3(transpose(inverse(u_Model))) * aNormal;
                v_TexCoord = aTexCoords;
                gl_Position = u_Projection * u_View * worldPos;
            }
        )";
    } else {
        // Vertex shader para postprocesado (quad fullscreen)
        vertexCode = R"(
            #version 330 core
            layout (location = 0) in vec2 a_Position;
            layout (location = 1) in vec2 a_TexCoord;
            out vec2 v_TexCoord;
            void main() {
                v_TexCoord = a_TexCoord;
                gl_Position = vec4(a_Position, 0.0, 1.0);
            }
        )";
    }

    // Compilar vertex shader
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

    // Compilar fragment shader
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

    // Linkear programa
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

cJSON* ShaderNodeEditorManager::SerializeNodeUserData(std::shared_ptr<Node> node) {
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

void ShaderNodeEditorManager::DeserializeNodeUserData(std::shared_ptr<Node> node, cJSON* userDataJson) {
    // Cargar imagen si existe
    cJSON* imagePath = cJSON_GetObjectItem(userDataJson, "imagePath");
    if (imagePath && imagePath->valuestring) {
        LoadImageForNode(node->id, imagePath->valuestring);
    }

    // Deserializar otros datos específicos si es necesario
}

void ShaderNodeEditorManager::SerializeCustomData(cJSON* root)
{
    // Guardar el último shader code generado (opcional)
    if (!m_LastShaderCode.empty()) {
        cJSON_AddStringToObject(root, "lastShaderCode", m_LastShaderCode.c_str());
    }

    // Podrías guardar otros datos globales aquí si los necesitas
}

void ShaderNodeEditorManager::DeserializeCustomData(cJSON* root) {
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

std::shared_ptr<Node> ShaderNodeEditorManager::CreateNodeFromJSON(cJSON* nodeJson) {
    std::string type = cJSON_GetObjectItem(nodeJson, "type")->valuestring;
    std::shared_ptr<Node> node = CreateNodeOfType(type);

    if (!node) {
        return NodeEditorManager::CreateNodeFromJSON(nodeJson);
    }

    int oldId = node->id;
    int newId = cJSON_GetObjectItem(nodeJson, "id")->valueint;
    node->id = newId;

    if (node->name == "Internal Texture" || node->name == "Mesh Texture") {
        auto oldIt = m_NodeTextures.find(oldId);
        if (oldIt != m_NodeTextures.end()) {
            m_NodeTextures[newId] = oldIt->second;
            if (oldId != newId) {
                m_NodeTextures.erase(oldId);
            }
        } else {
            SetExternalTextureForNode(newId, 0);
        }
    }

    // ✅ ELIMINAR PINS CREADOS AUTOMÁTICAMENTE
    for (auto& pin : node->inputs) {
        m_Pins.erase(std::remove_if(m_Pins.begin(), m_Pins.end(),
            [pin](const std::shared_ptr<Pin>& p) { return p->id == pin->id; }), m_Pins.end());
    }
    for (auto& pin : node->outputs) {
        m_Pins.erase(std::remove_if(m_Pins.begin(), m_Pins.end(),
            [pin](const std::shared_ptr<Pin>& p) { return p->id == pin->id; }), m_Pins.end());
    }

    node->inputs.clear();
    node->outputs.clear();

    // ✅ RECREAR INPUTS CON IDS CORRECTOS
    cJSON* inputsArray = cJSON_GetObjectItem(nodeJson, "inputs");
    if (inputsArray) {
        cJSON* pinJson = nullptr;
        cJSON_ArrayForEach(pinJson, inputsArray) {
            int pinId = cJSON_GetObjectItem(pinJson, "id")->valueint;
            std::string pinName = cJSON_GetObjectItem(pinJson, "name")->valuestring;
            PinType pinType = static_cast<PinType>(cJSON_GetObjectItem(pinJson, "type")->valueint);

            auto pin = std::make_shared<Pin>(pinId, newId, ed::PinKind::Input, pinType, pinName);
            node->inputs.push_back(pin);
            m_Pins.push_back(pin);

            if (pinId >= m_NextId) m_NextId = pinId + 1;
        }
    }

    // ✅ RECREAR OUTPUTS CON IDS CORRECTOS
    cJSON* outputsArray = cJSON_GetObjectItem(nodeJson, "outputs");
    if (outputsArray) {
        cJSON* pinJson = nullptr;
        cJSON_ArrayForEach(pinJson, outputsArray) {
            int pinId = cJSON_GetObjectItem(pinJson, "id")->valueint;
            std::string pinName = cJSON_GetObjectItem(pinJson, "name")->valuestring;
            PinType pinType = static_cast<PinType>(cJSON_GetObjectItem(pinJson, "type")->valueint);

            auto pin = std::make_shared<Pin>(pinId, newId, ed::PinKind::Output, pinType, pinName);
            node->outputs.push_back(pin);
            m_Pins.push_back(pin);

            if (pinId >= m_NextId) m_NextId = pinId + 1;
        }
    }

    if (newId >= m_NextId) {
        m_NextId = newId + 1;
    }

    return node;
}

void ShaderNodeEditorManager::SetExternalTextureForNode(int nodeId, GLuint textureId)
{
    DeleteImageForNode(nodeId);

    auto window = Components::get()->Window();

    m_NodeTextures[nodeId].image = new Image(textureId, window->getWidthRender(), window->getHeightRender());
    m_NodeTextures[nodeId].filepath = "[System Texture]";

    LOG_MESSAGE("External texture set for node %d (GLuint: %u, %dx%d)", nodeId, textureId, window->getWidthRender(), window->getHeightRender());
}

void ShaderNodeEditorManager::PlugSystemTexture(std::shared_ptr<Node>& node, GLuint systemTextureId)
{
    SetExternalTextureForNode(node->id, systemTextureId);
}

void ShaderNodeEditorManager::UpdateExternalTextureForNode(int nodeId, GLuint textureId)
{
    auto it = m_NodeTextures.find(nodeId);
    if (it != m_NodeTextures.end() && it->second.image) {
        it->second.image->setOGLTextureID(textureId);
    }
}

void ShaderNodeEditorManager::UpdateChainOutputTexture(GLuint textureId)
{
    // Buscar todos los nodos ChainOutput y actualizar su textura
    for (auto& node : GetNodes()) {
        if (node->name == "Internal Texture") {
            UpdateExternalTextureForNode(node->id, textureId);
        }
    }
}

void ShaderNodeEditorManager::UpdateInternalTextures(GLuint colorTexture, GLuint depthTexture)
{
    for (auto& node : GetNodes()) {
        if (node->name == "Internal Texture") {
            InternalTextureType* texType = (InternalTextureType*)node->userData;
            if (texType) {
                GLuint textureToUse = 0;

                switch (*texType) {
                    case InternalTextureType::Color:
                        textureToUse = colorTexture;
                        break;
                    case InternalTextureType::Depth:
                        textureToUse = depthTexture;
                        break;
                }

                UpdateExternalTextureForNode(node->id, textureToUse);
            }
        }
    }
}

void ShaderNodeEditorManager::UpdateMeshTextures(GLuint diffuseTexture, GLuint specularTexture)
{
    for (auto& node : GetNodes()) {
        if (node->name == "Mesh Texture") {
            MeshTextureType* texType = (MeshTextureType*)node->userData;
            if (texType) {
                GLuint textureToUse = 0;

                switch (*texType) {
                    case MeshTextureType::Diffuse:
                        textureToUse = diffuseTexture;
                        break;
                    case MeshTextureType::Specular:
                        textureToUse = specularTexture;
                        break;
                    case MeshTextureType::Normal:
                    case MeshTextureType::Roughness:
                    case MeshTextureType::AO:
                        // Por ahora usar diffuse como fallback
                        textureToUse = diffuseTexture;
                        break;
                }

                UpdateExternalTextureForNode(node->id, textureToUse);
            }
        }
    }
}

void ShaderNodeEditorManager::RenderMesh(
    GLuint fb,
    GLuint vertexBuffer,
    GLuint uvBuffer,
    GLuint normalBuffer,
    int vertexCount,
    const glm::mat4& model,
    const glm::mat4& view,
    const glm::mat4& projection
)
{
    if (!m_ShaderProgram) return;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    Components::get()->Render()->ChangeOpenGLFramebuffer(fb);
    Components::get()->Render()->ChangeOpenGLProgram(m_ShaderProgram);

    GLint projLoc = glGetUniformLocation(m_ShaderProgram, "u_Projection");
    GLint viewLoc = glGetUniformLocation(m_ShaderProgram, "u_View");
    GLint modelLoc = glGetUniformLocation(m_ShaderProgram, "u_Model");

    if (projLoc != -1) glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
    if (viewLoc != -1) glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
    if (modelLoc != -1) glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    float time = (float)ImGui::GetTime();
    GLint timeLoc = glGetUniformLocation(m_ShaderProgram, "u_Time");
    if (timeLoc != -1) glUniform1f(timeLoc, time);

    int textureUnit = 0;
    for (auto& node : GetNodes()) {
        if ((node->name == "Image" ||
             node->name == "Internal Texture" ||
             node->name == "Mesh Texture") &&
            m_NodeTextures.find(node->id) != m_NodeTextures.end()) {
            auto& tex = m_NodeTextures[node->id];
            if (tex.image && tex.image->getOGLTextureID() != 0) {
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

    // Location 0: Position (vec4, 4 componentes)
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Location 1: Normal (vec3, 3 componentes)
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Location 2: UV (vec2, 2 componentes)
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    for (int i = 0; i < textureUnit; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glUseProgram(0);
    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderNodeEditorManager::RenderShaderDebugPanel()
{
    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8, 6));

    if (ImGui::BeginTable("NodesTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Node", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Connections", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (auto& node : m_Nodes) {
            ImGui::TableNextRow();

            // Columna 1: Icono + Nombre del nodo (con color)
            ImGui::TableNextColumn();

            // Determinar si está conectado
            bool hasConnections = false;
            for (auto& pin : node->inputs) {
                if (IsPinConnected(pin->id)) {
                    hasConnections = true;
                    break;
                }
            }
            for (auto& pin : node->outputs) {
                if (IsPinConnected(pin->id)) {
                    hasConnections = true;
                    break;
                }
            }

            ImTextureID icon = FileSystemGUI::Icon(hasConnections ? IconGUI::SHADER_UNLOCK : IconGUI::SHADER_LOCK);

            ImGui::Image(icon, ImVec2(16, 16));
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Text, node->color);
            ImGui::Text("%s (ID:%d)", node->name.c_str(), node->id);
            ImGui::PopStyleColor();

            // Columna 2: Con qué está conectado
            ImGui::TableNextColumn();
            std::string connections = "";

            // Buscar conexiones de inputs
            for (auto& pin : node->inputs) {
                for (auto& link : m_Links) {
                    if (link->endPinId == pin->id) {
                        auto startPin = FindPin(link->startPinId);
                        if (startPin) {
                            auto sourceNode = FindNode(startPin->nodeId);
                            if (sourceNode) {
                                if (!connections.empty()) connections += ", ";
                                connections += sourceNode->name + "." + startPin->name + " -> " + pin->name;
                            }
                        }
                    }
                }
            }

            // Buscar conexiones de outputs
            for (auto& pin : node->outputs) {
                for (auto& link : m_Links) {
                    if (link->startPinId == pin->id) {
                        auto endPin = FindPin(link->endPinId);
                        if (endPin) {
                            auto targetNode = FindNode(endPin->nodeId);
                            if (targetNode) {
                                if (!connections.empty()) connections += ", ";
                                connections += pin->name + " -> " + targetNode->name + "." + endPin->name;
                            }
                        }
                    }
                }
            }

            if (connections.empty()) {
                ImGui::TextDisabled("None");
            } else {
                ImGui::TextWrapped("%s", connections.c_str());
            }
        }

        ImGui::EndTable();
    }

    ImGui::PopStyleVar();
    ImGui::Spacing();

    // ===== CÓDIGO DEL SHADER =====
    if (ImGui::CollapsingHeader("Shader Code")) {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        ImGui::BeginChild("ShaderCode", ImVec2(0, 300), true);
        ImGui::TextUnformatted(m_LastShaderCode.c_str());
        ImGui::EndChild();
        ImGui::PopStyleColor();

        if (ImGui::Button("Copy to Clipboard")) {
            ImGui::SetClipboardText(m_LastShaderCode.c_str());
        }
    }
}