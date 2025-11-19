
#ifndef BRAKEZA3D_SHADEROPENGLPOINTS_H
#define BRAKEZA3D_SHADEROPENGLPOINTS_H

#include "ShaderBaseOpenGL.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Mesh3DAnimation.h"

class ShaderOpenGLPoints : public ShaderBaseOpenGL {
    GLuint VertexArrayID;
public:
    ShaderOpenGLPoints();

    void render(Mesh3D* m, GLint particlesBuffer, int numberPoints, Color c, GLuint framebuffer);

    static void setVAOAttributes(GLint particlesBuffer) ;

    void destroy() override;

    void renderMesh(Mesh3D *mesh, GLuint framebuffer);

    void renderMeshAnimation(Mesh3DAnimation *mesh, GLuint framebuffer);
};


#endif //BRAKEZA3D_SHADEROPENGLPOINTS_H
