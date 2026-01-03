//
// Created by edu on 10/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLLINE_H
#define BRAKEZA3D_SHADEROPENGLLINE_H


#include "../Base/ShaderBaseOpenGL.h"
#include "../../Render/Point2D.h"
#include "../../Render/Color.h"
#include "../Base/ShaderBaseOpenGLQuad.h"

class ShaderOGLLine : public ShaderBaseOpenGL, public ShaderBaseOpenGLQuad
{
    GLuint lineStartUniform = 0;
    GLuint lineEndUniform = 0;
    GLuint lineColorUniform = 0;
    GLuint weightUniform = 0;
public:
    ShaderOGLLine();
    void PrepareMainThread() override;
    void LoadUniforms() override;

    void render(Point2D a, Point2D b, Color c, float weight, GLuint fbo);
    void Destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLLINE_H
