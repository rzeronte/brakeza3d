#ifndef BRAKEZA3D_SHADEROPENGLCUSTOM_H
#define BRAKEZA3D_SHADEROPENGLCUSTOM_H

#define GL_GLEXT_PROTOTYPES

#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/matrix_float2x2.hpp>
#include <variant>
#include <map>
#include "ShaderBaseOpenGL.h"
#include "ShaderBaseOpenGLQuad.h"
#include "../Misc/cJSON.h"
#include "../Render/Image.h"
#include <sstream>
#include <vector>

enum ShaderCustomType {
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
    SCENE,
    DEPTH
};

struct ShaderTypeInfo {
    ShaderOpenGLCustomDataType type;
    std::string label;
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

static std::map<std::string, ShaderTypeInfo> GLSLTypeMapping = {
    {"int", {ShaderOpenGLCustomDataType::INT, "Integer" }},
    {"float", {ShaderOpenGLCustomDataType::FLOAT, "Float" }},
    {"vec2", {ShaderOpenGLCustomDataType::VEC2, "Vec2" }},
    {"vec3", {ShaderOpenGLCustomDataType::VEC3, "Vec3" }},
    {"vec4", {ShaderOpenGLCustomDataType::VEC4, "Vec4" }},
    {"texture", {ShaderOpenGLCustomDataType::TEXTURE2D, "Texture"}},
    {"diffuse", {ShaderOpenGLCustomDataType::DIFFUSE, "Diffuse Mesh3D texture (internal)" }},
    {"specular", {ShaderOpenGLCustomDataType::SPECULAR, "Specular Mesh3D texture (internal)" }},
    {"delta_time", {ShaderOpenGLCustomDataType::DELTA_TIME, "Delta time (internal)" }},
    {"execution_time", {ShaderOpenGLCustomDataType::EXECUTION_TIME, "Execution time (internal)" }},
    {"scene", {ShaderOpenGLCustomDataType::SCENE, "Scene texture (internal)" }},
    {"depth", {ShaderOpenGLCustomDataType::DEPTH, "Depth texture (internal)" }}
};

class ShaderOGLCustom: public ShaderBaseOpenGL
{
    GLuint resultFramebuffer = 0;
    GLuint textureResult = 0;

    std::string label;

    bool enabled = true;

    ShaderCustomType type;
    std::string fileTypes;

    std::vector<ShaderOpenGLCustomType> dataTypesDefaultValues;

protected:
    std::vector<ShaderOpenGLCustomType> dataTypes;

    int numTextures = 0;

    ShaderOGLCustom(
        std::string label,
        const std::string &vertexFilename,
        const std::string &fragmentFilename,
        ShaderCustomType type,
        cJSON *types
    );

public:
    explicit ShaderOGLCustom(
        std::string label,
        const std::string &vertexFilename,
        const std::string &fragmentFilename,
        ShaderCustomType type
    );

    virtual void render(GLuint fbo) = 0;

    void drawImGuiProperties(Image *diffuse, Image *specular);

    virtual GLuint compile();

    std::string sourceVS;
    std::string sourceFS;

    void destroy() override;

protected:

    bool existDataType(const char *name, const char *type) const;

    void parseTypesFromFileAttributes();

    static std::string dataTypesFileFor(std::string basicString);

    static std::string removeFilenameExtension(std::string &filename);

    void setDataTypesFromJSON(cJSON *typesJSON);

    void addDataType(const char *name, const char *type, cJSON *value);

public:
    void setEnabled(bool value);
    void onUpdate() const;
    void postUpdate();
    void setLabel(std::string value);
    void setDataTypesUniforms();
    void UpdateFileTypes();
    void AddDataTypeEmpty(const char *name, const char *type);
    void removeDataType(const ShaderOpenGLCustomType &data);
    void setDataTypeValue(const std::string &name, const ShaderOpenGLCustomDataValue &newValue);
    void setDataTypeValue(const std::string &name, int newValue);
    void setDataTypeValue(const std::string &name, float newValue);
    void setDataTypeValue(const std::string &name, glm::vec2 newValue);
    void setDataTypeValue(const std::string &name, glm::vec3 newValue);
    void setDataTypeValue(const std::string &name, glm::vec4 newValue);
    void ResetNumberTextures();
    void IncreaseNumberTextures();
    std::string getFolder();
    cJSON* getTypesJSON();
    [[nodiscard]] bool isEnabled() const;
    [[nodiscard]] const std::vector<ShaderOpenGLCustomType> &getDataTypes() const;
    [[nodiscard]] const std::string &getLabel() const;
    [[nodiscard]] ShaderCustomType getType() const;
    static ShaderCustomType getShaderTypeFromString(const std::string &shaderName);
    static std::string getShaderTypeString(ShaderCustomType type);
    static void createEmptyCustomShader(const std::string& name, const std::string& folder, ShaderCustomType type);
    static void RemoveCustomShaderFiles(const std::string& folder, const std::string &name);
    static ShaderCustomType extractTypeFromShaderName(const std::string& folder, const std::string &name);
    void Reload();
    void CaptureDragDropUpdateImage(ShaderOpenGLCustomType &type, const Image *texture) const;
    void ReadShaderFiles(const std::string &vertexFilename, const std::string &fragmentFilename);
    void CreateFramebuffer();
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOM_H
