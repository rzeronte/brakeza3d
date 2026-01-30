//
// Created by Eduardo on 04/01/2026.
//

#include "../../../include/OpenGL/Nodes/ShaderNodesPostProcessing.h"

#include "../../../include/GUI/ShaderNodeEditorManager.h"
#include "../../../include/Components/Components.h"

ShaderNodesPostProcessing::ShaderNodesPostProcessing(
    const std::string &label,
    const std::string &typesFile,
    ShaderCustomType type,
    ShaderNodeEditorManager *nodeManager
)
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
    GetNodeManager()->SetNeedsRecompile();
}

void ShaderNodesPostProcessing::Render(GLuint fbo, GLuint scenePostProcessedTexture)
{
    GetNodeManager()->Update();
    GetNodeManager()->RenderEffect(fbo);
}

void ShaderNodesPostProcessing::DrawImGuiProperties(const Image *diffuse, Image *specular)
{
    GetNodeManager()->RenderShaderDebugPanel();
}

void ShaderNodesPostProcessing::UpdateInternalTextures(GLuint colorTexture, GLuint depthTexture) const
{
    GetNodeManager()->UpdateInternalTextures(colorTexture, depthTexture);
}
