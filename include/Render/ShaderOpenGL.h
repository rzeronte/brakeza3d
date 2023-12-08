#define GL_GLEXT_PROTOTYPES

#ifndef BRAKEZA3D_SHADEROPENGL_H
#define BRAKEZA3D_SHADEROPENGL_H

#include <string>
#include <vec2.hpp>
#include <ext/matrix_float3x3.hpp>
#include <SDL2/SDL_opengl.h>

class ShaderOpenGL {
    std::string vertexFilename;
    std::string fragmentFilename;
protected:
    GLuint programID;
public:
    ShaderOpenGL(const std::string &vertexFilename, const std::string &fragmentFilename);
    static GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path);

    [[nodiscard]] GLuint getProgramID() const;

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


#endif //BRAKEZA3D_SHADEROPENGL_H
