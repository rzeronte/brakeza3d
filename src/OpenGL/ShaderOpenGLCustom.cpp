//
// Created by edu on 13/12/23.
//

#include <fstream>
#include <utility>
#include "../../include/OpenGL/ShaderOpenGLCustom.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ShaderOpenGLCustom::ShaderOpenGLCustom(
    std::string label,
    const std::string &vertexFilename,
    const std::string &fragmentFilename,
    ShaderCustomTypes type
)
:
    ShaderBaseOpenGL(vertexFilename, fragmentFilename, type == ShaderCustomTypes::SHADER_OBJECT),
    label(std::move(label)),
    type(type),
    fileTypes(dataTypesFileFor(fragmentFilename))
{
    readShaderFiles(vertexFilename, fragmentFilename);
    parseTypesFromFileAttributes();
    createFramebuffer();
}

ShaderOpenGLCustom::ShaderOpenGLCustom(
    std::string label,
    const std::string &vertexFilename,
    const std::string &fragmentFilename,
    ShaderCustomTypes type,
    cJSON *types
)
:
    label(std::move(label)),
    enabled(true),
    ShaderBaseOpenGL(vertexFilename, fragmentFilename, type == ShaderCustomTypes::SHADER_OBJECT),
    fileTypes(ShaderOpenGLCustom::dataTypesFileFor(fragmentFilename)),
    type(type)
{
    readShaderFiles(vertexFilename, fragmentFilename);
    setDataTypesFromJSON(types);
    createFramebuffer();
}

void ShaderOpenGLCustom::readShaderFiles(const std::string &vertexFilename, const std::string &fragmentFilename)
{
    if (!Tools::fileExists(vertexFilename.c_str()) || !Tools::fileExists(fragmentFilename.c_str())) {
        Logging::Message("[error] Cannot open custom shader files (%s, %s)", vertexFilename.c_str(), fragmentFilename.c_str());
    }
    size_t file_size_fs;
    sourceFS = Tools::readFile(fragmentFilename, file_size_fs);

    size_t file_size_vs;
    sourceVS = Tools::readFile(vertexFilename, file_size_vs);
}

bool ShaderOpenGLCustom::existDataType(const char *name, const char *type) const
{
    for (const auto& t: dataTypes) {
        if (t.name == name && t.type == type) {
            return true;
        }
    }

    return false;
}

void ShaderOpenGLCustom::parseTypesFromFileAttributes()
{
    size_t file_size;
    auto contentFile = Tools::readFile(EngineSetup::get()->SHADERS_FOLDER + fileTypes, file_size);
    Logging::Message("Parsing attributes from: '%s'", fileTypes.c_str());

    setDataTypesFromJSON(cJSON_GetObjectItemCaseSensitive(cJSON_Parse(contentFile), "types"));
}

std::string ShaderOpenGLCustom::dataTypesFileFor(std::string basicString)
{
    return ShaderOpenGLCustom::removeFilenameExtension(basicString) + ".json";
}

std::string ShaderOpenGLCustom::removeFilenameExtension(std::string& filename)
{
    size_t dotPosition = filename.find_last_of('.');

    if (dotPosition != std::string::npos) {
        return filename.substr(0, dotPosition);
    }

    return filename;
}

void ShaderOpenGLCustom::setDataTypesFromJSON(cJSON *typesJSON)
{
    cJSON *currentType;
    cJSON_ArrayForEach(currentType, typesJSON) {
        auto name = cJSON_GetObjectItemCaseSensitive(currentType, "name")->valuestring;
        auto type = cJSON_GetObjectItemCaseSensitive(currentType, "type")->valuestring;
        auto value = cJSON_GetObjectItemCaseSensitive(currentType, "value");

        if (!existDataType(name, type)){
            addDataType(name, type, value);
            Logging::Message("Loading shader variable (%s, %s)", name, type);
        } else {
            Logging::Message("Keeping shader variable (%s, %s)", name, type);
        }
    }
}

void ShaderOpenGLCustom::addDataType(const char *name, const char *type, cJSON *value)
{
    ShaderOpenGLCustomDataValue LUAValue;

    switch (GLSLTypeMapping[type]) {
        case ShaderOpenGLCustomDataType::INT: {
            LUAValue = value->valueint;
            break;
        }
        case ShaderOpenGLCustomDataType::FLOAT: {
            LUAValue = static_cast<float>(value->valuedouble);
            break;
        }
        case ShaderOpenGLCustomDataType::VEC2: {
           LUAValue = glm::vec2(
                static_cast<float>(cJSON_GetObjectItemCaseSensitive(value, "x")->valuedouble),
                static_cast<float>(cJSON_GetObjectItemCaseSensitive(value, "y")->valuedouble)
           );
           break;
        }
        case ShaderOpenGLCustomDataType::VEC3: {
            LUAValue = glm::vec3(
                static_cast<float>(cJSON_GetObjectItemCaseSensitive(value, "x")->valuedouble),
                static_cast<float>(cJSON_GetObjectItemCaseSensitive(value, "y")->valuedouble),
                static_cast<float>(cJSON_GetObjectItemCaseSensitive(value, "z")->valuedouble)
            );
            break;
        }
        case ShaderOpenGLCustomDataType::VEC4: {
            LUAValue = glm::vec4(
                static_cast<float>(cJSON_GetObjectItemCaseSensitive(value, "x")->valuedouble),
                static_cast<float>(cJSON_GetObjectItemCaseSensitive(value, "y")->valuedouble),
                static_cast<float>(cJSON_GetObjectItemCaseSensitive(value, "z")->valuedouble),
                static_cast<float>(cJSON_GetObjectItemCaseSensitive(value, "w")->valuedouble)
            );
            break;
        }
        case ShaderOpenGLCustomDataType::TEXTURE2D: {
            LUAValue = nullptr;
            if (value != nullptr) {
                LUAValue = new Image(cJSON_GetObjectItemCaseSensitive(value, "path")->valuestring);
            }
            break;
        }
        case ShaderOpenGLCustomDataType::DIFFUSE: {
            LUAValue = nullptr;
            break;
        }
        case ShaderOpenGLCustomDataType::SPECULAR: {
            LUAValue = nullptr;
            break;
        }
        case ShaderOpenGLCustomDataType::DELTA_TIME: {
            LUAValue = 0.0f;
            break;
        }
        case ShaderOpenGLCustomDataType::EXECUTION_TIME: {
            LUAValue = 0.0f;
            break;
        }
        case ShaderOpenGLCustomDataType::SCENE: {
            LUAValue = nullptr;
            break;
        }
        case ShaderOpenGLCustomDataType::DEPTH: {
            LUAValue = nullptr;
            break;
        }
        default:
            break;
    }

    dataTypes.emplace_back(name, type, LUAValue);
    dataTypesDefaultValues.emplace_back(name, type, LUAValue);
}

void ShaderOpenGLCustom::addDataTypeEmpty(const char *name, const char *type)
{
    ShaderOpenGLCustomDataValue typeValue;

    switch (GLSLTypeMapping[type]) {
        case ShaderOpenGLCustomDataType::INT: {
            typeValue = 0;
            break;
        }
        case ShaderOpenGLCustomDataType::FLOAT: {
            typeValue = 0.0f;
            break;
        }
        case ShaderOpenGLCustomDataType::VEC2: {
            typeValue = glm::vec2(0);
            break;
        }
        case ShaderOpenGLCustomDataType::VEC3: {
            typeValue = glm::vec3(0);
            break;
        }
        case ShaderOpenGLCustomDataType::VEC4: {
            typeValue = glm::vec4(0);
            break;
        }
        case ShaderOpenGLCustomDataType::TEXTURE2D: {
            typeValue = nullptr;
            break;
        }
        case ShaderOpenGLCustomDataType::DIFFUSE: {
            typeValue = nullptr;
            break;
        }
        case ShaderOpenGLCustomDataType::SPECULAR: {
            typeValue = nullptr;
            break;
        }
        case ShaderOpenGLCustomDataType::DELTA_TIME: {
            typeValue = 0.f;
            break;
        }
        case ShaderOpenGLCustomDataType::EXECUTION_TIME: {
            typeValue = 0.f;
            break;
        }
        case ShaderOpenGLCustomDataType::SCENE: {
            typeValue = nullptr;
            break;
        }
        case ShaderOpenGLCustomDataType::DEPTH: {
            typeValue = nullptr;
            break;
        }
    }

    dataTypes.emplace_back(name, type, typeValue);
    dataTypesDefaultValues.emplace_back(name, type, typeValue);
}

GLuint ShaderOpenGLCustom::compile()
{
    Logging::Message("Compiling custom shader (%s, %s)", vertexFilename.c_str(), fragmentFilename.c_str());

    programID = LoadShaders(vertexFilename.c_str(), fragmentFilename.c_str(), type == ShaderCustomTypes::SHADER_OBJECT);

    return programID;
}

void ShaderOpenGLCustom::destroy()
{
    glDeleteFramebuffers(1, &resultFramebuffer);
    glDeleteTextures(1, &textureResult);
    createFramebuffer();
}

void ShaderOpenGLCustom::drawImGuiProperties(Image *diffuse, Image *specular) {
    ImGui::SeparatorText("OpenGL custom uniforms");

    static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    int i = 0;
    for (auto &type: dataTypes) {
        switch (GLSLTypeMapping[type.type]) {
            case ShaderOpenGLCustomDataType::INT: {
                ImGui::PushID(i);
                int valueInt = std::get<int>(type.value);
                if (ImGui::InputInt(type.name.c_str(), &valueInt)) {
                    type.value = valueInt;
                }
                i++;
                ImGui::PopID();
                break;
            }
            case ShaderOpenGLCustomDataType::FLOAT: {
                ImGui::PushID(i);
                float valueFloat = std::get<float>(type.value);
                if (ImGui::InputFloat(type.name.c_str(), &valueFloat, 0.01f, 1.0f, "%.3f")) {
                    type.value = valueFloat;
                }
                i++;
                ImGui::PopID();
                break;
            }
            case ShaderOpenGLCustomDataType::VEC2: {
                ImGui::PushID(i);
                auto valueFloat = std::get<glm::vec2>(type.value);
                if (ImGui::DragFloat2(type.name.c_str(), &valueFloat[0], 0.01f, 0.0f, 1.0f)) {
                    type.value = valueFloat;
                }
                i++;
                ImGui::PopID();
                break;
            }
            case ShaderOpenGLCustomDataType::VEC3: {
                ImGui::PushID(i);
                auto valueFloat = std::get<glm::vec3>(type.value);
                if (ImGui::DragFloat3(type.name.c_str(), &valueFloat[0], 0.01f, 0.0f, 1.0f)) {
                    type.value = valueFloat;
                }
                i++;
                ImGui::PopID();
                break;
            }
            case ShaderOpenGLCustomDataType::VEC4: {
                ImGui::PushID(i);
                auto valueFloat = std::get<glm::vec4>(type.value);
                if (ImGui::DragFloat4(type.name.c_str(), &valueFloat[0], 0.01f, 0.0f, 1.0f)) {
                    type.value = valueFloat;
                }
                i++;
                ImGui::PopID();
                break;
            }
            default:
                break;
        }
    }

    if (i <= 0) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Custom uniforms not found");
    }

    ImGui::SeparatorText("OpenGL system uniforms");

    i = 0;
    for (auto &type: dataTypes) {
        switch (GLSLTypeMapping[type.type]) {
            case ShaderOpenGLCustomDataType::DELTA_TIME: {
                ImGui::PushID(i);
                ImGui::Text(type.name.c_str());
                i++;
                ImGui::PopID();
                break;
            }
            case ShaderOpenGLCustomDataType::EXECUTION_TIME: {
                ImGui::PushID(i);
                ImGui::Text(type.name.c_str());
                i++;
                ImGui::PopID();
                break;
            }
            default:
                break;
        }
    }

    if (i <= 0) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "System uniforms not found");
    }

    ImGui::SeparatorText("OpenGL textures");

    auto ImGuiTextures = Brakeza3D::get()->getManagerGui()->getImGuiTextures();

    if (ImGui::BeginTable("ShaderOpenGLCustomTexture", 4, flags)) {
        int j = 0;
        for (auto &type: dataTypes) {
            switch (GLSLTypeMapping[type.type]) {
                case ShaderOpenGLCustomDataType::DEPTH: {
                    ImGui::TableNextRow();
                    ImGui::PushID(j);

                    ImGui::TableSetColumnIndex(0);
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 3.0f, ImGui::GetCursorPosY() + 2.0f));

                    auto globalTexture = ComponentsManager::get()->getComponentWindow()->getDepthTexture();
                    ImGui::Image((ImTextureID) globalTexture, ImVec2(36, 36));
                    ImGui::SetItemTooltip("Depth Texture");

                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                    ImGui::Text("%s", type.name.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                    ImGui::Text("GL_TEXTURE%d", i);

                    ImGui::TableSetColumnIndex(3);
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                    ImGui::Text("%dx%d", EngineSetup::get()->screenWidth, EngineSetup::get()->screenHeight);
                    j++;
                    ImGui::PopID();
                    break;
                }
                case ShaderOpenGLCustomDataType::SCENE: {
                    ImGui::TableNextRow();
                    ImGui::PushID(j);

                    ImGui::TableSetColumnIndex(0);
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 3.0f, ImGui::GetCursorPosY() + 2.0f));

                    auto globalTexture = ComponentsManager::get()->getComponentWindow()->getGlobalTexture();
                    ImGui::Image((ImTextureID) globalTexture, ImVec2(36, 36));
                    ImGui::SetItemTooltip("Render scene");

                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                    ImGui::Text("%s", type.name.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                    ImGui::Text("GL_TEXTURE%d", i);

                    ImGui::TableSetColumnIndex(3);
                    ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                    ImGui::Text("%dx%d", EngineSetup::get()->screenWidth, EngineSetup::get()->screenHeight);
                    j++;
                    ImGui::PopID();
                    break;
                }
                case ShaderOpenGLCustomDataType::DIFFUSE: {
                    ImGui::TableNextRow();
                    ImGui::PushID(j);

                    if (diffuse != nullptr) {
                        ImGui::TableSetColumnIndex(0);
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 3.0f, ImGui::GetCursorPosY() + 2.0f));

                        ImGui::Image((ImTextureID) diffuse->getOGLTextureID(), ImVec2(36, 36));
                        ImGui::SetItemTooltip(diffuse->getFileName().c_str());

                        ImGui::TableSetColumnIndex(1);
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                        ImGui::Text("%s", type.name.c_str());

                        ImGui::TableSetColumnIndex(2);
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                        ImGui::Text("GL_TEXTURE%d", i);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                        ImGui::Text("%dx%d", diffuse->width(), diffuse->height());
                    }
                    j++;
                    ImGui::PopID();
                    break;
                }
                case ShaderOpenGLCustomDataType::TEXTURE2D: {
                    ImGui::TableNextRow();
                    ImGui::PushID(i);

                    auto texture = std::get<Image *>(type.value);
                    if (texture != nullptr) {
                        ImGui::TableSetColumnIndex(0);
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 3.0f, ImGui::GetCursorPosY() + 2.0f));

                        ImGui::Image((ImTextureID) texture->getOGLTextureID(), ImVec2(36, 36));
                        ImGui::SetItemTooltip(texture->getFileName().c_str());
                        captureDragDropUpdateImage(type, texture);

                        ImGui::TableSetColumnIndex(1);
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                        ImGui::Text("%s", type.name.c_str());

                        ImGui::TableSetColumnIndex(2);
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                        ImGui::Text("GL_TEXTURE%d", i);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                        ImGui::Text("%dx%d", texture->width(), texture->height());
                    } else {
                        ImGui::TableSetColumnIndex(0);
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 3.0f, ImGui::GetCursorPosY() + 2.0f));
                        ImGui::Image(TexturePackage::getOGLTextureID(*ImGuiTextures, "textureIcon"), ImVec2(36, 36));
                        captureDragDropUpdateImage(type, texture);

                        ImGui::TableSetColumnIndex(1);
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                        ImGui::Text("Empty texture");

                        ImGui::TableSetColumnIndex(2);
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                        ImGui::Text("GL_TEXTURE%d", i);

                        ImGui::TableSetColumnIndex(3);
                        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + 13.0f));
                        ImGui::Text("-");
                    }
                    j++;
                    ImGui::PopID();
                    break;
                }
                default:
                    break;
            }
        }
        ImGui::EndTable();

        if (j <= 0) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Textures not found");
        }
    }
}

void ShaderOpenGLCustom::captureDragDropUpdateImage(ShaderOpenGLCustomType &type, const Image *texture) const
{
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("IMAGE_ITEM")) {
            Logging::Message("Dropping image (%s) in emitter %s", payload->Data, getLabel().c_str());
            IM_ASSERT(payload->DataSize == sizeof(int));
            auto selection = (char*) payload->Data;
            auto fullPath = EngineSetup::get()->IMAGES_FOLDER + selection;
            if (texture == nullptr) {
                type.value = new Image(fullPath);
            } else {
                delete texture;
                type.value = new Image(fullPath);
            }
            Logging::Message("File %s", selection);
        }
        ImGui::EndDragDropTarget();
    }
}

bool ShaderOpenGLCustom::isEnabled() const
{
    return enabled;
}

void ShaderOpenGLCustom::setEnabled(bool value)
{
    enabled = value;
}

void ShaderOpenGLCustom::onUpdate() const
{
    if (!isEnabled()) return;
}

void ShaderOpenGLCustom::postUpdate()
{
    if (!isEnabled()) return;

    render(resultFramebuffer);

    auto window = ComponentsManager::get()->getComponentWindow();
    auto shaderOGLImage = ComponentsManager::get()->getComponentRender()->getShaderOGLImage();
    shaderOGLImage->renderTexture(textureResult, 0, 0, window->getWidth(), window->getHeight(), 1, true, window->getGlobalFramebuffer());
}

const std::string &ShaderOpenGLCustom::getLabel() const
{
    return label;
}

cJSON *ShaderOpenGLCustom::getTypesJSON()
{
    cJSON *scriptJSON = cJSON_CreateObject();
    cJSON_AddStringToObject(scriptJSON, "name", getLabel().c_str());
    cJSON_AddStringToObject(scriptJSON, "vertexshader", vertexFilename.c_str());
    cJSON_AddStringToObject(scriptJSON, "fragmentshader", fragmentFilename.c_str());
    cJSON_AddStringToObject(scriptJSON, "type", getShaderTypeString(type).c_str());

    cJSON *typesArray = cJSON_CreateArray();
    for (auto dataType : dataTypes) {
        cJSON *typeJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(typeJSON, "name", dataType.name.c_str());
        cJSON_AddStringToObject(typeJSON, "type", dataType.type.c_str());

        std::string name = dataType.name + "("+ dataType.type +")";
        switch (GLSLTypeMapping[dataType.type]) {
            case ShaderOpenGLCustomDataType::INT: {
                auto valueInt = std::get<int>(dataType.value);
                cJSON_AddNumberToObject(typeJSON, "value", valueInt);
                break;
            }
            case ShaderOpenGLCustomDataType::FLOAT: {
                auto valueFloat = std::get<float>(dataType.value);
                cJSON_AddNumberToObject(typeJSON, "value", valueFloat);
                break;
            }
            case ShaderOpenGLCustomDataType::VEC2: {
                auto value = std::get<glm::vec2>(dataType.value);
                cJSON *vertexJSON = cJSON_CreateObject();
                cJSON_AddNumberToObject(vertexJSON, "x", value.x);
                cJSON_AddNumberToObject(vertexJSON, "y", value.y);
                cJSON_AddItemToObject(typeJSON, "value", vertexJSON);
                break;
            }
            case ShaderOpenGLCustomDataType::VEC3: {
                auto value = std::get<glm::vec3>(dataType.value);
                cJSON *vertexJSON = cJSON_CreateObject();
                cJSON_AddNumberToObject(vertexJSON, "x", value.x);
                cJSON_AddNumberToObject(vertexJSON, "y", value.y);
                cJSON_AddNumberToObject(vertexJSON, "z", value.z);
                cJSON_AddItemToObject(typeJSON, "value", vertexJSON);
                break;
            }
            case ShaderOpenGLCustomDataType::VEC4: {
                auto value = std::get<glm::vec4>(dataType.value);
                cJSON *vertexJSON = cJSON_CreateObject();
                cJSON_AddNumberToObject(vertexJSON, "x", value.x);
                cJSON_AddNumberToObject(vertexJSON, "y", value.y);
                cJSON_AddNumberToObject(vertexJSON, "z", value.z);
                cJSON_AddNumberToObject(vertexJSON, "w", value.w);
                cJSON_AddItemToObject(typeJSON, "value", vertexJSON);
                break;
            }
            case ShaderOpenGLCustomDataType::TEXTURE2D: {
                auto value = std::get<Image *>(dataType.value);
                if (value != nullptr) {
                    cJSON *textureJSON = cJSON_CreateObject();
                    cJSON_AddStringToObject(textureJSON, "path", value->getFileName().c_str());
                    cJSON_AddItemToObject(typeJSON, "value", textureJSON);
                }
                break;
            }
            default:
                std::cerr << "Unknown data typeJSON." << std::endl;
        }
        cJSON_AddItemToArray(typesArray, typeJSON);
    }

    cJSON_AddItemToObject(scriptJSON, "types", typesArray);

    return scriptJSON;
}

void ShaderOpenGLCustom::setDataTypesUniforms()
{
    for (auto type: dataTypes) {
        switch (GLSLTypeMapping[type.type]) {
            case ShaderOpenGLCustomDataType::INT: {
                const int value = std::get<int>(type.value);
                setInt(type.name, value);
                break;
            }
            case ShaderOpenGLCustomDataType::FLOAT: {
                const float value = std::get<float>(type.value);
                setFloat(type.name, value);
                break;
            }
            case ShaderOpenGLCustomDataType::VEC2: {
                auto value = std::get<glm::vec2>(type.value);
                setVec2(type.name, value);
                break;
            }
            case ShaderOpenGLCustomDataType::VEC3: {
                auto value = std::get<glm::vec3>(type.value);
                setVec3(type.name, value);
                break;
            }
            case ShaderOpenGLCustomDataType::VEC4: {
                auto value = std::get<glm::vec4>(type.value);
                setVec4(type.name, value);
                break;
            }
            case ShaderOpenGLCustomDataType::TEXTURE2D: {
                auto image = std::get<Image *>(type.value);
                if (image != nullptr) {
                    setTexture(type.name, image->getOGLTextureID(), numTextures);
                    increaseNumberTextures();
                }
                break;
            }
            case ShaderOpenGLCustomDataType::DELTA_TIME: {
                setFloat(type.name, Brakeza3D::get()->getDeltaTime());
                break;
            }
            case ShaderOpenGLCustomDataType::EXECUTION_TIME: {
                setFloat(type.name, Brakeza3D::get()->getExecutionTime());
                break;
            }
            case ShaderOpenGLCustomDataType::SCENE: {
                auto globalTexture = ComponentsManager::get()->getComponentWindow()->getSceneTexture();
                setTexture(type.name, globalTexture, numTextures);
                increaseNumberTextures();
                break;
            }
            case ShaderOpenGLCustomDataType::DEPTH: {
                auto globalTexture = ComponentsManager::get()->getComponentWindow()->getDepthTexture();
                setTexture(type.name, globalTexture, numTextures);
                increaseNumberTextures();
                break;
            }
            default:
                break;
        }
    }
}

void ShaderOpenGLCustom::updateFileTypes()
{
    Logging::Message("Updating types file (%s)", this->fileTypes.c_str());
    char *output_string = cJSON_Print(getTypesJSON());

    Tools::writeToFile(this->fileTypes, output_string);

    delete output_string;
}

void ShaderOpenGLCustom::removeDataType(const ShaderOpenGLCustomType& data)
{
    for (auto it = dataTypes.begin(); it != dataTypes.end(); ++it) {
        if (it->name == data.name) {
            dataTypes.erase(it);
            return;
        }
    }
}

void ShaderOpenGLCustom::createEmptyCustomShader(
    const std::string& name,
    const std::string& folder,
    ShaderCustomTypes type
)
{
    Logging::Message("Creating new custom shader: %s de tipo %d", name.c_str(), type);

    std::string shaderFragmentFile = folder + std::string(name + ".fs");
    std::string shaderVertexFile = folder + std::string(name + ".vs");

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", name.c_str());
    cJSON_AddStringToObject(root, "file", shaderFragmentFile.c_str());
    cJSON_AddStringToObject(root, "type", ShaderOpenGLCustom::getShaderTypeString(type).c_str());

    cJSON *typesArray = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "types", typesArray);
    char *typesCode = cJSON_Print(root);


    // json
    Tools::writeToFile(folder + dataTypesFileFor(name), typesCode);

    switch(type) {
        case ShaderCustomTypes::SHADER_POSTPROCESSING : {
            // vs y fs
            Tools::copyFile(EngineSetup::get()->TEMPLATE_SHADER_POSTPROCESSING_VS, shaderVertexFile);
            Tools::copyFile(EngineSetup::get()->TEMPLATE_SHADER_POSTPROCESSING_FS, shaderFragmentFile);
            break;
        }
        case ShaderCustomTypes::SHADER_OBJECT : {
            // vs y fs
            Tools::copyFile(EngineSetup::get()->TEMPLATE_SHADER_OBJECT_VS, shaderVertexFile);
            Tools::copyFile(EngineSetup::get()->TEMPLATE_SHADER_OBJECT_FS, shaderFragmentFile);
            break;
        }
    }
}

void ShaderOpenGLCustom::setDataTypeValue(const std::string& name, const ShaderOpenGLCustomDataValue &newValue)
{
    for (auto& dataType : dataTypes) {
        if (dataType.name == name) {
            dataType.value = newValue;
            return;
        }
    }

    std::cerr << "Error: '" << name << "' no encontrado en dataTypes\n";
}

void ShaderOpenGLCustom::setDataTypeValue(const std::string& name, int newValue) {
    setDataTypeValue(name, ShaderOpenGLCustomDataValue(newValue));
}

void ShaderOpenGLCustom::setDataTypeValue(const std::string& name, float newValue) {
    setDataTypeValue(name, ShaderOpenGLCustomDataValue(newValue));
}

void ShaderOpenGLCustom::setDataTypeValue(const std::string& name, glm::vec2 newValue) {
    setDataTypeValue(name, ShaderOpenGLCustomDataValue(newValue));
}

void ShaderOpenGLCustom::setDataTypeValue(const std::string& name, glm::vec3 newValue) {
    setDataTypeValue(name, ShaderOpenGLCustomDataValue(newValue));
}

void ShaderOpenGLCustom::setDataTypeValue(const std::string& name, glm::vec4 newValue) {
    setDataTypeValue(name, ShaderOpenGLCustomDataValue(newValue));
}

void ShaderOpenGLCustom::resetNumberTextures()
{
    numTextures = 0;
}

void ShaderOpenGLCustom::increaseNumberTextures()
{
    numTextures++;
}

ShaderCustomTypes ShaderOpenGLCustom::getType() const {
    return type;
}

ShaderCustomTypes ShaderOpenGLCustom::getShaderTypeFromString(const std::string& shaderName)
{
    auto render = ComponentsManager::get()->getComponentRender();
    auto types = render->getShaderTypesMapping();

    auto it = types.find(shaderName);
    if (it != types.end()) {
        return it->second;
    }

    return SHADER_NONE;
}

std::string ShaderOpenGLCustom::getShaderTypeString(ShaderCustomTypes type)
{
    auto render = ComponentsManager::get()->getComponentRender();
    auto types = render->getShaderTypesMapping();

    for (const auto& a: types) {
        if (a.second == type) {
            return a.first;
        }
    }

    return "";
}

void ShaderOpenGLCustom::removeCustomShaderFiles(const std::string& folder, const std::string &name)
{
    Logging::Message("Deleting custom shader: %s", name.c_str());

    Tools::removeFile(folder + name + ".json");
    Tools::removeFile(folder + name + ".vs");
    Tools::removeFile(folder + name + ".fs");
}

ShaderCustomTypes ShaderOpenGLCustom::extractTypeFromShaderName(const std::string& folder, const std::string &name)
{
    std::string jsonFile = name + ".json";

    size_t file_size;
    auto contentFile = Tools::readFile(folder + jsonFile, file_size);
    Logging::Message("Extracting type from: '%s'", name.c_str());

    auto oJSON = cJSON_Parse(contentFile);
    std::string nameType = cJSON_GetObjectItemCaseSensitive(oJSON, "type")->valuestring;

    return ShaderOpenGLCustom::getShaderTypeFromString(nameType);
}

std::string ShaderOpenGLCustom::getFolder()
{
    return Tools::removeSubstring(vertexFilename, label + ".vs");
}

const std::vector<ShaderOpenGLCustomType> &ShaderOpenGLCustom::getDataTypes() const {
    return dataTypes;
}

void ShaderOpenGLCustom::reload()
{
    size_t file_size_fs;
    sourceFS = Tools::readFile(fragmentFilename, file_size_fs);

    size_t file_size_vs;
    sourceVS = Tools::readFile(vertexFilename, file_size_vs);

    dataTypes.clear();
    dataTypesDefaultValues.clear();

    parseTypesFromFileAttributes();

    compile();
}

void ShaderOpenGLCustom::createFramebuffer()
{
    glGenFramebuffers(1, &resultFramebuffer);
    ComponentsManager::get()->getComponentRender()->changeOpenGLFramebuffer(resultFramebuffer);

    auto window = ComponentsManager::get()->getComponentWindow();
    int w = window->getWidth();
    int h = window->getHeight();

    glGenTextures(1, &textureResult);
    glBindTexture(GL_TEXTURE_2D, textureResult);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureResult, 0);
}
