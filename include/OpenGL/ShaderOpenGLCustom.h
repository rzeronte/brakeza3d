#define GL_GLEXT_PROTOTYPES

#ifndef BRAKEZA3D_SHADEROPENGLCUSTOM_H
#define BRAKEZA3D_SHADEROPENGLCUSTOM_H


#include <string>
#include <SDL2/SDL_opengl.h>
#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include <ext/matrix_float2x2.hpp>
#include <variant>
#include <map>
#include "ShaderOpenGL.h"
#include "ShaderQuadOpenGL.h"
#include "../Misc/cJSON.h"
#include <sstream>
#include <vector>

enum class ShaderOpenGLCustomDataType {
    INT,
    FLOAT,
    VEC2,
    VEC3,
    VEC4
};

typedef std::variant<int, float, glm::vec2, glm::vec3, glm::vec4> ShaderOpenGLCustomDataValue;

struct ShaderOpenGLCustomType {
    ShaderOpenGLCustomType(const char *name, const char *type, ShaderOpenGLCustomDataValue value)
    :name(name), type(type), value(value)
    {}
    std::string name;
    std::string type;
    ShaderOpenGLCustomDataValue value;
};

class ShaderOpenGLCustom: ShaderOpenGL, ShaderQuadOpenGL
{
    std::map<std::string, ShaderOpenGLCustomDataType> LUADataTypesMapping = {
        {"int", ShaderOpenGLCustomDataType::INT},
        {"float", ShaderOpenGLCustomDataType::FLOAT},
        {"vec2", ShaderOpenGLCustomDataType::VEC2},
        {"vec3", ShaderOpenGLCustomDataType::VEC3},
        {"vec4", ShaderOpenGLCustomDataType::VEC4},
    };

    std::string label;
    std::string sourceFS;

    bool enabled;
    std::string fileTypes;

    std::vector<ShaderOpenGLCustomType> dataTypes;
    std::vector<ShaderOpenGLCustomType> dataTypesDefaultValues;

    GLint textureUniform;

    // imgui
    std::string currentVariableToAddName;
    char editableSource[1024 * 16];
protected:
public:
    explicit ShaderOpenGLCustom(std::string label, const std::string &fragmentFilename);

    void render(GLuint textureID, GLuint framebuffer);

    void drawImGuiProperties();

    GLuint compile();

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

    static ShaderOpenGLCustom *createEmptyCustomShader(const std::string& name);
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOM_H
