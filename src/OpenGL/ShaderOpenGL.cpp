
#include <fstream>
#include <sstream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include "../../include/OpenGL/ShaderOpenGL.h"
#include "../../include/Render/Logging.h"

ShaderOpenGL::ShaderOpenGL(const std::string &vertexFilename, const std::string &fragmentFilename, bool enableFeedback)
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

ShaderOpenGL::ShaderOpenGL(const std::string &vertexFilename, bool enableFeedback)
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
    printf("[OpenGL] Compiling shader : %s\n", vertexFilename.c_str());
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

GLuint ShaderOpenGL::LoadShaders(const char * vertex_file_path, const char * fragment_file_path, bool enableFeedback)
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
    printf("[OpenGL] Compiling shader : %s\n", vertex_file_path);
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
    printf("[OpenGL] Compiling shader : %s\n", fragment_file_path);
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
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

GLuint ShaderOpenGL::getProgramID() const {
    return programID;
}


void ShaderOpenGL::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
}
void ShaderOpenGL::setBoolUniform(GLint uniform, bool value) {
    glUniform1i(uniform, (int)value);
}
// ------------------------------------------------------------------------
void ShaderOpenGL::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
}
void ShaderOpenGL::setIntUniform(GLint uniform, int value)
{
    glUniform1i(uniform, value);
}
// ------------------------------------------------------------------------
void ShaderOpenGL::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
}
void ShaderOpenGL::setFloatUniform(GLint uniform, float value)
{
    glUniform1f(uniform, value);
}
// ------------------------------------------------------------------------
void ShaderOpenGL::setVec2(const std::string &name, const glm::vec2 &value) const
{
    glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void ShaderOpenGL::setVec2Uniform(GLint uniform, const glm::vec2 &value)
{
    glUniform2fv(uniform, 1, &value[0]);
}
void ShaderOpenGL::setVec2(const std::string &name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
}
void ShaderOpenGL::setVec2Uniform(GLint uniform, float x, float y)
{
    glUniform2f(uniform, x, y);
}

// ------------------------------------------------------------------------
void ShaderOpenGL::setVec3(const std::string &name, const glm::vec3 &value) const
{
    glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void ShaderOpenGL::setVec3Uniform(GLint uniform, const glm::vec3 &value)
{
    glUniform3fv(uniform, 1, &value[0]);
}
void ShaderOpenGL::setVec3(const std::string &name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
}
void ShaderOpenGL::setVec3Uniform(GLint uniform, float x, float y, float z)
{
    glUniform3f(uniform, x, y, z);
}

// ------------------------------------------------------------------------
void ShaderOpenGL::setVec4(const std::string &name, const glm::vec4 &value) const
{
    glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
}
void ShaderOpenGL::setVec4Uniform(GLint uniform, const glm::vec4 &value)
{
    glUniform4fv(uniform, 1, &value[0]);
}

void ShaderOpenGL::setVec4(const std::string &name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
}

void ShaderOpenGL::setVec4Uniform(GLint uniform, float x, float y, float z, float w)
{
    glUniform4f(uniform, x, y, z, w);
}
// ------------------------------------------------------------------------
void ShaderOpenGL::setMat2(const std::string &name, const glm::mat2 &mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderOpenGL::setMat2Uniform(GLint uniform, const glm::mat2 &mat)
{
    glUniformMatrix2fv(uniform, 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ShaderOpenGL::setMat3(const std::string &name, const glm::mat3 &mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderOpenGL::setMat3Uniform(GLint uniform, const glm::mat3 &mat)
{
    glUniformMatrix3fv(uniform, 1, GL_FALSE, &mat[0][0]);
}
// ------------------------------------------------------------------------
void ShaderOpenGL::setMat4(const std::string &name, const glm::mat4 &mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void ShaderOpenGL::setMat4Array(const std::string &name, std::vector<glm::mat4> &Transforms) const
{
    glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), (GLsizei)Transforms.size(), GL_FALSE, glm::value_ptr(Transforms[0]));
}

void ShaderOpenGL::setMat4ArrayUniform(GLint uniform, std::vector<glm::mat4> &Transforms) const
{
    glUniformMatrix4fv(uniform, (GLsizei)Transforms.size(), GL_FALSE, glm::value_ptr(Transforms[0]));
}

void ShaderOpenGL::setMat4Uniform(GLint uniform, const glm::mat4 &mat)
{
    glUniformMatrix4fv(uniform, 1, GL_FALSE, &mat[0][0]);
}

void ShaderOpenGL::setTexture(const std::string &name, GLuint textureID, int index) const
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
