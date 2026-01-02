#include "ShaderNodeEditor.h"
#include "../../include/Components/Components.h"
#include "../../include/Render/Image.h"
#include <cmath>

ShaderNodeEditor::ShaderNodeEditor()
    : NodeEditorManager(nullptr)
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

    if (m_ShaderProgram) glDeleteProgram(m_ShaderProgram);
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
}

void ShaderNodeEditor::RegisterNodeTypes()
{
    // ==================== GENERADORES ====================

    // UV Coords
    RegisterNode("UV Coords", {
        "UV Coords",
        ImVec4(0.3f, 0.6f, 0.3f, 1.0f),
        nullptr,
        nullptr,
        nullptr,
        [](std::shared_ptr<Node>& node, std::shared_ptr<Pin>& sourcePin,
           std::stringstream& code, ShaderNodeEditor* editor) -> std::string {
            std::string varName = "var" + std::to_string(node->id);
            code << "    vec2 " << varName << " = v_TexCoord;\n";
            return varName;
        }
    });

    // Color
    RegisterNode("Color", {
        "Color",
        ImVec4(0.8f, 0.4f, 0.2f, 1.0f),

        [](std::shared_ptr<Node>& node) {
            node->userData = new ImVec4(1.0f, 0.5f, 0.2f, 1.0f);
        },

        [](std::shared_ptr<Node>& node) {
            if (node->userData) {
                delete (ImVec4*)node->userData;
                node->userData = nullptr;
            }
        },

        [](std::shared_ptr<Node>& node, ShaderNodeEditor* editor) {
            if (node->userData) {
                ImVec4* color = (ImVec4*)node->userData;
                std::string colorId = "##color" + std::to_string(node->id);
                if (ImGui::ColorEdit3(colorId.c_str(), (float*)color)) {
                    editor->SetNeedsRecompile();
                }
            }
        },

        [](std::shared_ptr<Node>& node, std::shared_ptr<Pin>& sourcePin,
           std::stringstream& code, ShaderNodeEditor* editor) -> std::string {
            std::string varName = "var" + std::to_string(node->id);
            ImVec4* color = (ImVec4*)node->userData;
            if (color) {
                code << "    vec3 " << varName << " = vec3("
                     << color->x << ", " << color->y << ", " << color->z << ");\n";
            } else {
                code << "    vec3 " << varName << " = vec3(1.0, 1.0, 1.0);\n";
            }
            return varName;
        }
    });

    // Time
    RegisterNode("Time", {
        "Time",
        ImVec4(0.3f, 0.5f, 0.6f, 1.0f),
        nullptr,
        nullptr,
        nullptr,

        [](std::shared_ptr<Node>& node, std::shared_ptr<Pin>& sourcePin,
           std::stringstream& code, ShaderNodeEditor* editor) -> std::string {
            std::string varName = "var" + std::to_string(node->id);
            code << "    float " << varName << " = u_Time;\n";
            return varName;
        }
    });

    // Image
    RegisterNode("Image", {
        "Image",
        ImVec4(0.7f, 0.3f, 0.7f, 1.0f),

        [](std::shared_ptr<Node>& node) {
            node->userData = new std::string("");
        },

        [](std::shared_ptr<Node>& node) {
            if (node->userData) {
                delete (std::string*)node->userData;
                node->userData = nullptr;
            }
        },

        [](std::shared_ptr<Node>& node, ShaderNodeEditor* editor) {
            std::string* filepath = (std::string*)node->userData;
            if (!filepath) return;

            ImGui::PushItemWidth(200);

            auto& textures = editor->GetNodeTextures();

            if (textures.find(node->id) != textures.end()) {
                auto& tex = textures[node->id];
                if (tex.image) {
                    ImGui::Text("Loaded: %dx%d", tex.image->width(), tex.image->height());
                    float previewSize = 200.0f;
                    float aspect = (float)tex.image->width() / (float)tex.image->height();
                    ImVec2 imageSize;

                    if (aspect > 1.0f) {
                        imageSize = ImVec2(previewSize, previewSize / aspect);
                    } else {
                        imageSize = ImVec2(previewSize * aspect, previewSize);
                    }

                    ImGui::Image(tex.image->getOGLImTexture(), imageSize);

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM)) {
                            auto selection = static_cast<char*>(payload->Data);
                            auto fullPath = Config::get()->IMAGES_FOLDER + selection;
                            *filepath = fullPath;
                            editor->LoadImageForNode(node->id, fullPath);
                        }
                        ImGui::EndDragDropTarget();
                    }

                    if (ImGui::Button("Remove Image", ImVec2(200, 0))) {
                        editor->DeleteImageForNode(node->id);
                        *filepath = "";
                        editor->SetNeedsRecompile();
                    }
                }
            } else {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No image loaded");
                ImGui::Spacing();

                ImVec2 dropZoneSize(200, 150);
                ImVec2 cursorPos = ImGui::GetCursorScreenPos();
                ImGui::InvisibleButton("##dropzone", dropZoneSize);

                ImDrawList* drawList = ImGui::GetWindowDrawList();
                ImU32 borderColor = ImGui::GetColorU32(ImVec4(0.5f, 0.5f, 0.5f, 0.5f));

                if (ImGui::IsItemHovered() && ImGui::GetDragDropPayload()) {
                    borderColor = ImGui::GetColorU32(ImVec4(0.3f, 0.8f, 0.3f, 1.0f));
                }

                for (int i = 0; i < 4; i++) {
                    ImVec2 p1, p2;
                    switch(i) {
                        case 0: p1 = cursorPos; p2 = ImVec2(cursorPos.x + dropZoneSize.x, cursorPos.y); break;
                        case 1: p1 = ImVec2(cursorPos.x + dropZoneSize.x, cursorPos.y);
                                p2 = ImVec2(cursorPos.x + dropZoneSize.x, cursorPos.y + dropZoneSize.y); break;
                        case 2: p1 = ImVec2(cursorPos.x + dropZoneSize.x, cursorPos.y + dropZoneSize.y);
                                p2 = ImVec2(cursorPos.x, cursorPos.y + dropZoneSize.y); break;
                        case 3: p1 = ImVec2(cursorPos.x, cursorPos.y + dropZoneSize.y); p2 = cursorPos; break;
                    }

                    float len = sqrtf(powf(p2.x - p1.x, 2) + powf(p2.y - p1.y, 2));
                    int segments = (int)(len / 10.0f);
                    for (int j = 0; j < segments; j += 2) {
                        float t1 = (float)j / (float)segments;
                        float t2 = (float)(j + 1) / (float)segments;
                        ImVec2 segStart = ImVec2(p1.x + (p2.x - p1.x) * t1, p1.y + (p2.y - p1.y) * t1);
                        ImVec2 segEnd = ImVec2(p1.x + (p2.x - p1.x) * t2, p1.y + (p2.y - p1.y) * t2);
                        drawList->AddLine(segStart, segEnd, borderColor, 2.0f);
                    }
                }

                const char* dropText = "Drop image here";
                ImVec2 textSize = ImGui::CalcTextSize(dropText);
                ImVec2 textPos = ImVec2(
                    cursorPos.x + (dropZoneSize.x - textSize.x) * 0.5f,
                    cursorPos.y + (dropZoneSize.y - textSize.y) * 0.5f
                );
                drawList->AddText(textPos, ImGui::GetColorU32(ImVec4(0.6f, 0.6f, 0.6f, 1.0f)), dropText);

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM)) {
                        auto selection = static_cast<char*>(payload->Data);
                        auto fullPath = Config::get()->IMAGES_FOLDER + selection;
                        *filepath = fullPath;
                        editor->LoadImageForNode(node->id, fullPath);
                    }
                    ImGui::EndDragDropTarget();
                }
            }

            if (!filepath->empty()) {
                ImGui::Spacing();
                ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 200);
                ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Path:");
                ImGui::TextWrapped("%s", filepath->c_str());
                ImGui::PopTextWrapPos();
            }

            ImGui::PopItemWidth();
        },

        [](std::shared_ptr<Node>& node, std::shared_ptr<Pin>& sourcePin,
           std::stringstream& code, ShaderNodeEditor* editor) -> std::string {
            std::string varName = "var" + std::to_string(node->id);
            auto& textures = editor->GetNodeTextures();

            if (textures.find(node->id) != textures.end() && textures[node->id].image) {
                if (sourcePin->name == "Alpha") {
                    code << "    float " << varName << " = texture(u_Texture" << node->id
                         << ", v_TexCoord).a;\n";
                } else {
                    code << "    vec3 " << varName << " = texture(u_Texture" << node->id
                         << ", v_TexCoord).rgb;\n";
                }
            } else {
                code << "    vec3 " << varName << " = vec3(1.0, 0.0, 1.0);\n";
            }
            return varName;
        }
    });

    // ==================== PROCESADORES ====================

    // Mix
    RegisterNode("Mix", {
        "Mix",
        ImVec4(0.4f, 0.4f, 0.7f, 1.0f),

        [](std::shared_ptr<Node>& node) {
            node->userData = new float(0.5f);
        },

        [](std::shared_ptr<Node>& node) {
            if (node->userData) {
                delete (float*)node->userData;
                node->userData = nullptr;
            }
        },

        [](std::shared_ptr<Node>& node, ShaderNodeEditor* editor) {
            if (node->userData) {
                float* factor = (float*)node->userData;
                if (ImGui::SliderFloat("Factor", factor, 0.0f, 1.0f)) {
                    editor->SetNeedsRecompile();
                }
            }
        },

        nullptr // Se maneja en TraverseNode
    });

    // Gradient
    RegisterNode("Gradient", {
        "Gradient",
        ImVec4(0.5f, 0.3f, 0.6f, 1.0f),
        nullptr, nullptr, nullptr, nullptr
    });

    // Multiply
    RegisterNode("Multiply", {
        "Multiply",
        ImVec4(0.6f, 0.5f, 0.3f, 1.0f),
        nullptr, nullptr, nullptr, nullptr
    });

    // Add
    RegisterNode("Add", {
        "Add",
        ImVec4(0.4f, 0.6f, 0.5f, 1.0f),
        nullptr, nullptr, nullptr, nullptr
    });

    // Subtract
    RegisterNode("Subtract", {
        "Subtract",
        ImVec4(0.6f, 0.4f, 0.5f, 1.0f),
        nullptr, nullptr, nullptr, nullptr
    });

    // ==================== NODOS MATEMÁTICOS ====================

    // Sin
    RegisterNode("Sin", {
        "Sin",
        ImVec4(0.3f, 0.7f, 0.8f, 1.0f),
        nullptr, nullptr, nullptr, nullptr
    });

    // Cos
    RegisterNode("Cos", {
        "Cos",
        ImVec4(0.3f, 0.8f, 0.7f, 1.0f),
        nullptr, nullptr, nullptr, nullptr
    });

    // Smoothstep
    RegisterNode("Smoothstep", {
        "Smoothstep",
        ImVec4(0.5f, 0.7f, 0.4f, 1.0f),

        [](std::shared_ptr<Node>& node) {
            // userData = {edge0, edge1}
            node->userData = new ImVec2(0.0f, 1.0f);
        },

        [](std::shared_ptr<Node>& node) {
            if (node->userData) {
                delete (ImVec2*)node->userData;
                node->userData = nullptr;
            }
        },

        [](std::shared_ptr<Node>& node, ShaderNodeEditor* editor) {
            if (node->userData) {
                ImVec2* edges = (ImVec2*)node->userData;
                if (ImGui::DragFloat("Edge 0", &edges->x, 0.01f, 0.0f, 1.0f)) {
                    editor->SetNeedsRecompile();
                }
                if (ImGui::DragFloat("Edge 1", &edges->y, 0.01f, 0.0f, 1.0f)) {
                    editor->SetNeedsRecompile();
                }
            }
        },

        nullptr // Se maneja en TraverseNode
    });

    // ==================== NODOS UV ====================

    // UV Offset
    RegisterNode("UV Offset", {
        "UV Offset",
        ImVec4(0.4f, 0.7f, 0.6f, 1.0f),
        nullptr, nullptr, nullptr, nullptr
    });

    // UV Scale
    RegisterNode("UV Scale", {
        "UV Scale",
        ImVec4(0.5f, 0.6f, 0.7f, 1.0f),
        nullptr, nullptr, nullptr, nullptr
    });

    // UV Rotate
    RegisterNode("UV Rotate", {
        "UV Rotate",
        ImVec4(0.6f, 0.7f, 0.5f, 1.0f),
        nullptr, nullptr, nullptr, nullptr
    });

    // ==================== NODO OUTPUT ====================

    RegisterNode("Output", {
        "Output",
        ImVec4(0.8f, 0.2f, 0.2f, 1.0f),
        nullptr, nullptr, nullptr, nullptr
    });
}

void ShaderNodeEditor::RegisterNode(const std::string& typeName, const NodeDefinition& definition)
{
    m_NodeRegistry[typeName] = definition;
    LOG_MESSAGE("[ShaderNodeEditor] Registered node type: %s", typeName.c_str());
}

std::shared_ptr<Node> ShaderNodeEditor::CreateNodeOfType(const std::string& typeName)
{
    auto it = m_NodeRegistry.find(typeName);
    if (it == m_NodeRegistry.end()) {
        LOG_MESSAGE("[ShaderNodeEditor] Unknown node type: %s", typeName.c_str());
        return nullptr;
    }

    auto& def = it->second;
    auto node = CreateNode(def.name, def.color);

    if (def.onCreate) {
        def.onCreate(node);
    }

    // Añadir pins según el tipo de nodo
    if (typeName == "UV Coords") {
        AddOutputPin(node, "UV", PinType::Vector);
    }
    else if (typeName == "Color") {
        AddOutputPin(node, "RGB", PinType::Vector);
    }
    else if (typeName == "Time") {
        AddOutputPin(node, "Time", PinType::Float);
    }
    else if (typeName == "Image") {
        AddInputPin(node, "UV", PinType::Vector);
        AddOutputPin(node, "Color", PinType::Vector);
        AddOutputPin(node, "Alpha", PinType::Float);
    }
    else if (typeName == "Mix") {
        AddInputPin(node, "A", PinType::Vector);
        AddInputPin(node, "B", PinType::Vector);
        AddInputPin(node, "Factor", PinType::Float);
        AddOutputPin(node, "Result", PinType::Vector);
    }
    else if (typeName == "Gradient") {
        AddInputPin(node, "UV", PinType::Vector);
        AddOutputPin(node, "Color", PinType::Vector);
    }
    else if (typeName == "Multiply" || typeName == "Add" || typeName == "Subtract") {
        AddInputPin(node, "A", PinType::Vector);
        AddInputPin(node, "B", PinType::Vector);
        AddOutputPin(node, "Result", PinType::Vector);
    }
    else if (typeName == "Sin" || typeName == "Cos") {
        AddInputPin(node, "Input", PinType::Float);
        AddOutputPin(node, "Output", PinType::Float);
    }
    else if (typeName == "Smoothstep") {
        AddInputPin(node, "Value", PinType::Float);
        AddOutputPin(node, "Output", PinType::Float);
    }
    else if (typeName == "UV Offset" || typeName == "UV Scale") {
        AddInputPin(node, "UV", PinType::Vector);
        AddInputPin(node, "Offset", PinType::Vector);
        AddOutputPin(node, "UV", PinType::Vector);
    }
    else if (typeName == "UV Rotate") {
        AddInputPin(node, "UV", PinType::Vector);
        AddInputPin(node, "Angle", PinType::Float);
        AddOutputPin(node, "UV", PinType::Vector);
    }
    else if (typeName == "Output") {
        AddInputPin(node, "Color", PinType::Vector);
    }

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

    if (node->inputs.empty() && !node->outputs.empty()) {
        ImGui::Text("[GEN] %s", node->name.c_str());
    } else if (node->outputs.empty() && !node->inputs.empty()) {
        ImGui::Text("[OUT] %s", node->name.c_str());
    } else {
        ImGui::Text("[PRO] %s", node->name.c_str());
    }

    auto it = m_NodeRegistry.find(node->name);
    if (it != m_NodeRegistry.end() && it->second.renderUI) {
        it->second.renderUI(node, this);
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
                    // Llamar onDelete si existe
                    auto it = m_NodeRegistry.find(node->name);
                    if (it != m_NodeRegistry.end() && it->second.onDelete) {
                        it->second.onDelete(node);
                    }

                    // Limpiar imagen si es nodo Image
                    if (node->name == "Image") {
                        DeleteImageForNode(node->id);
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
        // Crear nueva imagen usando tu clase Image
        //Image* img = new Image(filepath);
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

    std::string varName = "var" + std::to_string(sourceNode->id);

    // Usar el generador de código del registro
    auto it = m_NodeRegistry.find(sourceNode->name);
    if (it != m_NodeRegistry.end() && it->second.generateCode) {
        std::string result = it->second.generateCode(sourceNode, sourcePin, code, this);
        if (!result.empty()) {
            m_NodeOutputs[sourceNode->id] = result;
            return result;
        }
    }

    // Manejo especial para nodos con múltiples inputs
    if (sourceNode->name == "Mix") {
        if (sourceNode->inputs.size() >= 3) {
            std::string a = TraverseNode(sourceNode->inputs[0]->id, code);
            std::string b = TraverseNode(sourceNode->inputs[1]->id, code);
            std::string factor = TraverseNode(sourceNode->inputs[2]->id, code);

            if (a.empty()) a = "vec3(0.0)";
            if (b.empty()) b = "vec3(1.0)";
            if (factor.empty()) {
                float* f = (float*)sourceNode->userData;
                factor = std::to_string(f ? *f : 0.5f);
            }

            code << "    vec3 " << varName << " = mix(" << a << ", " << b << ", " << factor << ");\n";
            m_NodeOutputs[sourceNode->id] = varName;
            return varName;
        }
    }
    else if (sourceNode->name == "Gradient") {
        std::string uv = TraverseNode(sourceNode->inputs[0]->id, code);
        if (uv.empty()) uv = "v_TexCoord";

        code << "    vec3 " << varName << " = vec3(" << uv << ".x, " << uv << ".y, 0.5);\n";
        m_NodeOutputs[sourceNode->id] = varName;
        return varName;
    }
    else if (sourceNode->name == "Multiply") {
        if (sourceNode->inputs.size() >= 2) {
            std::string a = TraverseNode(sourceNode->inputs[0]->id, code);
            std::string b = TraverseNode(sourceNode->inputs[1]->id, code);

            if (a.empty()) a = "vec3(1.0)";
            if (b.empty()) b = "vec3(1.0)";

            code << "    vec3 " << varName << " = " << a << " * " << b << ";\n";
            m_NodeOutputs[sourceNode->id] = varName;
            return varName;
        }
    }
    else if (sourceNode->name == "Add") {
        if (sourceNode->inputs.size() >= 2) {
            std::string a = TraverseNode(sourceNode->inputs[0]->id, code);
            std::string b = TraverseNode(sourceNode->inputs[1]->id, code);

            if (a.empty()) a = "vec3(0.0)";
            if (b.empty()) b = "vec3(0.0)";

            code << "    vec3 " << varName << " = " << a << " + " << b << ";\n";
            m_NodeOutputs[sourceNode->id] = varName;
            return varName;
        }
    }
    else if (sourceNode->name == "Subtract") {
        if (sourceNode->inputs.size() >= 2) {
            std::string a = TraverseNode(sourceNode->inputs[0]->id, code);
            std::string b = TraverseNode(sourceNode->inputs[1]->id, code);

            if (a.empty()) a = "vec3(1.0)";
            if (b.empty()) b = "vec3(0.0)";

            code << "    vec3 " << varName << " = " << a << " - " << b << ";\n";
            m_NodeOutputs[sourceNode->id] = varName;
            return varName;
        }
    }
    else if (sourceNode->name == "Sin") {
        std::string input = TraverseNode(sourceNode->inputs[0]->id, code);
        if (input.empty()) input = "0.0";

        code << "    float " << varName << " = sin(" << input << ");\n";
        m_NodeOutputs[sourceNode->id] = varName;
        return varName;
    }
    else if (sourceNode->name == "Cos") {
        std::string input = TraverseNode(sourceNode->inputs[0]->id, code);
        if (input.empty()) input = "0.0";

        code << "    float " << varName << " = cos(" << input << ");\n";
        m_NodeOutputs[sourceNode->id] = varName;
        return varName;
    }
    else if (sourceNode->name == "Smoothstep") {
        std::string value = TraverseNode(sourceNode->inputs[0]->id, code);
        if (value.empty()) value = "0.0";

        ImVec2* edges = (ImVec2*)sourceNode->userData;
        float edge0 = edges ? edges->x : 0.0f;
        float edge1 = edges ? edges->y : 1.0f;

        code << "    float " << varName << " = smoothstep(" << edge0 << ", " << edge1 << ", " << value << ");\n";
        m_NodeOutputs[sourceNode->id] = varName;
        return varName;
    }
    else if (sourceNode->name == "UV Offset") {
        std::string uv = TraverseNode(sourceNode->inputs[0]->id, code);
        std::string offset = TraverseNode(sourceNode->inputs[1]->id, code);

        if (uv.empty()) uv = "v_TexCoord";
        if (offset.empty()) offset = "vec2(0.0)";

        code << "    vec2 " << varName << " = " << uv << " + " << offset << ";\n";
        m_NodeOutputs[sourceNode->id] = varName;
        return varName;
    }
    else if (sourceNode->name == "UV Scale") {
        std::string uv = TraverseNode(sourceNode->inputs[0]->id, code);
        std::string scale = TraverseNode(sourceNode->inputs[1]->id, code);

        if (uv.empty()) uv = "v_TexCoord";
        if (scale.empty()) scale = "vec2(1.0)";

        code << "    vec2 " << varName << " = " << uv << " * " << scale << ";\n";
        m_NodeOutputs[sourceNode->id] = varName;
        return varName;
    }
    else if (sourceNode->name == "UV Rotate") {
        std::string uv = TraverseNode(sourceNode->inputs[0]->id, code);
        std::string angle = TraverseNode(sourceNode->inputs[1]->id, code);

        if (uv.empty()) uv = "v_TexCoord";
        if (angle.empty()) angle = "0.0";

        code << "    vec2 center = vec2(0.5, 0.5);\n";
        code << "    vec2 uv_centered = " << uv << " - center;\n";
        code << "    float s = sin(" << angle << ");\n";
        code << "    float c = cos(" << angle << ");\n";
        code << "    vec2 " << varName << " = vec2(\n";
        code << "        uv_centered.x * c - uv_centered.y * s,\n";
        code << "        uv_centered.x * s + uv_centered.y * c\n";
        code << "    ) + center;\n";
        m_NodeOutputs[sourceNode->id] = varName;
        return varName;
    } else if (sourceNode->name == "Image") {
        std::string varName = "var" + std::to_string(sourceNode->id);

        if (m_NodeTextures.find(sourceNode->id) != m_NodeTextures.end() && m_NodeTextures[sourceNode->id].image) {
            // Obtener las coordenadas UV (si están conectadas)
            std::string uvCoords = "v_TexCoord";  // Por defecto

            if (sourceNode->inputs.size() > 0) {
                std::string customUV = TraverseNode(sourceNode->inputs[0]->id, code);
                if (!customUV.empty()) {
                    uvCoords = customUV;  // Usar UV modificadas
                }
            }

            if (sourcePin->name == "Alpha") {
                code << "    float " << varName << " = texture(u_Texture" << sourceNode->id
                     << ", " << uvCoords << ").a;\n";
            } else {
                code << "    vec3 " << varName << " = texture(u_Texture" << sourceNode->id
                     << ", " << uvCoords << ").rgb;\n";
            }
        } else {
            code << "    vec3 " << varName << " = vec3(1.0, 0.0, 1.0);\n";
        }

        m_NodeOutputs[sourceNode->id] = varName;
        return varName;
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