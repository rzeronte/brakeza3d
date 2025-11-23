//
// Created by edu on 10/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLLINE_H
#define BRAKEZA3D_SHADEROPENGLLINE_H


#include "ShaderBaseOpenGL.h"
#include "../Objects/Point2D.h"
#include "../Misc/Color.h"
#include "ShaderBaseOpenGLQuad.h"

class ShaderOpenGLLine : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad  {
    GLuint lineStartUniform;
    GLuint lineEndUniform;
    GLuint lineColorUniform;
    GLuint weightUniform;
public:
    ShaderOpenGLLine();

    void render(Point2D a, Point2D b, Color c, float weight, GLuint fbo);

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLLINE_H
