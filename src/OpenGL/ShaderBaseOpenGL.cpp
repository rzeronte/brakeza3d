
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include "../../include/OpenGL/ShaderBaseOpenGL.h"

ShaderBaseOpenGL::ShaderBaseOpenGL(const std::string &vertexFilename, const std::string &fragmentFilename, bool enableFeedback)
:
    vertexFilename(vertexFilename),
    fragmentFilename(fragmentFilename)
{
    programID = LoadShaders(
        vertexFilename.c_str(),
        fragmentFilename.c_str(),
        enableFeedback
    );
}

ShaderBaseOpenGL::ShaderBaseOpenGL(const std::string &vertexFilename, bool enableFeedback)
:
    vertexFilename(vertexFilename)
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
    printf("[OpenGL] Compiling vertex shader : %s\n", vertexFilename.c_str());
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , nullptr);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
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
        printf("Check the program: %s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);

    glDeleteShader(VertexShaderID);

    programID = ProgramID;
}

GLuint ShaderBaseOpenGL::LoadShaders(const char * vertex_file_path, const char * fragment_file_path, bool enableFeedback)
{
    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open()){
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("[OpenGL] Compiling vertex shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , nullptr);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, nullptr, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("[OpenGL] Compiling fragment shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , nullptr);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0){
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, nullptr, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }


    // Link the program
    printf("Linking program\n");
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
        printf("Check the program: %s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

GLuint ShaderBaseOpenGL::getProgramID() const
{
    return programID;
}

void ShaderBaseOpenGL::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), static_cast<int>(value));
}
void ShaderBaseOpenGL::setBoolUniform(GLint uniform, bool value)
{
    glUniform1i(uniform, static_cast<int>(value));
}
// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}
void ShaderBaseOpenGL::setIntUniform(GLint uniform, int value)
{
    glUniform1i(uniform, value);
}
// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
void ShaderBaseOpenGL::setFloatUniform(GLint uniform, float value)
{
    glUniform1f(uniform, value);
}
// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void ShaderBaseOpenGL::setVec2Uniform(GLint uniform, const glm::vec2 &value)
{
    glUniform2fv(uniform, 1, &value[0]);
}
void ShaderBaseOpenGL::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
}
void ShaderBaseOpenGL::setVec2Uniform(GLint uniform, float x, float y)
{
    glUniform2f(uniform, x, y);
}

// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void ShaderBaseOpenGL::setVec3Uniform(GLint uniform, const glm::vec3 &value)
{
    glUniform3fv(uniform, 1, &value[0]);
}
void ShaderBaseOpenGL::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}
void ShaderBaseOpenGL::setVec3Uniform(GLint uniform, float x, float y, float z)
{
    glUniform3f(uniform, x, y, z);
}

// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void ShaderBaseOpenGL::setVec4Uniform(GLint uniform, const glm::vec4 &value)
{
    glUniform4fv(uniform, 1, &value[0]);
}

void ShaderBaseOpenGL::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
}

void ShaderBaseOpenGL::setVec4Uniform(GLint uniform, float x, float y, float z, float w)
{
    glUniform4f(uniform, x, y, z, w);
}
// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderBaseOpenGL::setMat2Uniform(GLint uniform, const glm::mat2 &mat)
{
    glUniformMatrix2fv(uniform, 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ShaderBaseOpenGL::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderBaseOpenGL::setMat3Uniform(GLint uniform, const glm::mat3 &mat)
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

void ShaderBaseOpenGL::setMat4ArrayUniform(GLint uniform, std::vector<glm::mat4> &Transforms)
{
    glUniformMatrix4fv(uniform, static_cast<GLsizei>(Transforms.size()), GL_FALSE, glm::value_ptr(Transforms[0]));
}

std::string ShaderBaseOpenGL::getVertexFilename() const {
    return vertexFilename;
}

std::string ShaderBaseOpenGL::getFragmentFilename() const {
    return fragmentFilename;
}

void ShaderBaseOpenGL::setMat4Uniform(GLint uniform, const glm::mat4 &mat)
{
    glUniformMatrix4fv(uniform, 1, GL_FALSE, &mat[0][0]);
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
