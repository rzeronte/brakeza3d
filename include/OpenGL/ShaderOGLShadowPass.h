//
// Created by Eduardo on 13/11/2025.
//

#ifndef BRAKEZA3D_SHADERSHADOWPASS_H
#define BRAKEZA3D_SHADERSHADOWPASS_H

#include "ShaderOpenGL.h"
#include "../include/Objects/Mesh3D.h"
#include "../include/Objects/LightPoint3D.h"

class ShaderOGLShadowPass : public ShaderOpenGL
{
    GLuint VertexArrayID;

    GLuint shadowFBO;

    GLint matrixViewUniform;
    GLint matrixModelUniform;

public:
    ShaderOGLShadowPass();

    void renderMesh(Mesh3D *o, LightPoint3D* light, GLuint shadowMapArrayTex, int lightIndex, GLuint framebuffer);

    void render(
        Object3D* o,
        LightPoint3D* light,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int size,
        GLuint shadowMapArrayTex,
        int layer,
        GLuint framebuffer
    ) const;

    GLuint getShadowFBO() const;

    static void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer);

    void destroy() override;

    void setupFBO();
};


#endif //BRAKEZA3D_SHADERSHADOWPASS_H