
#ifndef BRAKEZA3D_SHADEROPENGLPOINTS_H
#define BRAKEZA3D_SHADEROPENGLPOINTS_H

#include "ShaderBaseOpenGL.h"
#include "../Objects/Mesh3D.h"
#include "../Objects/Mesh3DAnimation.h"

class ShaderOGLPoints : public ShaderBaseOpenGL
{
    GLuint VertexArrayID;
public:
    ShaderOGLPoints();

    void render(Mesh3D* m, GLuint particlesBuffer, int numberPoints, Color c, GLuint fbo);
    void destroy() override;
    void renderMesh(Mesh3D *mesh, bool useFeedbackBuffer, GLuint fbo);
    void renderMeshAnimation(Mesh3DAnimation *mesh, GLuint fbo);
    static void setVAOAttributes(GLuint particlesBuffer) ;
};


#endif //BRAKEZA3D_SHADEROPENGLPOINTS_H
