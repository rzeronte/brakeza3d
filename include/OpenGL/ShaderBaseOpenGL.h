#ifndef BRAKEZA3D_SHADEROPENGL_H
#define BRAKEZA3D_SHADEROPENGL_H

#define GL_GLEXT_PROTOTYPES

#include <string>
#include <glm/vec2.hpp>
#include <glm/ext/matrix_float3x3.hpp>
#include <GL/glew.h>
#include <vector>

class ShaderBaseOpenGL {
protected:
    GLuint programID = 0;
    std::string vertexFilename;
    std::string fragmentFilename;
    std::string commonCodeFile;
    bool enableFeedback = false;

    std::string sourceVS;
    std::string sourceFS;
    std::string commonCode;
public:
    ShaderBaseOpenGL() = default;
    ShaderBaseOpenGL(const std::string &vertexFilename, const std::string &fragmentFilename, bool enableFeedback);
    ShaderBaseOpenGL(const std::string &vertexFilename, bool enableFeedback);

    virtual void LoadUniforms() = 0;
    virtual void PrepareBackground();
    virtual void PrepareMainThread();
    void PrepareSync();

    virtual ~ShaderBaseOpenGL() = default;

    void CompileShaderToProgramID(bool enableFeedback);
    virtual void Destroy() = 0;
    void setBool(const std::string &name, bool value) const;
    static void setBoolUniform(GLuint uniform, bool value);
    void setInt(const std::string &name, int value) const;
    static void setIntUniform(GLuint uniform, int value);
    void setFloat(const std::string &name, float value) const;
    static void setFloatUniform(GLuint uniform, float value);
    void setVec2(const std::string &name, const glm::vec2 &value) const;
    static void setVec2Uniform(GLuint uniform, const glm::vec2 &value);
    void setVec2(const std::string &name, float x, float y) const;
    static void setVec2Uniform(GLuint uniform, float x, float y);
    void setVec3(const std::string &name, const glm::vec3 &value) const;
    static void setVec3Uniform(GLuint uniform, const glm::vec3 &value);
    void setVec3(const std::string &name, float x, float y, float z) const;
    static void setVec3Uniform(GLuint uniform, float x, float y, float z);
    void setVec4(const std::string &name, const glm::vec4 &value) const;
    static void setVec4Uniform(GLuint uniform, const glm::vec4 &value);
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    static void setVec4Uniform(GLuint uniform, float x, float y, float z, float w);
    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    static void setMat2Uniform(GLuint uniform, const glm::mat2 &mat);
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    static void setMat3Uniform(GLuint uniform, const glm::mat3 &mat);
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
    static void setMat4Uniform(GLuint uniform, const glm::mat4 &mat);
    static void setTextureUniform(GLuint uniform, GLuint texture, int index);

    static void setTextureArrayUniform(GLuint uniform, GLuint texture, int index);
    void setTexture(const std::string &name, GLuint textureID, int index) const;
    void setMat4Array(const std::string &name, std::vector<glm::mat4> &Transforms) const;
    static void setMat4ArrayUniform(GLuint uniform, std::vector<glm::mat4> &Transforms);

    [[nodiscard]] GLuint getProgramID() const;
    [[nodiscard]] std::string getVertexFilename() const;
    [[nodiscard]] std::string getFragmentFilename() const;

    static void setVAOAttributes(GLuint vertexBuffer, GLuint uvBuffer, GLuint normalBuffer);
    void ReadShaderFiles(const std::string &vertexFilename, const std::string &fragmentFilename);
};


#endif //BRAKEZA3D_SHADEROPENGL_H
