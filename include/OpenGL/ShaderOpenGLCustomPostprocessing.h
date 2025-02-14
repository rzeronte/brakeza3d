//
// Created by Eduardo on 12/02/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H
#define BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H


#include "ShaderOpenGLCustom.h"

class ShaderOpenGLCustomPostprocessing : public ShaderOpenGLCustom, ShaderQuadOpenGL {
    GLint textureUniform;

public:
    ShaderOpenGLCustomPostprocessing(
        const std::string &label,
        const std::string &vertexFilename,
        const std::string &fragmentFilename
    );

    GLuint compile() override;

    void render(GLuint framebuffer);

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLCUSTOMPOSTPROCESSING_H
