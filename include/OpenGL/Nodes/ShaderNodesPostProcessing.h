//
// Created by Eduardo on 04/01/2026.
//

#ifndef BRAKEZA3D_SHADERNODESPOSTPROCESSING_H
#define BRAKEZA3D_SHADERNODESPOSTPROCESSING_H

#include "ShaderBaseNodes.h"
#include "../../GUI/ShaderNodeEditorManager.h"

class ShaderNodesPostProcessing  : public ShaderBaseNodes
{
    ShaderNodeEditorManager *nodeManager;
public:
    ShaderNodesPostProcessing(const std::string &label, const std::string &typesFile, ShaderCustomType type, ShaderNodeEditorManager *nodeManager);

    void LoadUniforms() override;
    void Destroy() override;
    void Reload() override;
    void Render(GLuint fbo, GLuint scenePostProcessedTexture) override;
    void DrawImGuiProperties(const Image *diffuse, Image *specular) override;
    void UpdateInternalTextures(GLuint colorTexture, GLuint depthTexture);
};


#endif //BRAKEZA3D_SHADERNODESPOSTPROCESSING_H