//
// Created by Eduardo on 04/01/2026.
//

#ifndef BRAKEZA3D_SHADERNODESPOSTPROCESSING_H
#define BRAKEZA3D_SHADERNODESPOSTPROCESSING_H

#include "ShaderBaseNodes.h"

class ShaderNodesPostProcessing  : public ShaderBaseNodes
{
public:
    ShaderNodesPostProcessing(const std::string &label, const std::string &typesFile, ShaderCustomType type, NodeEditorManager *nodeManager);

    void LoadUniforms() override;
    void Destroy() override;
    void Reload() override;
    void Render(GLuint fbo, GLuint texture) override;
    void DrawImGuiProperties(const Image *diffuse, Image *specular) override;
};


#endif //BRAKEZA3D_SHADERNODESPOSTPROCESSING_H