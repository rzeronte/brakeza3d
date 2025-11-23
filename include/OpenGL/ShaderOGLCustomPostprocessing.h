//
// Created by Eduardo on 12/02/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H
#define BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H


#include "ShaderOGLCustom.h"

class ShaderOGLCustomPostprocessing : public ShaderOGLCustom, ShaderBaseOpenGLQuad {
public:
    ShaderOGLCustomPostprocessing(
        const std::string &label,
        const std::string &vertexFilename,
        const std::string &fragmentFilename
    );

    ShaderOGLCustomPostprocessing(
        const std::string &label,
        const std::string &vertexFilename,
        const std::string &fragmentFilename,
        cJSON *types
    );

    GLuint compile() override;

    void render(GLuint fbo);

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H
