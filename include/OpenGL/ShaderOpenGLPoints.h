
#ifndef BRAKEZA3D_SHADEROPENGLPOINTS_H
#define BRAKEZA3D_SHADEROPENGLPOINTS_H

#include "ShaderOpenGL.h"
#include "../Objects/ParticlesContext.h"
#include "../Objects/Mesh3D.h"

class ShaderOpenGLPoints : public ShaderOpenGL {
    GLuint VertexArrayID;
public:
    ShaderOpenGLPoints();

    void render(Mesh3D* m, GLint particlesBuffer, int numberPoints, Color c, GLuint framebuffer);

    static void setVAOAttributes(GLint particlesBuffer) ;

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLPOINTS_H
