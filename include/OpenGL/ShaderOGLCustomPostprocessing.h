//
// Created by Eduardo on 12/02/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H
#define BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H


#include "ShaderOGLCustom.h"

class ShaderOGLCustomPostprocessing : public ShaderOGLCustom, ShaderBaseOpenGLQuad {
public:
    ShaderOGLCustomPostprocessing(const std::string &label, const std::string &vsFile, const std::string &fsFile);
    ShaderOGLCustomPostprocessing(const std::string &label, const std::string &vsFile, const std::string &fsFile, cJSON *types);
    void LoadUniforms() override;
    void PrepareMainThread() override;
    void render(GLuint fbo) override;
    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H
