//
// Created by edu on 22/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLDOF_H
#define BRAKEZA3D_SHADEROPENGLDOF_H


#include "ShaderOpenGL.h"

class ShaderOpenGLDOF : ShaderOpenGL {
    GLuint VertexArrayID;

    GLuint framebuffer;
    GLuint sceneTexture, depthTexture;
    GLuint quadVAO, quadVBO;
public:
    ShaderOpenGLDOF(const std::string &vertexFilename, const std::string &fragmentFilename);

    void render();
};


#endif //BRAKEZA3D_SHADEROPENGLDOF_H
