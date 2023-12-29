//
// Created by edu on 28/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLSHOCKWAVE_H
#define BRAKEZA3D_SHADEROPENGLSHOCKWAVE_H


#include "ShaderOpenGL.h"
#include "ShaderQuadOpenGL.h"
#include "../Objects/Point2D.h"

class ShaderOpenGLShockWave: public ShaderOpenGL, public ShaderQuadOpenGL {
    float timeLive;
    float speed;
    int focalPointX;
    int focalPointY;

    GLint timeUniform;
    GLint focalPointXUniform;
    GLint focalPointYUniform;

    GLint screenWidthUniform;
    GLint screenHeightUniform;

    GLint textureUniform;
public:
    ShaderOpenGLShockWave();
    void render(GLuint textureID, GLuint framebuffer);
    void destroy() override;

    void setPoint(Point2D p);

    void setTimeLive(float timeLive);
};


#endif //BRAKEZA3D_SHADEROPENGLSHOCKWAVE_H
