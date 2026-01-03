//
// Created by edu on 8/12/23.
//

#ifndef BRAKEZA3D_COMMONOPENGLSHADERTYPES_H
#define BRAKEZA3D_COMMONOPENGLSHADERTYPES_H

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <variant>
#include "../../Render/Image.h"

// uniforms mapping structs
struct MaterialOpenGL {
    GLuint diffuse;
    GLuint specular;
    float shininess;
};

struct DirLightOpenGL {
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLightOpenGL {
    glm::vec4 position;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;
    float padding;
};

struct SpotLightOpenGL {
    glm::vec4 position;
    glm::vec4 direction;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
    float padding;
    float padding1;
    float padding2;
};

struct OpenGLPickingBuffer {
    GLuint FBO = 0;
    GLuint rbgTexture = 0;
    GLuint depthTexture = 0;
};

struct OpenGLGBuffer {
    GLuint FBO = 0;
    GLuint positions = 0;
    GLuint normals = 0;
    GLuint albedo = 0;
    GLuint depth = 0;
};

struct OpenGLGlobalFramebuffers
{
    GLuint globalFBO = 0;
    GLuint sceneFBO = 0;
    GLuint backgroundFBO = 0;
    GLuint foregroundFBO = 0;
    GLuint uiFBO = 0;

    GLuint globalTexture = 0;
    GLuint sceneTexture = 0;
    GLuint backgroundTexture = 0;
    GLuint foregroundTexture = 0;
    GLuint uiTexture = 0;
};

// SHADER OGL CUSTOM CODE
enum ShaderCustomType {
    SHADER_NONE = -1,
    SHADER_POSTPROCESSING = 0,
    SHADER_OBJECT = 1,
    SHADER_NODE_OBJECT = 2,
    SHADER_NODE_POSTPROCESSING = 3,
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

struct ShaderOGLMetaInfo {
    std::string name;
    std::string type;
    std::string vsFile;
    std::string fsFile;
    std::string typesFile;
};


#endif //BRAKEZA3D_COMMONOPENGLSHADERTYPES_H
