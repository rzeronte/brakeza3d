//
// Created by Eduardo on 04/01/2026.
//

#include "../../OpenGL/Nodes/ShaderBaseNodes.h"

#include "../../Misc/Logging.h"

void ShaderBaseNodes::LoadUniforms()
{
}

void ShaderBaseNodes::Destroy()
{
}

void ShaderBaseNodes::Reload()
{
}

void ShaderBaseNodes::Render(GLuint fbo, GLuint texture)
{
}

void ShaderBaseNodes::DrawImGuiProperties(const Image *diffuse, Image *specular)
{
}

void ShaderBaseNodes::WriteEmptyCustomShaderToDisk(const std::string &name, const std::string &folder, ShaderCustomType type)
{
    LOG_MESSAGE("[ShaderBaseNodes] Creating new custom shader '%s' of type '%d' in '%s'", name.c_str(), type, name.c_str());
}
