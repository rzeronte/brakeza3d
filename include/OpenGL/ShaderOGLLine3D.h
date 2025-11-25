//
// Created by Eduardo on 24/01/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLLINE3D_H
#define BRAKEZA3D_SHADEROPENGLLINE3D_H


#include "../../include/OpenGL/ShaderBaseOpenGL.h"
#include "../3D/Vertex3D.h"
#include "../../include/Misc/Color.h"
#include "../3D/Vector3D.h"
#include <vector>

class ShaderOGLLine3D : public ShaderBaseOpenGL
{
    GLuint VertexArrayID;
    GLuint vertexbuffer;
    glm::vec3 vertices[2];

    GLuint matrixProjectionUniform;
    GLuint matrixViewUniform;
    GLuint colorUniform;

    void destroy() override;

public:
    ShaderOGLLine3D();

    void render(
        Vertex3D from,
        Vertex3D to,
        GLuint fbo,
        Color c
    );

    void renderLines(
        const std::vector<Vector3D>&,
        GLuint fbo,
        Color c
    );
};


#endif //BRAKEZA3D_SHADEROPENGLLINE3D_H
