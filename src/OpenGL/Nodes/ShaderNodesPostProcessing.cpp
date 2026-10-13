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
    if (GetNodeManager()) GetNodeManager()->SetNeedsRecompile();
}

void ShaderNodesPostProcessing::Render(GLuint fbo, GLuint scenePostProcessedTexture)
{
    auto manager = GetNodeManager();
    if (!manager) return;

    manager->Update();

    // Si el grafo no compila (error de tipos, ciclo...) la cadena no debe quedarse sin imagen:
    // se copia la entrada tal cual. El error se muestra en el editor de nodos.
    if (manager->GetShaderProgram() == 0) {
        auto window = Components::get()->Window();
        const int w = window->getWidthRender();
        const int h = window->getHeightRender();
        Components::get()->Render()->getShaders()->shaderOGLImage->renderTexture(scenePostProcessedTexture, 0, 0, w, h, w, h, 1, true, fbo);
        return;
    }

    manager->RenderEffect(fbo);
}

void ShaderNodesPostProcessing::DrawImGuiProperties(const Image *diffuse, Image *specular)
{
    if (!GetNodeManager()) return;
    GetNodeManager()->RenderShaderDebugPanel();
}

void ShaderNodesPostProcessing::UpdateInternalTextures(GLuint colorTexture, GLuint depthTexture) const
{
    if (!GetNodeManager()) return;
    GetNodeManager()->UpdateInternalTextures(colorTexture, depthTexture);
}
