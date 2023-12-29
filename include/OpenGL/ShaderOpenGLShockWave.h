//
// Created by edu on 28/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLSHOCKWAVE_H
#define BRAKEZA3D_SHADEROPENGLSHOCKWAVE_H


#include "ShaderOpenGL.h"
#include "ShaderQuadOpenGL.h"
#include "../Objects/Point2D.h"

struct ShockWaveParams {
    float param1;
    float param2;
    float param3;
    [[nodiscard]] static ShockWaveParams standard() { return {10.0f, 0.8f, 0.1f}; }
};

class ShaderOpenGLShockWave: public ShaderOpenGL, public ShaderQuadOpenGL {
    GLint timeUniform;
    GLint focalPointXUniform;
    GLint focalPointYUniform;
    GLint speedUniform;
    GLint screenWidthUniform;
    GLint screenHeightUniform;

    GLint textureUniform;
public:
    ShaderOpenGLShockWave();
    void render(Point2D position, float timeLive, float speed, ShockWaveParams params, GLuint textureID, GLuint framebuffer);
    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLSHOCKWAVE_H
