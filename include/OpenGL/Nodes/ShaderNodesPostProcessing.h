//
// Created by Eduardo on 04/01/2026.
//

#ifndef BRAKEZA3D_SHADERNODESPOSTPROCESSING_H
#define BRAKEZA3D_SHADERNODESPOSTPROCESSING_H
#include "../Base/ShaderBaseCustom.h"


class ShaderNodesPostProcessing  : public ShaderBaseCustom
{
public:
    ShaderNodesPostProcessing(const std::string &label)
    :
        ShaderBaseCustom(label, SHADER_NODE_POSTPROCESSING)
    {
    }

    void LoadUniforms() override;
    void Destroy() override;
    void Reload() override;
    void Render(GLuint fbo, GLuint texture) override;
    void DrawImGuiProperties(const Image *diffuse, Image *specular) override;
};


#endif //BRAKEZA3D_SHADERNODESPOSTPROCESSING_H