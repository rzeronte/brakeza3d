//
// Created by edu on 13/12/23.
//


#include <fstream>
#include <variant>
#include <utility>
#include "../../../include/OpenGL/Code/ShaderBaseCustomOGLCode.h"
#include "../../../include/Misc/Tools.h"
#include "../../../include/Misc/Logging.h"
#include "../../../include/Components/Components.h"
#include "../../../include/Brakeza.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/OpenGL/Code/ShaderCustomOGLCodeTypes.h"
#include "../../../include/Render/Drawable.h"

ShaderBaseCustomOGLCode::ShaderBaseCustomOGLCode(
    const std::string &label,
    const std::string &typesFile,
    const std::string &vsFile,
    const std::string &fsFile,
    ShaderCustomType type,
    const cJSON *types
)
:
    ShaderBaseCustom(label, type, vsFile, fsFile, typesFile, type == SHADER_OBJECT)
{
    setDataTypesFromJSON(types);
}

ShaderBaseCustomOGLCode::ShaderBaseCustomOGLCode(
    const std::string &label,
    const std::string &typesFile,
    const std::string &vsFile,
    const std::string &fsFile,
    ShaderCustomType type
)
:
    ShaderBaseCustom(label, type, vsFile, fsFile, typesFile, type == SHADER_OBJECT)
{
    ParseTypesFromFileAttributes();
}

void ShaderBaseCustomOGLCode::PrepareBackground()
{
    ShaderBaseOpenGL::PrepareBackground();
    ParseTypesFromFileAttributes();
}

void ShaderBaseCustomOGLCode::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    CreateFramebuffer();
}

void ShaderBaseCustomOGLCode::DrawTypeImGuiControl(ShaderOGLCustomType &type, bool showName)
{
    ImGui::PushID(std::string(type.name + type.type).c_str());

    auto& info = GLSLTypeMapping[type.type];
    // Si showName es false, usar ## para ocultar el label
    auto label = showName ? Tools::ImGuiUnique(type.name) : std::string("##") + type.name;

    switch (info.type) {
        case ShaderOpenGLCustomDataType::INT: {
            int valueInt = std::get<int>(type.value);
            if (ImGui::InputInt(label.c_str(), &valueInt)) {
                type.value = valueInt;
            }
            break;
        }
        case ShaderOpenGLCustomDataType::FLOAT: {
            float valueFloat = std::get<float>(type.value);
            if (ImGui::InputFloat(label.c_str(), &valueFloat, 0.01f, 1.0f, "%.3f")) {
                type.value = valueFloat;
            }
            break;
        }
        case ShaderOpenGLCustomDataType::VEC2: {
            auto valueFloat = std::get<glm::vec2>(type.value);
            if (ImGui::DragFloat2(label.c_str(), &valueFloat[0], 0.01f, 0.0f, 1.0f)) {
                type.value = valueFloat;
            }
            break;
        }
        case ShaderOpenGLCustomDataType::VEC3: {
            auto valueFloat = std::get<glm::vec3>(type.value);
            if (ImGui::DragFloat3(label.c_str(), &valueFloat[0], 0.01f, 0.0f, 1.0f)) {
                type.value = valueFloat;
            }
            break;
        }
        case ShaderOpenGLCustomDataType::VEC4: {
            auto valueFloat = std::get<glm::vec4>(type.value);
            if (ImGui::DragFloat4(label.c_str(), &valueFloat[0], 0.01f, 0.0f, 1.0f)) {
                type.value = valueFloat;
            }
            break;
        }
        default:
            break;
    }

    ImGui::PopID();
}

int ShaderBaseCustomOGLCode::CountTypesByFilter(const std::vector<ShaderOGLCustomType>& types, const std::vector<ShaderOpenGLCustomDataType>& filterTypes)
{
    int count = 0;

    for (const auto& type : types) {
        ShaderOpenGLCustomDataType dataType = GLSLTypeMapping[type.type].type;

        // Verificar si el tipo está en el array de filtros
        for (const auto& filterType : filterTypes) {
            if (dataType == filterType) {
                count++;
                break; // No necesitamos seguir buscando para este elemento
            }
        }
    }

    return count;
}

void ShaderBaseCustomOGLCode::DrawTypeInternalImGuiControl(const ShaderOGLCustomType &type)
{
    switch (GLSLTypeMapping[type.type].type) {
        case ShaderOpenGLCustomDataType::DELTA_TIME: {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", type.name.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", type.type.c_str());

            break;
        }
        case ShaderOpenGLCustomDataType::EXECUTION_TIME: {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", type.name.c_str());

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%s", type.type.c_str());

            break;
        }
        default:
            break;
    }
}

void SetTableColumn(int index, float yOffset = 13.0f)
{
    ImGui::TableSetColumnIndex(index);
    if (index == 0) {
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX() + 3.0f, ImGui::GetCursorPosY() + 2.0f));
    } else {
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPosX(), ImGui::GetCursorPosY() + yOffset));
    }
}

void ShaderBaseCustomOGLCode::DrawTextureRow(
    const char* name,
    ImTextureID texture,
    const char* tooltip,
    int textureSlot,
    int width,
    int height
)
{
    ImGui::TableNextRow();

    SetTableColumn(0);
    ImGui::Image(texture, ImVec2(36, 36));
    ImGui::SetItemTooltip("%s", tooltip);

    SetTableColumn(1);
    ImGui::Text("%s", name);

    SetTableColumn(2);
    ImGui::Text("GL_TEXTURE%d", textureSlot);

    SetTableColumn(3);
    if (width > 0 && height > 0) {
        ImGui::Text("%dx%d", width, height);
    } else {
        ImGui::Text("-");
    }
}

void ShaderBaseCustomOGLCode::DrawImGuiProperties(const Image *diffuse, Image *specular)
{
    ImGui::Spacing();

    static ImGuiTableFlags flags = ImGuiTableFlags_SizingStretchProp |
                                ImGuiTableFlags_BordersOuter |
                                ImGuiTableFlags_BordersInnerV;

    int contUniforms = CountTypesByFilter(dataTypes, {
        ShaderOpenGLCustomDataType::INT,
        ShaderOpenGLCustomDataType::FLOAT,
        ShaderOpenGLCustomDataType::VEC2,
        ShaderOpenGLCustomDataType::VEC3,
        ShaderOpenGLCustomDataType::VEC4,
    });

    int countInternalUniforms = CountTypesByFilter(dataTypes, {
        ShaderOpenGLCustomDataType::DELTA_TIME,
        ShaderOpenGLCustomDataType::EXECUTION_TIME,
    });

    int countUniformTextures = CountTypesByFilter(dataTypes, {
        ShaderOpenGLCustomDataType::DEPTH,
        ShaderOpenGLCustomDataType::SCENE,
        ShaderOpenGLCustomDataType::TEXTURE2D,
        ShaderOpenGLCustomDataType::DIFFUSE,
    });

    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8, 8));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 4));

    // Custom uniforms
    ImGui::SeparatorText("Custom uniforms");
    if (ImGui::BeginTable("CustomUniforms", 2, flags)) {
        if (contUniforms > 0) {
            for (auto &type: dataTypes) {
                if (!IsCustomUniform(type)) continue;

                ImGui::TableNextRow(ImGuiTableRowFlags_None, ImGui::GetFrameHeight());
                ImGui::TableSetColumnIndex(0);
                ImGui::AlignTextToFramePadding();
                ImGui::Text("%s", type.name.c_str());

                ImGui::TableSetColumnIndex(1);
                DrawTypeImGuiControl(type, false);
            }
        } else {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            Drawable::WarningMessage("No custom uniforms found");
        }
        ImGui::EndTable();
    }

    ImGui::Spacing();

    // Internal/System uniforms
    ImGui::SeparatorText("Internal uniforms");
    if (ImGui::BeginTable("InternalUniforms", 2, flags)) {
        if (countInternalUniforms > 0) {
            for (auto &type: dataTypes) {
                DrawTypeInternalImGuiControl(type);
            }
        } else {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            Drawable::WarningMessage("No system uniforms found");
        }
        ImGui::EndTable();
    }

    ImGui::Spacing();

    // Textures table
    ImGui::SeparatorText("Textures uniforms");
    if (ImGui::BeginTable("ShaderOpenGLCustomTexture", 4, flags)) {
        if (countUniformTextures > 0) {
            int j = 0;

            for (auto &type: dataTypes) {
                switch (GLSLTypeMapping[type.type].type) {
                    case ShaderOpenGLCustomDataType::DEPTH: {
                        auto globalTexture = Components::get()->Window()->getGBuffer().depth;
                        DrawTextureRow(type.name.c_str(),
                                      reinterpret_cast<ImTextureID>(globalTexture),
                                      "Depth Texture", j,
                                      Config::get()->screenWidth,
                                      Config::get()->screenHeight);
                        j++;
                        break;
                    }

                    case ShaderOpenGLCustomDataType::SCENE: {
                        auto globalTexture = Components::get()->Window()->getGlobalTexture();
                        DrawTextureRow(type.name.c_str(),
                                      reinterpret_cast<ImTextureID>(globalTexture),
                                      "Render scene", j,
                                      Config::get()->screenWidth,
                                      Config::get()->screenHeight);
                        j++;
                        break;
                    }

                    case ShaderOpenGLCustomDataType::DIFFUSE: {
                        if (diffuse != nullptr) {
                            DrawTextureRow(type.name.c_str(),
                                          diffuse->getOGLImTexture(),
                                          diffuse->getFileName().c_str(), j,
                                          diffuse->width(), diffuse->height());
                        }
                        j++;
                        break;
                    }

                    case ShaderOpenGLCustomDataType::TEXTURE2D: {
                        ImGui::TableNextRow();
                        auto texture = std::get<Image *>(type.value);

                        SetTableColumn(0);
                        if (texture != nullptr) {
                            ImGui::Image(texture->getOGLImTexture(), ImVec2(36, 36));
                            ImGui::SetItemTooltip("%s", texture->getFileName().c_str());
                        } else {
                            ImGui::Image(FileSystemGUI::Icon(IconGUI::TEXTURE), ImVec2(36, 36));
                        }
                        CaptureDragDropUpdateImage(type, texture);

                        SetTableColumn(1);
                        ImGui::Text("%s", texture ? type.name.c_str() : "Empty texture");

                        SetTableColumn(2);
                        ImGui::Text("GL_TEXTURE%d", j);

                        SetTableColumn(3);
                        if (texture) {
                            ImGui::Text("%dx%d", texture->width(), texture->height());
                        } else {
                            ImGui::Text("-");
                        }

                        j++;
                        break;
                    }

                    default:
                        break;
                }
            }
        } else {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Textures not found");
        }

        ImGui::EndTable();
    }

    ImGui::PopStyleVar(2);

}

void ShaderBaseCustomOGLCode::Destroy()
{
    glDeleteFramebuffers(1, &resultFramebuffer);
    glDeleteTextures(1, &textureResult);
    CreateFramebuffer();
}

bool ShaderBaseCustomOGLCode::existDataType(const char *name, const char *type) const
{
    for (const auto& t: dataTypes) {
        if (t.name == name && t.type == type) {
            return true;
        }
    }

    return false;
}

void ShaderBaseCustomOGLCode::ParseTypesFromFileAttributes()
{
    auto contentFile = Tools::ReadFile(Config::get()->SHADERS_FOLDER + fileTypes);
    LOG_MESSAGE("[ShaderBaseCustomOGLCode] Parsing attributes from: '%s'", fileTypes.c_str());

    setDataTypesFromJSON(cJSON_GetObjectItemCaseSensitive(cJSON_Parse(contentFile), "types"));
}

std::string ShaderBaseCustomOGLCode::dataTypesFileFor(std::string basicString)
{
    return ExtractOnlyName(basicString) + ".json";
}

std::string ShaderBaseCustomOGLCode::ExtractOnlyName(std::string& filename)
{
    size_t dotPosition = filename.find_last_of('.');

    if (dotPosition != std::string::npos) {
        return filename.substr(0, dotPosition);
    }

    return filename;
}

void ShaderBaseCustomOGLCode::setDataTypesFromJSON(const cJSON *typesJSON)
{
    LOG_MESSAGE("[ShaderBaseCustomOGLCode] Settings variables into shader...");

    cJSON *currentType;
    cJSON_ArrayForEach(currentType, typesJSON) {
        auto name = cJSON_GetObjectItemCaseSensitive(currentType, "name")->valuestring;
        auto type = cJSON_GetObjectItemCaseSensitive(currentType, "type")->valuestring;
        auto value = cJSON_GetObjectItemCaseSensitive(currentType, "value");

        if (!existDataType(name, type)){
            addDataType(name, type, value);
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Loading shader variable: %s => %s", name, type);
        } else {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Keeping shader variable: %s => %s", name, type);
        }
    }
}

void ShaderBaseCustomOGLCode::addDataType(const char *name, const char *type, cJSON *value)
{
    ShaderOpenGLCustomDataValue LUAValue;

    switch (GLSLTypeMapping[type].type) {
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


void ShaderBaseCustomOGLCode::onUpdate() const
{
}

void ShaderBaseCustomOGLCode::postUpdate(GLuint outputFBO, GLuint inputTexture)
{
    if (!isEnabled()) return;

    Render(outputFBO, inputTexture);
}

void ShaderBaseCustomOGLCode::setDataTypesUniforms()
{
    for (auto type: dataTypes) {
        switch (GLSLTypeMapping[type.type].type) {
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
                    IncreaseNumberTextures();
                }
                break;
            }
            case ShaderOpenGLCustomDataType::DELTA_TIME: {
                setFloat(type.name, Brakeza::get()->getDeltaTime());
                break;
            }
            case ShaderOpenGLCustomDataType::EXECUTION_TIME: {
                setFloat(type.name, Brakeza::get()->getExecutionTime());
                break;
            }
            case ShaderOpenGLCustomDataType::SCENE: {
                setTexture(type.name, textureResult, numTextures);
                IncreaseNumberTextures();
                break;
            }
            case ShaderOpenGLCustomDataType::DEPTH: {
                auto globalTexture = Components::get()->Window()->getGBuffer().depth;
                setTexture(type.name, globalTexture, numTextures);
                IncreaseNumberTextures();
                break;
            }
            default:
                break;
        }
    }
}

void ShaderBaseCustomOGLCode::UpdateFileTypes() const
{
    LOG_MESSAGE("[ShaderBaseCustomOGLCode] Writing typesFile: %s", this->fileTypes.c_str());

    char *output_string = cJSON_Print(getTypesJSON());

    Tools::WriteToFile(this->fileTypes, output_string);

    delete output_string;
}

void ShaderBaseCustomOGLCode::AddDataTypeEmpty(const char *name, const char *type)
{
    ShaderOpenGLCustomDataValue typeValue;

    switch (GLSLTypeMapping[type].type) {
        case ShaderOpenGLCustomDataType::INT: {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Added INT type: %s => %s", name, type);
            typeValue = 0;
            break;
        }
        case ShaderOpenGLCustomDataType::FLOAT: {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Added FLOAT type: %s => %s", name, type);
            typeValue = 0.0f;
            break;
        }
        case ShaderOpenGLCustomDataType::VEC2: {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Added VEC2 type: %s => %s", name, type);
            typeValue = glm::vec2(0);
            break;
        }
        case ShaderOpenGLCustomDataType::VEC3: {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Added VEC3 type: %s => %s", name, type);
            typeValue = glm::vec3(0);
            break;
        }
        case ShaderOpenGLCustomDataType::VEC4: {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Added VEC4 type: %s => %s", name, type);
            typeValue = glm::vec4(0);
            break;
        }
        case ShaderOpenGLCustomDataType::TEXTURE2D: {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Added TEXTURE2D type: %s => %s", name, type);
            typeValue = nullptr;
            break;
        }
        case ShaderOpenGLCustomDataType::DIFFUSE: {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Added DIFFUSE type: %s => %s", name, type);
            typeValue = nullptr;
            break;
        }
        case ShaderOpenGLCustomDataType::SPECULAR: {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Added SPECULAR type: %s => %s", name, type);
            typeValue = nullptr;
            break;
        }
        case ShaderOpenGLCustomDataType::DELTA_TIME: {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Added DELTA_TIME type: %s => %s", name, type);
            typeValue = 0.f;
            break;
        }
        case ShaderOpenGLCustomDataType::EXECUTION_TIME: {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Added EXECUTION_TIME type: %s => %s", name, type);
            typeValue = 0.f;
            break;
        }
        case ShaderOpenGLCustomDataType::SCENE: {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Added SCENE type: %s => %s", name, type);
            typeValue = nullptr;
            break;
        }
        case ShaderOpenGLCustomDataType::DEPTH: {
            LOG_MESSAGE("[ShaderBaseCustomOGLCode] Added DEPTH type: %s => %s", name, type);
            typeValue = nullptr;
            break;
        }
    }

    dataTypes.emplace_back(name, type, typeValue);
    dataTypesDefaultValues.emplace_back(name, type, typeValue);
}

void ShaderBaseCustomOGLCode::removeDataType(const ShaderOGLCustomType& data)
{
    for (auto it = dataTypes.begin(); it != dataTypes.end(); ++it) {
        if (it->name == data.name) {
            dataTypes.erase(it);
            return;
        }
    }
}

void ShaderBaseCustomOGLCode::setDataTypeValue(const std::string& name, const ShaderOpenGLCustomDataValue &newValue)
{
    for (auto& dataType : dataTypes) {
        if (dataType.name == name) {
            dataType.value = newValue;
            return;
        }
    }

    std::cerr << "Error: '" << name << "' no encontrado en dataTypes\n";
}

void ShaderBaseCustomOGLCode::setDataTypeValue(const std::string& name, int newValue) {
    setDataTypeValue(name, ShaderOpenGLCustomDataValue(newValue));
}

void ShaderBaseCustomOGLCode::setDataTypeValue(const std::string& name, float newValue) {
    setDataTypeValue(name, ShaderOpenGLCustomDataValue(newValue));
}

void ShaderBaseCustomOGLCode::setDataTypeValue(const std::string& name, glm::vec2 newValue) {
    setDataTypeValue(name, ShaderOpenGLCustomDataValue(newValue));
}

void ShaderBaseCustomOGLCode::setDataTypeValue(const std::string& name, glm::vec3 newValue) {
    setDataTypeValue(name, ShaderOpenGLCustomDataValue(newValue));
}

void ShaderBaseCustomOGLCode::setDataTypeValue(const std::string& name, glm::vec4 newValue) {
    setDataTypeValue(name, ShaderOpenGLCustomDataValue(newValue));
}

void ShaderBaseCustomOGLCode::ResetNumberTextures()
{
    numTextures = 0;
}

void ShaderBaseCustomOGLCode::IncreaseNumberTextures()
{
    numTextures++;
}

std::string ShaderBaseCustomOGLCode::getFolder() const
{
    return Tools::removeSubstring(vertexFilename, label + ".vs");
}

cJSON *ShaderBaseCustomOGLCode::getTypesJSON() const
{
    cJSON *scriptJSON = cJSON_CreateObject();
    cJSON_AddStringToObject(scriptJSON, "name", getLabel().c_str());
    cJSON_AddStringToObject(scriptJSON, "type", getShaderTypeString(type).c_str());

    cJSON_AddStringToObject(scriptJSON, "vsFile", vertexFilename.c_str());
    cJSON_AddStringToObject(scriptJSON, "fsFile", fragmentFilename.c_str());
    cJSON_AddStringToObject(scriptJSON, "typesFile", fileTypes.c_str());

    cJSON *typesArray = cJSON_CreateArray();
    for (auto dataType : dataTypes) {
        cJSON *typeJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(typeJSON, "name", dataType.name.c_str());
        cJSON_AddStringToObject(typeJSON, "type", dataType.type.c_str());

        std::string name = dataType.name + "("+ dataType.type +")";
        switch (GLSLTypeMapping[dataType.type].type) {
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

void ShaderBaseCustomOGLCode::WriteEmptyCustomShaderToDisk(const std::string& name, const std::string& folder, ShaderCustomType type)
{
    LOG_MESSAGE("[ShaderBaseCustomOGLCode] Creating new custom shader '%s' of type '%d' in '%s'", name.c_str(), type, name.c_str());

    std::string typesFile = folder + std::string(name + ".json");
    std::string fsFile = folder + std::string(name + ".fs");
    std::string vsfile = folder + std::string(name + ".vs");

    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "fsFile", fsFile.c_str());
    cJSON_AddStringToObject(root, "vsFile", vsfile.c_str());
    cJSON_AddStringToObject(root, "typesFile", typesFile.c_str());
    cJSON_AddStringToObject(root, "name", name.c_str());
    cJSON_AddStringToObject(root, "type", getShaderTypeString(type).c_str());

    cJSON *typesArray = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "types", typesArray);
    char *typesCode = cJSON_Print(root);

    // json
    Tools::WriteToFile(typesFile, typesCode);

    switch(type) {
        case SHADER_POSTPROCESSING : {
            // vs y fs
            Tools::CopyFileTo(Config::get()->TEMPLATE_SHADER_POSTPROCESSING_VS, vsfile);
            Tools::CopyFileTo(Config::get()->TEMPLATE_SHADER_POSTPROCESSING_FS, fsFile);
            break;
        }
        default:
        case SHADER_OBJECT : {
            // vs y fs
            Tools::CopyFileTo(Config::get()->TEMPLATE_SHADER_OBJECT_VS, vsfile);
            Tools::CopyFileTo(Config::get()->TEMPLATE_SHADER_OBJECT_FS, fsFile);
            break;
        }
    }
}

void ShaderBaseCustomOGLCode::RemoveCustomShaderFiles(const std::string& folder, const std::string &name)
{
    LOG_MESSAGE("[ShaderBaseCustomOGLCode] Deleting custom shader: %s", name.c_str());

    Tools::RemoveFile(folder + name + ".json");
    Tools::RemoveFile(folder + name + ".vs");
    Tools::RemoveFile(folder + name + ".fs");
}

void ShaderBaseCustomOGLCode::Reload()
{
    ReadShaderFiles(vertexFilename, fragmentFilename);
    dataTypes.clear(); dataTypesDefaultValues.clear();
    ParseTypesFromFileAttributes();
    CompileShaderToProgramID(type == SHADER_OBJECT);
}

void ShaderBaseCustomOGLCode::CaptureDragDropUpdateImage(ShaderOGLCustomType &type, const Image *texture) const
{
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM)) {
            LOG_MESSAGE("Dropping image (%s) in emitter %s", payload->Data, getLabel().c_str());
            IM_ASSERT(payload->DataSize == sizeof(int));
            auto selection = (char*) payload->Data;
            auto fullPath = Config::get()->IMAGES_FOLDER + selection;
            if (texture == nullptr) {
                type.value = new Image(fullPath);
            } else {
                delete texture;
                type.value = new Image(fullPath);
            }
            LOG_MESSAGE("File %s", selection);
        }
        ImGui::EndDragDropTarget();
    }
}

void ShaderBaseCustomOGLCode::CreateFramebuffer()
{
    glGenFramebuffers(1, &resultFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, resultFramebuffer);

    auto window = Components::get()->Window();
    int w = window->getWidthRender();
    int h = window->getHeightRender();

    glGenTextures(1, &textureResult);
    glBindTexture(GL_TEXTURE_2D, textureResult);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureResult, 0);
}

void ShaderBaseCustomOGLCode::setTextureResult(GLuint value)
{
    textureResult = value;
}


bool ShaderBaseCustomOGLCode::IsCustomUniform(const ShaderOGLCustomType &type)
{
    auto dataType = GLSLTypeMapping[type.type].type;

    return dataType == ShaderOpenGLCustomDataType::INT ||
           dataType == ShaderOpenGLCustomDataType::FLOAT ||
           dataType == ShaderOpenGLCustomDataType::VEC2 ||
           dataType == ShaderOpenGLCustomDataType::VEC3 ||
           dataType == ShaderOpenGLCustomDataType::VEC4;
}