
#ifndef BRAKEZA3D_SHADEROPENGLPOINTS_H
#define BRAKEZA3D_SHADEROPENGLPOINTS_H

#include "ShaderOpenGL.h"
#include "../Objects/OpenCLTypes.h"

class ShaderOpenGLPoints : public ShaderOpenGL {
    GLuint VertexArrayID;
public:
    ShaderOpenGLPoints(const std::string &vertexFilename, const std::string &fragmentFilename);

    void render(GLint particlesBuffer, int numberPoints, Color cf, Color ct);

    static void setVAOAttributes(GLint particlesBuffer) ;
};


#endif //BRAKEZA3D_SHADEROPENGLPOINTS_H
