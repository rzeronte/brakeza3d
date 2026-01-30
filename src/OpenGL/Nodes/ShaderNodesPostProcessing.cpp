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
    ShaderBaseNodes(label, typesFile, type, nodeManager),
    nodeManager(nodeManager)
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
    nodeManager->SetNeedsRecompile();
}

void ShaderNodesPostProcessing::Render(GLuint fbo, GLuint scenePostProcessedTexture)
{
    nodeManager->Update();
    nodeManager->RenderEffect(fbo);
}

void ShaderNodesPostProcessing::DrawImGuiProperties(const Image *diffuse, Image *specular)
{
    nodeManager->RenderShaderDebugPanel();
   //ShaderBaseNodes::DrawImGuiProperties(diffuse, specular);
}

void ShaderNodesPostProcessing::UpdateInternalTextures(GLuint colorTexture, GLuint depthTexture)
{
    nodeManager->UpdateInternalTextures(colorTexture, depthTexture);
}