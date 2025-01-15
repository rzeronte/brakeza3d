
#ifndef BRAKEZA3D_SHADEROPENGLPOINTS_H
#define BRAKEZA3D_SHADEROPENGLPOINTS_H

#include "ShaderOpenGL.h"
#include "../Objects/OCParticlesContext.h"

class ShaderOpenGLPoints : public ShaderOpenGL {
    GLuint VertexArrayID;
public:
    ShaderOpenGLPoints();

    void render(GLint particlesBuffer, int numberPoints, Color cf, Color ct);

    static void setVAOAttributes(GLint particlesBuffer) ;

    void destroy() override;
};


#endif //BRAKEZA3D_SHADEROPENGLPOINTS_H
