#include "../../../include/OpenGL/Base/ShaderBaseCompute.h"
#include "../../../include/Misc/Logging.h"

#include <fstream>
#include <sstream>
#include <vector>

ShaderBaseCompute::ShaderBaseCompute(const FilePath::ComputeShaderFile& computeFile)
    : computeFilePath(computeFile)
{
    vertexFilename = computeFile.path;
}

void ShaderBaseCompute::PrepareBackground()
{
    std::ifstream file(computeFilePath.path);
    if (!file.is_open()) {
        LOG_ERROR("[ShaderBaseCompute] Cannot open compute shader: %s", computeFilePath.c_str());
        return;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    sourceCS = ss.str();
}

void ShaderBaseCompute::PrepareMainThread()
{
    if (sourceCS.empty()) {
        LOG_ERROR("[ShaderBaseCompute] Empty source for: %s", computeFilePath.c_str());
        return;
    }

    GLuint csID = glCreateShader(GL_COMPUTE_SHADER);
    const char* src = sourceCS.c_str();
    glShaderSource(csID, 1, &src, nullptr);
    glCompileShader(csID);

    GLint result = GL_FALSE;
    int logLen = 0;
    glGetShaderiv(csID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(csID, GL_INFO_LOG_LENGTH, &logLen);
    if (result == GL_FALSE) {
        std::vector<char> msg(logLen > 0 ? logLen + 1 : 1, '\0');
        if (logLen > 0) glGetShaderInfoLog(csID, logLen, nullptr, &msg[0]);
        LOG_ERROR("[ShaderBaseCompute] Compile FAILED: %s\n%s", computeFilePath.c_str(), &msg[0]);
        glDeleteShader(csID);
        return;
    }
    if (logLen > 0) {
        std::vector<char> msg(logLen + 1);
        glGetShaderInfoLog(csID, logLen, nullptr, &msg[0]);
        LOG_MESSAGE("[ShaderBaseCompute] Compile warning: %s", &msg[0]);
    }

    GLuint pid = glCreateProgram();
    glAttachShader(pid, csID);
    glLinkProgram(pid);

    glGetProgramiv(pid, GL_LINK_STATUS, &result);
    glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &logLen);
    if (result == GL_FALSE) {
        std::vector<char> msg(logLen > 0 ? logLen + 1 : 1, '\0');
        if (logLen > 0) glGetProgramInfoLog(pid, logLen, nullptr, &msg[0]);
        LOG_ERROR("[ShaderBaseCompute] Link FAILED: %s\n%s", computeFilePath.c_str(), &msg[0]);
        glDeleteProgram(pid);
        glDeleteShader(csID);
        return;
    }

    glDetachShader(pid, csID);
    glDeleteShader(csID);

    if (programID != 0) glDeleteProgram(programID);
    programID = pid;
    uniformLocationCache.clear();

    LOG_MESSAGE("[OpenGL] Compute program compiled OK - programID=%d (%s)", programID, computeFilePath.c_str());
    LoadUniforms();
}

void ShaderBaseCompute::dispatch(GLuint groupsX, GLuint groupsY, GLuint groupsZ) const
{
    glUseProgram(programID);
    glDispatchCompute(groupsX, groupsY, groupsZ);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ShaderBaseCompute::Destroy()
{
    if (programID != 0) {
        glDeleteProgram(programID);
        programID = 0;
    }
}
