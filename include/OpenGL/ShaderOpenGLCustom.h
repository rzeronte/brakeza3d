#define GL_GLEXT_PROTOTYPES

#ifndef BRAKEZA3D_SHADEROPENGLCUSTOM_H
#define BRAKEZA3D_SHADEROPENGLCUSTOM_H


#include <string>
#include <SDL2/SDL_opengl.h>
#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include <ext/matrix_float2x2.hpp>

class ShaderOpenGLCustom {
    std::string shaderFilename;

protected:
    GLuint programID;
public:
    explicit ShaderOpenGLCustom(const std::string &shaderFilename);

protected:
    static GLuint LoadShaders(const char *file_path);

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setVec2(const std::string &name, const glm::vec2 &value) const;

    void setVec2(const std::string &name, float x, float y) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void setVec3(const std::string &name, float x, float y, float z) const;

    void setVec4(const std::string &name, const glm::vec4 &value) const;

    void setVec4(const std::string &name, float x, float y, float z, float w) const;

    void setMat2(const std::string &name, const glm::mat2 &mat) const;

    void setMat3(const std::string &name, const glm::mat3 &mat) const;

    void setMat4(const std::string &name, const glm::mat4 &mat) const;
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOM_H
