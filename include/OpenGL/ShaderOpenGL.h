#define GL_GLEXT_PROTOTYPES

#ifndef BRAKEZA3D_SHADEROPENGL_H
#define BRAKEZA3D_SHADEROPENGL_H

#include <string>
#include <vec2.hpp>
#include <ext/matrix_float3x3.hpp>
#include <SDL2/SDL_opengl.h>

class ShaderOpenGL {
protected:
    GLuint programID;
    std::string vertexFilename;
    std::string fragmentFilename;
public:
    ShaderOpenGL(const std::string &vertexFilename, const std::string &fragmentFilename);
    static GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

    virtual void destroy() = 0;

    [[nodiscard]] GLuint getProgramID() const;

    void setBool(const std::string &name, bool value) const;
    static void setBoolUniform(GLint uniform, bool value);

    void setInt(const std::string &name, int value) const;
    static void setIntUniform(GLint uniform, int value);

    void setFloat(const std::string &name, float value) const;
    static void setFloatUniform(GLint uniform, float value);

    void setVec2(const std::string &name, const glm::vec2 &value) const;
    static void setVec2Uniform(GLint uniform, const glm::vec2 &value);

    void setVec2(const std::string &name, float x, float y) const;
    static void setVec2Uniform(GLint uniform, float x, float y);

    void setVec3(const std::string &name, const glm::vec3 &value) const;
    static void setVec3Uniform(GLint uniform, const glm::vec3 &value);

    void setVec3(const std::string &name, float x, float y, float z) const;
    static void setVec3Uniform(GLint uniform, float x, float y, float z);

    void setVec4(const std::string &name, const glm::vec4 &value) const;
    static void setVec4Uniform(GLint uniform, const glm::vec4 &value);

    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    static void setVec4Uniform(GLint uniform, float x, float y, float z, float w);

    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    static void setMat2Uniform(GLint uniform, const glm::mat2 &mat);

    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    static void setMat3Uniform(GLint uniform, const glm::mat3 &mat);

    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    static void setMat4Uniform(GLint uniform, const glm::mat4 &mat);
};


#endif //BRAKEZA3D_SHADEROPENGL_H
