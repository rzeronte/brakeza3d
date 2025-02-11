//
// Created by edu on 13/12/23.
//

#include <fstream>
#include <utility>
#include "../../include/OpenGL/ShaderOpenGLCustom.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Logging.h"
#include "../../include/ComponentsManager.h"


ShaderOpenGLCustom::ShaderOpenGLCustom(std::string label, const std::string &fragmentFilename)
:
    label(std::move(label)),
    enabled(true),
    ShaderOpenGL(EngineSetup::get()->SHADERS_FOLDER + "Custom.vs", fragmentFilename),
    fileTypes(ShaderOpenGLCustom::dataTypesFileFor(fragmentFilename))
{
    size_t file_size;
    sourceFS = Tools::readFile(fragmentFilename, file_size);
    strcpy(editableSource, sourceFS.c_str());

    parseTypesFromFileAttributes();

    setupQuadUniforms(programID);

    textureUniform = glGetUniformLocation(programID, "sceneTexture");
}

bool ShaderOpenGLCustom::existDataType(const char *name, const char *type)
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
            Logging::Message("Loading script variable (%s, %s)", name, type);
        } else {
            Logging::Message("Keeping script variable (%s, %s)", name, type);
        }
    }
}

void ShaderOpenGLCustom::addDataType(const char *name, const char *type, cJSON *value)
{
    ShaderOpenGLCustomDataValue LUAValue;

    switch (LUADataTypesMapping[type]) {
        case ShaderOpenGLCustomDataType::INT: {
            LUAValue = value->valueint;
            break;
        }
        case ShaderOpenGLCustomDataType::FLOAT: {
            LUAValue = (float) value->valuedouble;
            break;
        }
        case ShaderOpenGLCustomDataType::VEC2: {
           LUAValue = glm::vec2(
                (float) cJSON_GetObjectItemCaseSensitive(value, "x")->valuedouble,
                (float) cJSON_GetObjectItemCaseSensitive(value, "y")->valuedouble
           );
        }
        case ShaderOpenGLCustomDataType::VEC3: {
            LUAValue = glm::vec3(
                (float) cJSON_GetObjectItemCaseSensitive(value, "x")->valuedouble,
                (float) cJSON_GetObjectItemCaseSensitive(value, "y")->valuedouble,
                (float) cJSON_GetObjectItemCaseSensitive(value, "z")->valuedouble
            );
        }
        case ShaderOpenGLCustomDataType::VEC4: {
            LUAValue = glm::vec4(
                (float) cJSON_GetObjectItemCaseSensitive(value, "x")->valuedouble,
                (float) cJSON_GetObjectItemCaseSensitive(value, "y")->valuedouble,
                (float) cJSON_GetObjectItemCaseSensitive(value, "z")->valuedouble,
                (float) cJSON_GetObjectItemCaseSensitive(value, "w")->valuedouble
            );
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

    switch (LUADataTypesMapping[type]) {
        case ShaderOpenGLCustomDataType::INT: {
            typeValue = 0;
            break;
        }
        case ShaderOpenGLCustomDataType::FLOAT: {
            typeValue = 0;
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
    }

    dataTypes.emplace_back(name, type, typeValue);
    dataTypesDefaultValues.emplace_back(name, type, typeValue);
}

GLuint ShaderOpenGLCustom::compile()
{
    Logging::Message("Compiling custom shader (%s, %s)", vertexFilename.c_str(), fragmentFilename.c_str());

    programID = LoadShaders(vertexFilename.c_str(), fragmentFilename.c_str());

    setupQuadUniforms(programID);

    return programID;
}

void ShaderOpenGLCustom::render(GLuint textureID, GLuint framebuffer)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glUseProgram(programID);

    loadQuadMatrixUniforms();
    setDataTypesUniforms();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureUniform, 0);

    drawQuad();
}

void ShaderOpenGLCustom::destroy()
{
    resetQuadMatrix();
}

void ShaderOpenGLCustom::drawImGuiProperties()
{
    ImGui::Separator();
    static char name[256];

    strncpy(name, currentVariableToAddName.c_str(), sizeof(name));

    if (ImGui::InputText("Variable name##", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AutoSelectAll)) {
        currentVariableToAddName = name;
    }

    const char* items[] = { "int", "float", "vec2", "vec3", "vec4" };
    static int selectedItem = 0;
    ImGui::Combo("Type", &selectedItem, items, IM_ARRAYSIZE(items));

    if (ImGui::Button(std::string("Create").c_str())) {
        LUADataValue LUAValue;
        addDataTypeEmpty(currentVariableToAddName.c_str(), items[selectedItem]);
    }

    ImGui::Separator();

    for (auto&  type: dataTypes) {
        switch (LUADataTypesMapping[type.type]) {
            case ShaderOpenGLCustomDataType::INT: {
                const float rangeMin = -500000;
                const float rangeMax = 500000;

                int valueInt = std::get<int>(type.value);
                if (ImGui::DragScalar(type.name.c_str(), ImGuiDataType_S32, &valueInt, 1.0 , &rangeMin, &rangeMax, "%d", 1.0f)) {
                    type.value = valueInt;
                }
                ImGui::SameLine();
                if (ImGui::Button(std::string("Remove").c_str())) {
                    removeDataType(type);
                    updateFileTypes();
                }
                break;
            }
            case ShaderOpenGLCustomDataType::FLOAT: {
                const float rangeMin = -500000;
                const float rangeMax = 500000;
                const float rangeSensibility = 0.1;

                float valueFloat = std::get<float>(type.value);
                if (ImGui::DragScalar(type.name.c_str(), ImGuiDataType_Float, &valueFloat, rangeSensibility , &rangeMin, &rangeMax, "%f", 1.0f)) {
                    type.value = valueFloat;
                }
                ImGui::SameLine();
                if (ImGui::Button(std::string("Remove").c_str())) {
                    removeDataType(type);
                    updateFileTypes();
                }

                break;
            }
            case ShaderOpenGLCustomDataType::VEC2: {
                if (ImGui::TreeNode(type.name.c_str())) {
                    const float rangeMin = -500000;
                    const float rangeMax = 500000;
                    const float rangeSensibility = 0.1;

                    auto valueFloat = std::get<glm::vec2>(type.value);
                    if (ImGui::DragScalar("x", ImGuiDataType_Float, &valueFloat.x, rangeSensibility , &rangeMin, &rangeMax, "%f", 1.0f)) {
                        type.value = valueFloat;
                    }
                    if (ImGui::DragScalar("y", ImGuiDataType_Float, &valueFloat.y, rangeSensibility , &rangeMin, &rangeMax, "%f", 1.0f)) {
                        type.value = valueFloat;
                    }
                    if (ImGui::Button(std::string("Remove").c_str())) {
                        removeDataType(type);
                        updateFileTypes();
                    }
                    ImGui::TreePop();
                }
            break;
            }
            case ShaderOpenGLCustomDataType::VEC3: {
                if (ImGui::TreeNode(type.name.c_str())) {
                    const float rangeMin = -500000;
                    const float rangeMax = 500000;
                    const float rangeSensibility = 0.1;

                    auto valueFloat = std::get<glm::vec3>(type.value);
                    if (ImGui::DragScalar("x", ImGuiDataType_Float, &valueFloat.x, rangeSensibility , &rangeMin, &rangeMax, "%f", 1.0f)) {
                        type.value = valueFloat;
                    }
                    if (ImGui::DragScalar("y", ImGuiDataType_Float, &valueFloat.y, rangeSensibility , &rangeMin, &rangeMax, "%f", 1.0f)) {
                        type.value = valueFloat;
                    }
                    if (ImGui::DragScalar("z", ImGuiDataType_Float, &valueFloat.z, rangeSensibility , &rangeMin, &rangeMax, "%f", 1.0f)) {
                        type.value = valueFloat;
                    }
                    if (ImGui::Button(std::string("Remove").c_str())) {
                        removeDataType(type);
                        updateFileTypes();
                    }

                    ImGui::TreePop();
                }
            break;
            }
            case ShaderOpenGLCustomDataType::VEC4: {
                if (ImGui::TreeNode(type.name.c_str())) {
                    const float rangeMin = -500000;
                    const float rangeMax = 500000;
                    const float rangeSensibility = 0.1;

                    auto valueFloat = std::get<glm::vec3>(type.value);
                    if (ImGui::DragScalar("x", ImGuiDataType_Float, &valueFloat.x, rangeSensibility , &rangeMin, &rangeMax, "%f", 1.0f)) {
                        type.value = valueFloat;
                    }
                    if (ImGui::DragScalar("y", ImGuiDataType_Float, &valueFloat.y, rangeSensibility , &rangeMin, &rangeMax, "%f", 1.0f)) {
                        type.value = valueFloat;
                    }
                    if (ImGui::DragScalar("z", ImGuiDataType_Float, &valueFloat.z, rangeSensibility , &rangeMin, &rangeMax, "%f", 1.0f)) {
                        type.value = valueFloat;
                    }
                    if (ImGui::DragScalar("w", ImGuiDataType_Float, &valueFloat.z, rangeSensibility , &rangeMin, &rangeMax, "%f", 1.0f)) {
                        type.value = valueFloat;
                    }
                    if (ImGui::Button(std::string("Remove").c_str())) {
                        removeDataType(type);
                        updateFileTypes();
                    }

                    ImGui::TreePop();
                }
                break;
            }
            default:
                break;
        }
    }

    if ((int) dataTypes.size() > 0) {
        ImGui::Text("%s", "No types defined");
    }

    if (ImGui::Button(std::string("Save shader types").c_str())) {
        updateFileTypes();
    }

    ImGui::Separator();

    static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
    ImGui::InputTextMultiline("##source", editableSource, IM_ARRAYSIZE(editableSource), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 10), flags);
    if (ImGui::Button(std::string("Save").c_str())) {
        sourceFS = editableSource;
        Tools::writeToFile(fragmentFilename, sourceFS.c_str());
    }

    ImGui::TreePop();
}

bool ShaderOpenGLCustom::isEnabled() const {
    return enabled;
}

void ShaderOpenGLCustom::setEnabled(bool enabled) {
    ShaderOpenGLCustom::enabled = enabled;
}

void ShaderOpenGLCustom::onUpdate()
{
    if (!isEnabled()) return;
}

void ShaderOpenGLCustom::postUpdate()
{
    if (!isEnabled()) return;

    auto window = ComponentsManager::get()->getComponentWindow();
    render(window->globalTexture,window->getGlobalFramebuffer());
}

const std::string &ShaderOpenGLCustom::getLabel() const {
    return label;
}

cJSON *ShaderOpenGLCustom::getTypesJSON()
{
    cJSON *scriptJSON = cJSON_CreateObject();
    cJSON_AddStringToObject(scriptJSON, "name", getLabel().c_str());
    cJSON_AddStringToObject(scriptJSON, "file", fragmentFilename.c_str());

    cJSON *typesArray = cJSON_CreateArray();
    for (auto dataType : dataTypes) {
        cJSON *typeJSON = cJSON_CreateObject();
        cJSON_AddStringToObject(typeJSON, "name", dataType.name.c_str());
        cJSON_AddStringToObject(typeJSON, "type", dataType.type.c_str());

        std::string name = dataType.name + "("+ dataType.type +")";
        switch (LUADataTypesMapping[dataType.type]) {
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
        switch (LUADataTypesMapping[type.type]) {
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

ShaderOpenGLCustom *ShaderOpenGLCustom::createEmptyCustomShader(const std::string& name)
{
    Logging::Message("Creating new custom shader: %s", name.c_str());

    char *shaderCode = "#version 330 core\n"
                       "in vec2 TexCoords;\n"
                       "uniform sampler2D sceneTexture;\n"
                       "out vec4 FragColor;\n"
                       "void main()\n"
                       "{\n"
                       "    FragColor = texture(sceneTexture, TexCoords);\n"
                       "}";

    cJSON *root = cJSON_CreateObject();

    std::string shaderFile = EngineSetup::get()->CUSTOM_SHADERS_FOLDER + std::string(name + ".fs");
    cJSON_AddStringToObject(root, "name", name.c_str());
    cJSON_AddStringToObject(root, "file", shaderFile.c_str());

    cJSON *typesArray = cJSON_CreateArray();
    cJSON_AddItemToObject(root, "types", typesArray);
    char *typesCode = cJSON_Print(root);

    Tools::writeToFile(shaderFile, shaderCode);
    Tools::writeToFile(EngineSetup::get()->CUSTOM_SHADERS_FOLDER + ShaderOpenGLCustom::dataTypesFileFor(name), typesCode);

    return new ShaderOpenGLCustom(name, shaderFile);
}

