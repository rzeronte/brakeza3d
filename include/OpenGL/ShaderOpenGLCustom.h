#define GL_GLEXT_PROTOTYPES

#ifndef BRAKEZA3D_SHADEROPENGLCUSTOM_H
#define BRAKEZA3D_SHADEROPENGLCUSTOM_H


#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_float2x2.hpp>
#include <utility>
#include <variant>
#include <map>
#include "ShaderOpenGL.h"
#include "ShaderQuadOpenGL.h"
#include "../Misc/cJSON.h"
#include "../Misc/Image.h"
#include <sstream>
#include <vector>
#include <optional>

enum ShaderCustomTypes {
    SHADER_NONE = -1,
    SHADER_POSTPROCESSING = 0,
    SHADER_OBJECT = 1,
};

enum class ShaderOpenGLCustomDataType {
    INT,
    FLOAT,
    VEC2,
    VEC3,
    VEC4,
    TEXTURE2D,
    DIFFUSE,
    SPECULAR,
    DELTA_TIME,
    EXECUTION_TIME,
    SCENE
};

typedef std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, Image*> ShaderOpenGLCustomDataValue;

struct ShaderOpenGLCustomType {
    ShaderOpenGLCustomType(const char *name, const char *type, ShaderOpenGLCustomDataValue value)
    :name(name), type(type), value(value)
    {}
    std::string name;
    std::string type;
    ShaderOpenGLCustomDataValue value;
};

class ShaderOpenGLCustom: public ShaderOpenGL
{

    std::string label;

    bool enabled;
    ShaderCustomTypes type;
    std::string fileTypes;

    std::vector<ShaderOpenGLCustomType> dataTypesDefaultValues;

    // imgui
    std::string currentVariableToAddName;
    char editableSource[1024 * 16];

protected:
    std::vector<ShaderOpenGLCustomType> dataTypes;
    std::map<std::string, ShaderOpenGLCustomDataType> GLSLTypeMapping = {
        {"int", ShaderOpenGLCustomDataType::INT},
        {"float", ShaderOpenGLCustomDataType::FLOAT},
        {"vec2", ShaderOpenGLCustomDataType::VEC2},
        {"vec3", ShaderOpenGLCustomDataType::VEC3},
        {"vec4", ShaderOpenGLCustomDataType::VEC4},
        {"texture", ShaderOpenGLCustomDataType::TEXTURE2D},
        {"diffuse", ShaderOpenGLCustomDataType::DIFFUSE},
        {"specular", ShaderOpenGLCustomDataType::SPECULAR},
        {"delta_time", ShaderOpenGLCustomDataType::DELTA_TIME},
        {"execution_time", ShaderOpenGLCustomDataType::EXECUTION_TIME},
        {"scene", ShaderOpenGLCustomDataType::SCENE}
    };
    int numTextures = 0;

    ShaderOpenGLCustom(std::string label, const std::string &vertexFilename, const std::string &fragmentFilename,
                       ShaderCustomTypes type, cJSON *types);

public:
    explicit ShaderOpenGLCustom(
        std::string label,
        const std::string &vertexFilename,
        const std::string &fragmentFilename,
        ShaderCustomTypes type
    );

    virtual void render(GLuint framebuffer) = 0;

    void drawImGuiProperties(Image *diffuse, Image *specular);

    virtual GLuint compile();

    std::string sourceVS;
    std::string sourceFS;
protected:

    bool existDataType(const char *name, const char *type);

    void parseTypesFromFileAttributes();

    static std::string dataTypesFileFor(std::string basicString);

    static std::string removeFilenameExtension(std::string &filename);

    void setDataTypesFromJSON(cJSON *typesJSON);

    void addDataType(const char *name, const char *type, cJSON *value);

private:
    void destroy() override;

public:
    [[nodiscard]] bool isEnabled() const;

    void setEnabled(bool enabled);

    void onUpdate();
    void postUpdate();

    [[nodiscard]] const std::string &getLabel() const;
    cJSON* getTypesJSON();

    void setDataTypesUniforms();

    void updateFileTypes();

    void addDataTypeEmpty(const char *name, const char *type);

    void removeDataType(const ShaderOpenGLCustomType &data);

    void setDataTypeValue(const std::string &name, ShaderOpenGLCustomDataValue newValue);

    void setDataTypeValue(const std::string &name, int newValue);

    void setDataTypeValue(const std::string &name, float newValue);

    void setDataTypeValue(const std::string &name, glm::vec2 newValue);

    void setDataTypeValue(const std::string &name, glm::vec3 newValue);

    void setDataTypeValue(const std::string &name, glm::vec4 newValue);

    void resetNumberTextures();

    void increaseNumberTextures();

    [[nodiscard]] ShaderCustomTypes getType() const;

    void setType(ShaderCustomTypes type);

    static ShaderCustomTypes getShaderTypeFromString(const std::string &shaderName);

    static std::string getShaderTypeString(ShaderCustomTypes type);

    static void createEmptyCustomShader(
        const std::string& name,
        const std::string& folder,
        ShaderCustomTypes type
    );

    static void removeCustomShaderFiles(const std::string& folder, std::string name);

    static ShaderCustomTypes extractTypeFromShaderName(const std::string& folder, std::string name);

    std::string getFolder();

    [[nodiscard]] const std::vector<ShaderOpenGLCustomType> &getDataTypes() const;

    void reload();

    void captureDragDropUpdateImage(ShaderOpenGLCustomType &type, const Image *texture) const;
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOM_H
