//
// Created by Eduardo on 12/02/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H
#define BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H


#include "ShaderBaseCustomOGLCode.h"

class ShaderOGLCustomCodePostprocessing : public ShaderBaseCustomOGLCode {
public:
    ShaderOGLCustomCodePostprocessing(const std::string &label, const std::string &typesFile, const std::string &vsFile, const std::string &fsFile);
    ShaderOGLCustomCodePostprocessing(const std::string &label, const std::string &typesFile, const std::string &vsFile, const std::string &fsFile, cJSON *types);
    void LoadUniforms() override;
    void PrepareMainThread() override;
    void Render(GLuint fbo, GLuint texture) override;
    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H
