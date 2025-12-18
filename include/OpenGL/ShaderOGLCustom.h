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

struct ShaderOGLCustomType
{
    ShaderOGLCustomType(const char *name, const char *type, ShaderOpenGLCustomDataValue value)
    :
        name(name),
        type(type),
        value(value)
    {

    }
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

struct ShaderOGLMetaInfo {
    std::string name;
    std::string type;
    std::string vsFile;
    std::string fsFile;
    std::string typesFile;
};

class ShaderOGLCustom: public ShaderBaseOpenGL
{
    GLuint resultFramebuffer = 0;
    GLuint textureResult = 0;

    std::string label;

    bool enabled = true;

    ShaderCustomType type;
    std::string fileTypes;

    std::vector<ShaderOGLCustomType> dataTypesDefaultValues;

protected:

    int numTextures = 0;

    ShaderOGLCustom(
        std::string label,
        const std::string &typesFile,
        const std::string &vsFile,
        const std::string &fsFile,
        ShaderCustomType type,
        const cJSON *types
    );

public:
    explicit ShaderOGLCustom(
        std::string label,
        const std::string &typesFile,
        const std::string &vsFile,
        const std::string &fsFile,
        ShaderCustomType type
    );
    std::vector<ShaderOGLCustomType> dataTypes;

    void PrepareBackground() override;
    void PrepareMainThread() override;

    static void DrawTypeImGuiControl(ShaderOGLCustomType &type);
    static int CountTypesByFilter(const std::vector<ShaderOGLCustomType> &types,const std::vector<ShaderOpenGLCustomDataType> &filterTypes);
    static void DrawTypeInternalImGuiControl(const ShaderOGLCustomType &type);

    virtual void render(GLuint fbo, GLuint texture) = 0;

    void drawImGuiProperties(const Image *diffuse, Image *specular);

    std::string sourceVS;
    std::string sourceFS;

    void Destroy() override;

protected:

    bool existDataType(const char *name, const char *type) const;
    void ParseTypesFromFileAttributes();
    static std::string dataTypesFileFor(std::string basicString);
    static std::string ExtractOnlyName(std::string &filename);
    void setDataTypesFromJSON(const cJSON *typesJSON);
    void addDataType(const char *name, const char *type, cJSON *value);

public:
    void setEnabled(bool value);
    void onUpdate() const;
    void postUpdate(GLuint outputFBO, GLuint inputTexture);
    void setLabel(const std::string &value);
    void setDataTypesUniforms();
    void UpdateFileTypes() const;
    void AddDataTypeEmpty(const char *name, const char *type);
    void removeDataType(const ShaderOGLCustomType &data);
    void setDataTypeValue(const std::string &name, const ShaderOpenGLCustomDataValue &newValue);
    void setDataTypeValue(const std::string &name, int newValue);
    void setDataTypeValue(const std::string &name, float newValue);
    void setDataTypeValue(const std::string &name, glm::vec2 newValue);
    void setDataTypeValue(const std::string &name, glm::vec3 newValue);
    void setDataTypeValue(const std::string &name, glm::vec4 newValue);
    void ResetNumberTextures();
    void IncreaseNumberTextures();
    std::string getFolder() const;
    cJSON* getTypesJSON() const;

    [[nodiscard]] std::string getTypesFile() const                      { return fileTypes;}
    [[nodiscard]] bool isEnabled() const                                { return enabled; }
    [[nodiscard]] std::vector<ShaderOGLCustomType> &getDataTypes()      { return dataTypes; }
    [[nodiscard]] const std::string &getLabel() const                   { return label; }
    [[nodiscard]] ShaderCustomType getType() const                      { return type; }

    static ShaderCustomType getShaderTypeFromString(const std::string &shaderName);
    static std::string getShaderTypeString(ShaderCustomType type);
    static void createEmptyCustomShader(const std::string& name, const std::string& folder, ShaderCustomType type);
    static void RemoveCustomShaderFiles(const std::string& folder, const std::string &name);
    static ShaderCustomType ExtractTypeFromShaderName(const std::string& folder, const std::string &name);
    void Reload();
    void CaptureDragDropUpdateImage(ShaderOGLCustomType &type, const Image *texture) const;
    void CreateFramebuffer();
    void setTextureResult(GLuint value);
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOM_H
