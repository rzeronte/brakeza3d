//
// Created by edu on 28/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLLINELASER_H
#define BRAKEZA3D_SHADEROPENGLLINELASER_H


#include "../../include/OpenGL/ShaderOpenGL.h"
#include "../../include/Objects/Point2D.h"
#include "../../include/Misc/Color.h"
#include "../../include/Misc/Image.h"
#include "../../include/OpenGL/ShaderQuadOpenGL.h"

class ShaderOpenGLLineLaser : public ShaderOpenGL, public ShaderQuadOpenGL  {
    GLint textureUniform;
    GLint maskDirectionUniform;
    GLint lineStartUniform;
    GLint lineEndUniform;
    GLint lineColorUniform;
    GLint weightUniform;
    GLint timeUniform;
    GLint maskSpeedUniform;
    GLint maskIntensityUniform;
public:
    ShaderOpenGLLineLaser();

    void render(GLuint textureMaskID, Point2D a, Point2D b, Color c, float weight, GLuint framebuffer);

    void destroy() override;

};

#endif //BRAKEZA3D_SHADEROPENGLLINELASER_H