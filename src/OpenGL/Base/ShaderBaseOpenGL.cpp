
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include "../../../include/OpenGL/Base/ShaderBaseOpenGL.h"
#include "../../../include/Misc/Logging.h"
#include "../../../include/Misc/Tools.h"
#include "../../../include/OpenGL/ShaderPreProcessor.h"

ShaderBaseOpenGL::ShaderBaseOpenGL(const std::string &vertexFilename, const std::string &fragmentFilename, bool enableFeedback)
:
    vertexFilename(vertexFilename),
    fragmentFilename(fragmentFilename),
    enableFeedback(enableFeedback)
{
}

void ShaderBaseOpenGL::PrepareBackground()
{
    ReadShaderFiles(vertexFilename, fragmentFilename);
}

void ShaderBaseOpenGL::PrepareMainThread()
{
    CompileShaderToProgramID(enableFeedback);
}

void ShaderBaseOpenGL::PrepareSync()
{
    PrepareBackground();
    PrepareMainThread();
}

ShaderBaseOpenGL::ShaderBaseOpenGL(const std::string &vertexFilename, bool enableFeedback)
:
    vertexFilename(vertexFilename),
    enableFeedback(enableFeedback)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertexFilename, std::ios::in);
    if (VertexShaderStream.is_open()){
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    LOG_MESSAGE("[OpenGL] Compiling vertex shader : %s", vertexFilename.c_str());
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , nullptr);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        LOG_MESSAGE("[OpenGL] %s", &VertexShaderErrorMessage[0]);
    }

    // Link the program
    LOG_MESSAGE("[OpenGL] Linking program...");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);

    if (enableFeedback) {
        const char* feedbackVaryings[] = {"tf_Position"};  // Usamos un arreglo de cadenas (const char*)
        glTransformFeedbackVaryings(ProgramID, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
    }

    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        LOG_MESSAGE("[OpenGL] Check the program: %s", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);

    glDeleteShader(VertexShaderID);

    programID = ProgramID;
}

void ShaderBaseOpenGL::CompileShaderToProgramID(bool enableFeedback)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    LOG_MESSAGE("[OpenGL] Compiling vertex shader: %s", vertexFilename.c_str());
    char const * VertexSourcePointer = sourceVS.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , nullptr);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        LOG_MESSAGE("[OpenGL] ERROR: %s", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    LOG_MESSAGE("[OpenGL] Compiling fragment shader: %s", fragmentFilename.c_str());
    char const * FragmentSourcePointer = sourceFS.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , nullptr);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
        LOG_MESSAGE("[OpenGL] %s", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    LOG_MESSAGE("[OpenGL] Linking program...");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);

    if (enableFeedback) {
        const char* feedbackVaryings[] = {"tf_Position"};
        glTransformFeedbackVaryings(ProgramID, 1, feedbackVaryings, GL_INTERLEAVED_ATTRIBS);
    }

    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength+1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
        LOG_MESSAGE("[OpenGL] Check the program: %s", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    this->programID = ProgramID;
}

GLuint ShaderBaseOpenGL::getProgramID() const
{
    return programID;
}

void ShaderBaseOpenGL::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), static_cast<int>(value));
}
void ShaderBaseOpenGL::setBoolUniform(GLuint uniform, bool value)
{
    glUniform1i(uniform, static_cast<int>(value));
}
// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}
void ShaderBaseOpenGL::setIntUniform(GLuint uniform, int value)
{
    glUniform1i(uniform, value);
}
// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
void ShaderBaseOpenGL::setFloatUniform(GLuint uniform, float value)
{
    glUniform1f(uniform, value);
}
// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void ShaderBaseOpenGL::setVec2Uniform(GLuint uniform, const glm::vec2 &value)
{
    glUniform2fv(uniform, 1, &value[0]);
}
void ShaderBaseOpenGL::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
}
void ShaderBaseOpenGL::setVec2Uniform(GLuint uniform, float x, float y)
{
    glUniform2f(uniform, x, y);
}

// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void ShaderBaseOpenGL::setVec3Uniform(GLuint uniform, const glm::vec3 &value)
{
    glUniform3fv(uniform, 1, &value[0]);
}
void ShaderBaseOpenGL::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}
void ShaderBaseOpenGL::setVec3Uniform(GLuint uniform, float x, float y, float z)
{
    glUniform3f(uniform, x, y, z);
}

// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void ShaderBaseOpenGL::setVec4Uniform(GLuint uniform, const glm::vec4 &value)
{
    glUniform4fv(uniform, 1, &value[0]);
}

void ShaderBaseOpenGL::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
}

void ShaderBaseOpenGL::setVec4Uniform(GLuint uniform, float x, float y, float z, float w)
{
    glUniform4f(uniform, x, y, z, w);
}
// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderBaseOpenGL::setMat2Uniform(GLuint uniform, const glm::mat2 &mat)
{
    glUniformMatrix2fv(uniform, 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void ShaderBaseOpenGL::setMat3Uniform(GLuint uniform, const glm::mat3 &mat)
{
    glUniformMatrix3fv(uniform, 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void ShaderBaseOpenGL::setMat4Array(const std::string &name, std::vector<glm::mat4> &Transforms) const
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), static_cast<GLsizei>(Transforms.size()), GL_FALSE, glm::value_ptr(Transforms[0]));
}
void ShaderBaseOpenGL::setMat4ArrayUniform(GLuint uniform, std::vector<glm::mat4> &Transforms)
{
    glUniformMatrix4fv(uniform, static_cast<GLsizei>(Transforms.size()), GL_FALSE, glm::value_ptr(Transforms[0]));
}
// ------------------------------------------------------------------------

std::string ShaderBaseOpenGL::getVertexFilename() const
{
    return vertexFilename;
}

std::string ShaderBaseOpenGL::getFragmentFilename() const
{
    return fragmentFilename;
}

void ShaderBaseOpenGL::setVAOAttributes(GLuint vertexBuffer, GLuint uvBuffer, GLuint normalBuffer)
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void ShaderBaseOpenGL::setMat4Uniform(GLuint uniform, const glm::mat4 &mat)
{
    glUniformMatrix4fv(uniform, 1, GL_FALSE, &mat[0][0]);
}

void ShaderBaseOpenGL::setTextureUniform(GLuint uniform, GLuint texture, int index)
{
    if (index == 0) {
        glActiveTexture(GL_TEXTURE0);
    } else if (index == 1) {
        glActiveTexture(GL_TEXTURE1);
    } else if (index == 2) {
        glActiveTexture(GL_TEXTURE2);
    } else if (index == 3) {
        glActiveTexture(GL_TEXTURE3);
    } else if (index == 4) {
        glActiveTexture(GL_TEXTURE4);
    } else if (index == 5) {
        glActiveTexture(GL_TEXTURE5);
    }
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(uniform, index);
}

void ShaderBaseOpenGL::setTextureArrayUniform(GLuint uniform, GLuint texture, int index)
{
    if (index == 0) {
        glActiveTexture(GL_TEXTURE0);
    } else if (index == 1) {
        glActiveTexture(GL_TEXTURE1);
    } else if (index == 2) {
        glActiveTexture(GL_TEXTURE2);
    } else if (index == 3) {
        glActiveTexture(GL_TEXTURE3);
    } else if (index == 4) {
        glActiveTexture(GL_TEXTURE4);
    } else if (index == 5) {
        glActiveTexture(GL_TEXTURE5);
    }

    glBindTexture(GL_TEXTURE_2D_ARRAY, texture);
    glUniform1i(uniform, index);
}

void ShaderBaseOpenGL::setTexture(const std::string &name, GLuint textureID, int index) const
{
    if (index == 0) {
        glActiveTexture(GL_TEXTURE0);
    } else if (index == 1) {
        glActiveTexture(GL_TEXTURE1);
    } else if (index == 2) {
        glActiveTexture(GL_TEXTURE2);
    } else if (index == 3) {
        glActiveTexture(GL_TEXTURE3);
    } else if (index == 4) {
        glActiveTexture(GL_TEXTURE4);
    } else if (index == 5) {
        glActiveTexture(GL_TEXTURE5);
    }
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(programID, name.c_str()), index);
}

void ShaderBaseOpenGL::ReadShaderFiles(const std::string &vertexFilename, const std::string &fragmentFilename)
{
    if (!Tools::FileExists(vertexFilename.c_str()) || !Tools::FileExists(fragmentFilename.c_str())) {
        LOG_ERROR("[ShaderBaseOpenGL] Cannot open shader files (%s, %s)", vertexFilename.c_str(), fragmentFilename.c_str());
        return;
    }

    const char* vsCodeRaw = Tools::ReadFile(vertexFilename);
    const char* fsCodeRaw = Tools::ReadFile(fragmentFilename);

    if (!vsCodeRaw || !fsCodeRaw) {
        LOG_ERROR("[ShaderBaseOpenGL] Failed to read shader files");
        if (vsCodeRaw) free((void*)vsCodeRaw);
        if (fsCodeRaw) free((void*)fsCodeRaw);
        return;
    }

    // Procesar includes
    std::string includeDir = Config::get()->SHADERS_FOLDER;
    sourceVS = ShaderPreprocessor::Process(std::string(vsCodeRaw), includeDir);
    sourceFS = ShaderPreprocessor::Process(std::string(fsCodeRaw), includeDir);

    // Liberar memoria
    free((void*)vsCodeRaw);
    free((void*)fsCodeRaw);
}