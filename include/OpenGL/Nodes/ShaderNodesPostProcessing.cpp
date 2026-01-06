//
// Created by Eduardo on 04/01/2026.
//

#include "ShaderNodesPostProcessing.h"

ShaderNodesPostProcessing::ShaderNodesPostProcessing(const std::string &label, const std::string &typesFile, ShaderCustomType type, NodeEditorManager *nodeManager)
:
    ShaderBaseNodes(label, typesFile, type, nodeManager)
{
}

void ShaderNodesPostProcessing::LoadUniforms()
{
}

void ShaderNodesPostProcessing::Destroy()
{
}

void ShaderNodesPostProcessing::Reload()
{
}

void ShaderNodesPostProcessing::Render(GLuint fbo, GLuint texture)
{
}

void ShaderNodesPostProcessing::DrawImGuiProperties(const Image *diffuse, Image *specular)
{
}
