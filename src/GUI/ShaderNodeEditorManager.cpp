#include "../../include/GUI/ShaderNodeEditorManager.h"
#include "../../include/Components/Components.h"
#include "../../include/Render/Image.h"
#include "../../include/OpenGL/Base/ShaderBaseOpenGL.h"

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
#include "NodeTypes/FloorNode.h"
#include "NodeTypes/FractNode.h"
#include "NodeTypes/AbsNode.h"
#include "NodeTypes/StepNode.h"
#include "NodeTypes/NoiseNode.h"
#include "NodeTypes/Vec3ComposeNode.h"
#include "NodeTypes/Vec3DecomposeNode.h"
#include "NodeTypes/PosterizeNode.h"
#include "NodeTypes/PixelateNode.h"
#include "NodeTypes/ScaleColorNode.h"
#include "NodeTypes/NormalNode.h"
#include "NodeTypes/FresnelNode.h"
#include "NodeTypes/UVRadialNode.h"

#include <cmath>
#include <vector>
#include <algorithm>

#include "../../include/Brakeza.h"
#include "../../include/GUI/Widgets/ImGuiLayoutHelpers.h"
#include "../../include/GUI/Windows/FileSystemGUI.h"
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

    // Este constructor puede ejecutarse en un hilo de carga: aquí no se toca OpenGL.
    // El quad y la primera compilación se hacen en Update(), que corre en el hilo principal.
    RegisterNodeTypes();
    CreateDefaultNodes();
    m_DefaultLayout = true;

    LOG_MESSAGE("[ShaderNodeEditorManager] Initialization complete.");
}

ShaderNodeEditorManager::~ShaderNodeEditorManager()
{
    // Liberar el userData de cada nodo y las texturas propias antes de borrar los tipos de nodo
    ReleaseAllNodes();

    // Limpiar tipos de nodo registrados
    for (auto& pair : m_NodeTypes) {
        delete pair.second;
    }
    m_NodeTypes.clear();

    if (m_ShaderProgram) glDeleteProgram(m_ShaderProgram);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
}

void ShaderNodeEditorManager::ReleaseAllNodes()
{
    m_Uniforms.clear();   // apuntan a datos de los nodos

    for (auto& node : m_Nodes) {
        auto it = m_NodeTypes.find(node->name);
        if (it != m_NodeTypes.end()) {
            it->second->OnDelete(node, this);
        }
    }

    for (auto& pair : m_NodeTextures) {
        ReleaseNodeImage(pair.second);
    }
    m_NodeTextures.clear();
}

void ShaderNodeEditorManager::ReleaseNodeImage(ImageTexture& tex)
{
    if (!tex.image) return;

    // Las texturas "de sistema" son del motor (resultado de la cadena, difusa del mesh...): el
    // destructor de Image las borraría con glDeleteTextures, así que se sueltan antes de borrar
    if (tex.filepath == kSystemTexture) {
        tex.image->setOGLTextureID(0);
    }

    delete tex.image;
    tex.image = nullptr;
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
    RegisterNodeType(new FloorNode());
    RegisterNodeType(new FractNode());
    RegisterNodeType(new AbsNode());
    RegisterNodeType(new StepNode());
    RegisterNodeType(new NoiseNode());
    RegisterNodeType(new Vec3ComposeNode());
    RegisterNodeType(new Vec3DecomposeNode());
    RegisterNodeType(new PosterizeNode());
    RegisterNodeType(new PixelateNode());
    RegisterNodeType(new ScaleColorNode());
    RegisterNodeType(new NormalNode());
    RegisterNodeType(new FresnelNode());
    RegisterNodeType(new UVRadialNode());
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
    if (m_VAO != 0) return;

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
    // Solo para el grafo por defecto recién creado: un grafo cargado de disco conserva sus posiciones
    if (!m_DefaultLayout) return;

    for (auto& node : GetNodes()) {
        if (node->name == "Color") {
            ed::SetNodePosition(node->id, ImVec2(50, 200));
        }
        else if (node->name == "Output") {
            ed::SetNodePosition(node->id, ImVec2(400, 200));
        }
    }
    m_DefaultLayout = false;
}

void ShaderNodeEditorManager::Render()
{
    if (!m_LastCompileError.empty()) {
        ImGui::TextColored(ImVec4(1.0f, 0.35f, 0.35f, 1.0f), "Shader error (see the Shader Code panel / log)");
    }

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
            auto node = FindNode(deletedNodeId.Get());
            // El nodo Output es el punto de entrada del grafo: no se puede borrar
            if (node && node->name == "Output") {
                ed::RejectDeletedItem();
                continue;
            }

            if (ed::AcceptDeletedItem()) {
                DeleteNode(deletedNodeId.Get());
            }
        }
    }
    ed::EndDelete();
}

void ShaderNodeEditorManager::DeleteNode(int nodeId)
{
    auto node = FindNode(nodeId);
    if (node) {
        auto it = m_NodeTypes.find(node->name);
        if (it != m_NodeTypes.end()) {
            it->second->OnDelete(node, this);
        }
    }

    m_Uniforms.clear();   // podían apuntar al userData que acaba de liberarse; se regeneran al recompilar
    NodeEditorManager::DeleteNode(nodeId);
    m_NeedsRecompile = true;
}

void ShaderNodeEditorManager::OnNodeContextMenu(int nodeId)
{
    auto node = FindNode(nodeId);
    if (!node) return;

    const bool isOutput = node->name == "Output";

    if (ImGui::MenuItem("Delete Node", nullptr, false, !isOutput)) {
        DeleteNode(nodeId);
    }

    if (ImGui::MenuItem("Duplicate Node", nullptr, false, !isOutput)) {
        DuplicateNode(node);
    }
}

void ShaderNodeEditorManager::OnLinkContextMenu(int linkId)
{
    if (ImGui::MenuItem("Delete Link")) {
        DeleteLink(linkId);
        m_NeedsRecompile = true;
    }
}

void ShaderNodeEditorManager::DuplicateNode(const std::shared_ptr<Node>& original)
{
    auto typeIt = m_NodeTypes.find(original->name);
    if (typeIt == m_NodeTypes.end()) return;

    auto copy = CreateNodeOfType(original->name);
    if (!copy) return;

    // Los parámetros viajan por el mismo JSON que se guarda en disco
    cJSON* params = cJSON_CreateObject();
    typeIt->second->SerializeParams(original, params);

    auto tex = m_NodeTextures.find(original->id);
    if (tex != m_NodeTextures.end() && tex->second.image &&
        !tex->second.filepath.empty() && tex->second.filepath != kSystemTexture) {
        cJSON_AddStringToObject(params, "imagePath", tex->second.filepath.c_str());
        LoadImageForNode(copy->id, tex->second.filepath);
    }

    typeIt->second->DeserializeParams(copy, params);
    cJSON_Delete(params);

    const ImVec2 pos = ed::GetNodePosition(original->id);
    ed::SetNodePosition(copy->id, ImVec2(pos.x + 40.0f, pos.y + 40.0f));

    m_NeedsRecompile = true;
}

void ShaderNodeEditorManager::AddNodeAtMenuPos(const std::string& typeName)
{
    auto node = CreateNodeOfType(typeName);
    if (node) {
        ed::SetNodePosition(node->id, ed::ScreenToCanvas(m_CreateMenuPos));
        m_NeedsRecompile = true;
    }
}

void ShaderNodeEditorManager::OnCreateNodeMenu()
{
    // === INPUTS ===
    ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "INPUTS");
    ImGui::Separator();

    if (ImGui::MenuItem("Time (float)")) AddNodeAtMenuPos("Time");
    if (ImGui::MenuItem("Float Value (float)")) AddNodeAtMenuPos("FloatValue");
    if (ImGui::MenuItem("UV Coords (vec2)")) AddNodeAtMenuPos("UV Coords");
    if (ImGui::MenuItem("Color (vec3)")) AddNodeAtMenuPos("Color");
    if (getType() == SHADER_NODE_OBJECT) {
        if (ImGui::MenuItem("Normal (vec3)")) AddNodeAtMenuPos("Normal");
        if (ImGui::MenuItem("Fresnel (float)")) AddNodeAtMenuPos("Fresnel");
    }

    // === TEXTURES ===
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.3f, 1.0f), "TEXTURES");
    ImGui::Separator();

    if (ImGui::MenuItem("Image Texture (vec3)")) AddNodeAtMenuPos("Image");
    if (getType() == SHADER_NODE_OBJECT) {
        if (ImGui::MenuItem("Mesh Texture (vec3)")) AddNodeAtMenuPos("Mesh Texture");
        if (ImGui::MenuItem("Chain Texture (vec3)")) AddNodeAtMenuPos("Internal Texture");
    } else if (getType() == SHADER_NODE_POSTPROCESSING) {
        if (ImGui::MenuItem("Internal Texture (vec3)")) AddNodeAtMenuPos("Internal Texture");
    }

    // === MATH FLOAT ===
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "MATH (float)");
    ImGui::Separator();

    if (ImGui::MenuItem("Add (float)")) AddNodeAtMenuPos("AddFloat");
    if (ImGui::MenuItem("Subtract (float)")) AddNodeAtMenuPos("SubtractFloat");
    if (ImGui::MenuItem("Multiply (float)")) AddNodeAtMenuPos("MultiplyFloat");
    if (ImGui::MenuItem("Sin (float)")) AddNodeAtMenuPos("Sin");
    if (ImGui::MenuItem("Cos (float)")) AddNodeAtMenuPos("Cos");
    if (ImGui::MenuItem("Smoothstep (float)")) AddNodeAtMenuPos("Smoothstep");
    if (ImGui::MenuItem("Step (float)")) AddNodeAtMenuPos("Step");
    if (ImGui::MenuItem("Floor (float)")) AddNodeAtMenuPos("Floor");
    if (ImGui::MenuItem("Fract (float)")) AddNodeAtMenuPos("Fract");
    if (ImGui::MenuItem("Abs (float)")) AddNodeAtMenuPos("Abs");

    // === MATH VEC3 ===
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "MATH (vec3)");
    ImGui::Separator();

    if (ImGui::MenuItem("Add (vec3)")) AddNodeAtMenuPos("Add");
    if (ImGui::MenuItem("Subtract (vec3)")) AddNodeAtMenuPos("Subtract");
    if (ImGui::MenuItem("Multiply (vec3)")) AddNodeAtMenuPos("Multiply");
    if (ImGui::MenuItem("Mix (vec3)")) AddNodeAtMenuPos("Mix");

    // === CONVERTERS ===
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(0.8f, 1.0f, 0.5f, 1.0f), "CONVERTERS");
    ImGui::Separator();

    if (ImGui::MenuItem("Vec2 Compose (float->vec2)")) AddNodeAtMenuPos("Vec2Compose");
    if (ImGui::MenuItem("Vec2 Decompose (vec2->float)")) AddNodeAtMenuPos("Vec2Decompose");
    if (ImGui::MenuItem("Vec3 Compose (float->vec3)")) AddNodeAtMenuPos("Vec3Compose");
    if (ImGui::MenuItem("Vec3 Decompose (vec3->float)")) AddNodeAtMenuPos("Vec3Decompose");

    // === UV OPERATIONS ===
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.5f, 1.0f), "UV (vec2)");
    ImGui::Separator();

    if (ImGui::MenuItem("UV Offset (vec2)")) AddNodeAtMenuPos("UV Offset");
    if (ImGui::MenuItem("UV Scale (vec2)")) AddNodeAtMenuPos("UV Scale");
    if (ImGui::MenuItem("UV Rotate (vec2)")) AddNodeAtMenuPos("UV Rotate");
    if (ImGui::MenuItem("Pixelate (vec2)")) AddNodeAtMenuPos("Pixelate");
    if (ImGui::MenuItem("UV Radial (vec2->float)")) AddNodeAtMenuPos("UVRadial");

    // === EFFECTS ===
    ImGui::Spacing();
    ImGui::TextColored(ImVec4(1.0f, 0.5f, 1.0f, 1.0f), "EFFECTS");
    ImGui::Separator();

    if (ImGui::MenuItem("Gradient (vec3)")) AddNodeAtMenuPos("Gradient");
    if (ImGui::MenuItem("Grayscale (vec3->float)")) AddNodeAtMenuPos("Grayscale");
    if (ImGui::MenuItem("Invert (vec3)")) AddNodeAtMenuPos("Invert");
    if (ImGui::MenuItem("Contrast (vec3)")) AddNodeAtMenuPos("Contrast");
    if (ImGui::MenuItem("Saturation (vec3)")) AddNodeAtMenuPos("Saturation");
    if (ImGui::MenuItem("Vignette (float)")) AddNodeAtMenuPos("Vignette");
    if (ImGui::MenuItem("Posterize (vec3)")) AddNodeAtMenuPos("Posterize");
    if (ImGui::MenuItem("Scale Color (vec3 * float)")) AddNodeAtMenuPos("ScaleColor");
    if (ImGui::MenuItem("Noise (vec2->float)")) AddNodeAtMenuPos("Noise");
}

void ShaderNodeEditorManager::Clear()
{
    // Liberar userData y texturas propias de los nodos
    ReleaseAllNodes();

    // Llamar al Clear de la clase base para limpiar nodos/links/pins
    NodeEditorManager::Clear();

    m_DefaultLayout = false;

    // Recompilar shader después de limpiar
    m_NeedsRecompile = true;
}

void ShaderNodeEditorManager::Update()
{
    if (!m_GLReady) {
        SetupQuad();
        m_GLReady = true;
        m_NeedsRecompile = true;
    }

    if (m_NeedsRecompile) {
        CompileShader();
        m_NeedsRecompile = false;
    }
}

// Enlaza las texturas de los nodos (Image, Internal Texture, Mesh Texture) a unidades consecutivas.
// Devuelve cuántas unidades se han usado, para liberarlas después con UnbindNodeTextures().
int ShaderNodeEditorManager::BindNodeTextures()
{
    int textureUnit = 0;
    for (auto& node : GetNodes()) {
        if (node->name != "Image" && node->name != "Internal Texture" && node->name != "Mesh Texture") continue;

        auto it = m_NodeTextures.find(node->id);
        if (it == m_NodeTextures.end() || !it->second.image) continue;

        const GLuint texID = it->second.image->getOGLTextureID();
        if (texID == 0) continue;

        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture(GL_TEXTURE_2D, texID);

        const std::string uniformName = "u_Texture" + std::to_string(node->id);
        const GLint texLoc = glGetUniformLocation(m_ShaderProgram, uniformName.c_str());
        if (texLoc != -1) {
            glUniform1i(texLoc, textureUnit);
        }

        textureUnit++;
    }
    return textureUnit;
}

void ShaderNodeEditorManager::UnbindNodeTextures(int textureUnits)
{
    for (int i = 0; i < textureUnits; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

// Los parámetros de los nodos (colores, valores...) viajan como uniforms: arrastrar un slider no
// obliga a recompilar el shader. Se leen del userData de cada nodo en cada dibujado.
void ShaderNodeEditorManager::UploadUniforms()
{
    for (const auto& u : m_Uniforms) {
        if (u.location < 0 || !u.data) continue;

        switch (u.components) {
            case 1: glUniform1fv(u.location, 1, u.data); break;
            case 2: glUniform2fv(u.location, 1, u.data); break;
            case 3: glUniform3fv(u.location, 1, u.data); break;
            case 4: glUniform4fv(u.location, 1, u.data); break;
            default: break;
        }
    }
}

void ShaderNodeEditorManager::AddGlslHelper(const std::string& key, const std::string& glslCode)
{
    for (auto& helper : m_GlslHelpers) {
        if (helper.first == key) return;   // ya declarada: se conserva su posición
    }
    m_GlslHelpers.emplace_back(key, glslCode);
}

std::string ShaderNodeEditorManager::AddUniform(const std::string& name, int components, const float* data)
{
    for (const auto& u : m_Uniforms) {
        if (u.name == name) return name;
    }

    ShaderUniform uniform;
    uniform.name = name;
    uniform.components = components;
    uniform.data = data;
    m_Uniforms.push_back(uniform);
    return name;
}

void ShaderNodeEditorManager::RenderEffect(GLuint fb)
{
    if (!m_ShaderProgram) return;

    Components::get()->Render()->ChangeOpenGLFramebuffer(fb);
    Components::get()->Render()->ChangeOpenGLProgram(m_ShaderProgram);

    float time = Brakeza::get()->getExecutionTime();
    GLint timeLoc = glGetUniformLocation(m_ShaderProgram, "u_Time");
    if (timeLoc != -1) glUniform1f(timeLoc, time);

    UploadUniforms();
    const int textureUnits = BindNodeTextures();

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);

    UnbindNodeTextures(textureUnits);

    Components::get()->Render()->ChangeOpenGLProgram(0);
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
    // La textura de sistema la gestiona el motor: no hay nada que cargar (ni que descartar)
    if (filepath == kSystemTexture) return;

    DeleteImageForNode(nodeId);

    try {
        m_NodeTextures[nodeId].image = new Image(filepath);
        m_NodeTextures[nodeId].image->setMipmaps(true);     // texturas de nodos Image sobre geometria (carteles): sin mipmaps se ven con aliasing
        m_NodeTextures[nodeId].filepath = filepath;
    } catch (const std::exception& e) {
        LOG_ERROR("Exception loading texture: %s", e.what());
        m_NodeTextures.erase(nodeId);
    }

    // El sampler del nodo se declara en el GLSL generado: hay que regenerarlo
    m_NeedsRecompile = true;
}

void ShaderNodeEditorManager::DeleteImageForNode(int nodeId)
{
    auto it = m_NodeTextures.find(nodeId);
    if (it != m_NodeTextures.end()) {
        ReleaseNodeImage(it->second);
        m_NodeTextures.erase(it);
        LOG_MESSAGE("Deleted texture for node %d", nodeId);
    }
}

std::string ShaderNodeEditorManager::GenerateShaderCode()
{
    m_NodeOutputs.clear();
    m_Traversing.clear();
    m_Uniforms.clear();
    m_GlslHelpers.clear();

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
        // Emision (G-Buffer COLOR_ATTACHMENT3: rgb = color que emite, a = intensidad). Solo se escribe si Mesh3D::RunObjectShaders activa el 4o
        // draw buffer (mesh emisivo); con 3 draw buffers la salida se descarta.
        code << "layout (location = 3) out vec4 gEmission;\n";
        code << "uniform float u_Emission;\n";
    }
    code << "uniform float u_Time;\n";

    if (getType() == SHADER_NODE_OBJECT) {
        code << "uniform vec2 u_ScreenSize;\n";
        code << "uniform vec3 u_ViewPos;\n";   // posición de la cámara (Fresnel...)
    }

    // Declarar uniforms de texturas
    for (auto& node : GetNodes()) {
        if (node->name != "Image" && node->name != "Internal Texture" && node->name != "Mesh Texture") continue;

        auto it = m_NodeTextures.find(node->id);
        if (it != m_NodeTextures.end() && it->second.image) {
            code << "uniform sampler2D u_Texture" << node->id << ";\n";
        }
    }

    // Buscar nodo Output
    std::shared_ptr<Node> outputNode = nullptr;
    for (auto& node : GetNodes()) {
        if (node->name == "Output") {
            outputNode = node;
            break;
        }
    }

    if (!outputNode || outputNode->inputs.empty()) {
        code << "\nvoid main() {\n";
        if (getType() == SHADER_NODE_OBJECT) {
            code << "    gPosition = v_FragPos;\n";
            code << "    gNormal = normalize(v_Normal);\n";
            code << "    gAlbedoSpec = vec4(1.0, 0.0, 1.0, 1.0);\n";
            code << "    gEmission = vec4(0.0);\n";
        } else {
            code << "    FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n";
        }
        code << "}\n";
        return code.str();
    }

    // El cuerpo se genera primero: al recorrer el grafo los nodos van registrando sus uniforms
    std::stringstream body;
    std::string finalColor = TraverseNode(outputNode->inputs[0]->id, body);

    for (const auto& u : m_Uniforms) {
        const char* glslType = u.components == 1 ? "float" : u.components == 2 ? "vec2" : u.components == 3 ? "vec3" : "vec4";
        code << "uniform " << glslType << " " << u.name << ";\n";
    }

    // Funciones auxiliares que piden los nodos (p. ej. Noise), antes de main()
    for (const auto& helper : m_GlslHelpers) {
        code << "\n" << helper.second;
    }

    code << "\nvoid main() {\n";
    code << body.str();

    if (getType() == SHADER_NODE_OBJECT) {
        code << "    gPosition = v_FragPos;\n";
        code << "    gNormal = normalize(v_Normal);\n";
        if (finalColor.empty()) {
            code << "    gAlbedoSpec = vec4(0.0, 1.0, 1.0, 1.0);\n";
        } else {
            code << "    gAlbedoSpec = vec4(" << finalColor << ", 1.0);\n";
        }
        code << "    gEmission = vec4(gAlbedoSpec.rgb, u_Emission);\n";
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

    // Cada SALIDA se genera una sola vez. La clave es el pin, no el nodo: un nodo con varias
    // salidas (Vec2 Decompose, Image: Color/Alpha...) devolvería la variable de la primera siempre.
    auto cached = m_NodeOutputs.find(sourcePin->id);
    if (cached != m_NodeOutputs.end()) {
        return cached->second;
    }

    // Defensa ante ciclos (ficheros editados a mano): un nodo que ya se está generando no se reentra
    if (!m_Traversing.insert(sourceNode->id).second) {
        LOG_ERROR("[ShaderNodeEditorManager] Cycle detected at node '%s' (id %d)", sourceNode->name.c_str(), sourceNode->id);
        return "";
    }

    std::string result;
    auto it = m_NodeTypes.find(sourceNode->name);
    if (it != m_NodeTypes.end()) {
        result = it->second->GenerateCode(sourceNode, sourcePin, code, this);
    }

    m_Traversing.erase(sourceNode->id);

    if (!result.empty()) {
        m_NodeOutputs[sourcePin->id] = result;
    }
    return result;
}

void ShaderNodeEditorManager::CompileShader()
{
    if (m_ShaderProgram) {
        glDeleteProgram(m_ShaderProgram);
        m_ShaderProgram = 0;
    }
    m_LastCompileError.clear();

    std::string fragmentCode = GenerateShaderCode();
    m_LastShaderCode = fragmentCode;

    LOG_MESSAGE("========== GENERATED SHADER CODE ==========\n%s", fragmentCode.c_str());

    // Vertex shader según el tipo de shader
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

    auto compileStage = [this](GLenum kind, const char* source, const char* label) -> GLuint {
        GLuint shader = glCreateShader(kind);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint ok = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            GLint length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
            std::vector<char> log(length > 1 ? length : 1, '\0');
            if (length > 1) glGetShaderInfoLog(shader, length, nullptr, log.data());

            m_LastCompileError = std::string(label) + ": " + log.data();
            LOG_ERROR("[ShaderNodeEditorManager] %s error: %s", label, log.data());
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    };

    GLuint vertexShader = compileStage(GL_VERTEX_SHADER, vertexCode, "Vertex shader");
    if (!vertexShader) return;

    GLuint fragmentShader = compileStage(GL_FRAGMENT_SHADER, fragmentCode.c_str(), "Fragment shader");
    if (!fragmentShader) {
        glDeleteShader(vertexShader);
        return;
    }

    // Linkear programa
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint linked = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        GLint length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> log(length > 1 ? length : 1, '\0');
        if (length > 1) glGetProgramInfoLog(program, length, nullptr, log.data());

        m_LastCompileError = std::string("Link: ") + log.data();
        LOG_ERROR("[ShaderNodeEditorManager] Shader link error: %s", log.data());
        glDeleteProgram(program);
        return;
    }

    m_ShaderProgram = program;
    for (auto& u : m_Uniforms) {
        u.location = glGetUniformLocation(m_ShaderProgram, u.name.c_str());
    }

    LOG_MESSAGE("Shader compiled successfully! ID: %u", m_ShaderProgram);
}

cJSON* ShaderNodeEditorManager::SerializeNodeUserData(std::shared_ptr<Node> node) {
    cJSON* userData = cJSON_CreateObject();

    // Imagen del nodo Image. Las texturas de sistema (Internal/Mesh Texture) no se guardan: las
    // reasigna el motor en cada frame y recargarlas como imagen de disco rompería el nodo.
    auto it = m_NodeTextures.find(node->id);
    if (it != m_NodeTextures.end() && !it->second.filepath.empty() && it->second.filepath != kSystemTexture) {
        cJSON_AddStringToObject(userData, "imagePath", it->second.filepath.c_str());
    }

    // Parámetros propios de cada tipo de nodo (color, valores, tipo de textura...)
    auto typeIt = m_NodeTypes.find(node->name);
    if (typeIt != m_NodeTypes.end()) {
        typeIt->second->SerializeParams(node, userData);
    }

    return userData;
}

void ShaderNodeEditorManager::DeserializeNodeUserData(std::shared_ptr<Node> node, cJSON* userDataJson) {
    if (!userDataJson) return;

    const std::string imagePath = NodeJson::String(userDataJson, "imagePath");
    if (!imagePath.empty()) {
        LoadImageForNode(node->id, imagePath);
    }

    auto typeIt = m_NodeTypes.find(node->name);
    if (typeIt != m_NodeTypes.end()) {
        typeIt->second->DeserializeParams(node, userDataJson);
    }
}

void ShaderNodeEditorManager::SerializeCustomData(cJSON* root)
{
    // El último GLSL generado es solo informativo (se regenera siempre desde el grafo)
    if (!m_LastShaderCode.empty()) {
        cJSON_AddStringToObject(root, "lastShaderCode", m_LastShaderCode.c_str());
    }
}

void ShaderNodeEditorManager::DeserializeCustomData(cJSON* root) {
    // Verificar que es un shader editor
    const std::string editorType = NodeJson::String(root, "editorType");
    if (editorType != "shader" && editorType != "base") {
        LOG_ERROR("[ShaderNodeEditorManager] Unexpected editorType '%s'", editorType.c_str());
    }

    // Marcar para recompilar (el código GLSL se regenera desde los nodos)
    m_NeedsRecompile = true;
}

std::shared_ptr<Node> ShaderNodeEditorManager::CreateNodeFromJSON(cJSON* nodeJson) {
    if (!nodeJson || !cJSON_GetObjectItem(nodeJson, "id")) return nullptr;

    const std::string typeName = NodeJson::String(nodeJson, "type");
    std::shared_ptr<Node> node = CreateNodeOfType(typeName);

    if (!node) {
        return NodeEditorManager::CreateNodeFromJSON(nodeJson);
    }

    const int oldId = node->id;
    const int newId = NodeJson::Int(nodeJson, "id", oldId);
    node->id = newId;

    // Las texturas externas se crearon con el id provisional: pasarlas al id del fichero
    if (node->name == "Internal Texture" || node->name == "Mesh Texture") {
        auto oldIt = m_NodeTextures.find(oldId);
        if (oldIt != m_NodeTextures.end()) {
            if (oldId != newId) {
                m_NodeTextures[newId] = oldIt->second;
                m_NodeTextures.erase(oldId);
            }
        } else {
            SetExternalTextureForNode(newId, 0);
        }
    }

    // Los pins los crea el propio tipo de nodo (nombres y tipos actuales); del fichero solo se
    // toman los ids, para que los enlaces guardados sigan apuntando a los pins correctos.
    auto restorePinIds = [&](std::vector<std::shared_ptr<Pin>>& pins, const char* key) {
        size_t index = 0;
        cJSON* pinJson = nullptr;
        cJSON_ArrayForEach(pinJson, cJSON_GetObjectItem(nodeJson, key)) {
            if (index >= pins.size()) break;
            pins[index]->id = NodeJson::Int(pinJson, "id", pins[index]->id);
            index++;
        }
        for (auto& pin : pins) {
            pin->nodeId = newId;
        }
    };
    restorePinIds(node->inputs, "inputs");
    restorePinIds(node->outputs, "outputs");

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
    m_NodeTextures[nodeId].filepath = kSystemTexture;

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
    for (auto& node : GetNodes()) {
        if (node->name == "Internal Texture") {
            auto it = m_NodeTextures.find(node->id);
            if (it != m_NodeTextures.end() && it->second.image) {
                it->second.image->setOGLTextureID(textureId);
            } else {
                SetExternalTextureForNode(node->id, textureId);
                m_NeedsRecompile = true;   // el sampler se declara solo si el nodo tiene imagen
            }
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
    const glm::mat4& projection,
    GLuint indexBuffer,
    GLsizei indexCount
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

    float time = Brakeza::get()->getExecutionTime();
    GLint timeLoc = glGetUniformLocation(m_ShaderProgram, "u_Time");
    if (timeLoc != -1) glUniform1f(timeLoc, time);

    GLint emissionLoc = glGetUniformLocation(m_ShaderProgram, "u_Emission");
    if (emissionLoc != -1) glUniform1f(emissionLoc, m_Emission);

    GLint screenSizeLoc = glGetUniformLocation(m_ShaderProgram, "u_ScreenSize");
    if (screenSizeLoc != -1) {
        auto window = Components::get()->Window();
        glUniform2f(screenSizeLoc, (float)window->getWidthRender(), (float)window->getHeightRender());
    }

    GLint viewPosLoc = glGetUniformLocation(m_ShaderProgram, "u_ViewPos");
    if (viewPosLoc != -1) {
        // La posición de la cámara es la traslación de la inversa de la matriz de vista
        const glm::vec3 viewPos = glm::vec3(glm::inverse(view)[3]);
        glUniform3f(viewPosLoc, viewPos.x, viewPos.y, viewPos.z);
    }

    UploadUniforms();
    const int textureUnits = BindNodeTextures();

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

    ShaderBaseOpenGL::DrawMeshGeometry(GL_TRIANGLES, indexBuffer, indexCount, vertexCount, "NodeEditorManager:RenderMesh");

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    UnbindNodeTextures(textureUnits);

    Components::get()->Render()->ChangeOpenGLProgram(0);
    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
}

void ShaderNodeEditorManager::RenderShaderDebugPanel()
{
    ImGui::Spacing();

    if (!m_LastCompileError.empty()) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
        ImGui::TextWrapped("Shader error: %s", m_LastCompileError.c_str());
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }

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