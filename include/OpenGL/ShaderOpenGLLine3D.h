//
// Created by Eduardo on 24/01/2025.
//

#ifndef BRAKEZA3D_SHADEROPENGLLINE3D_H
#define BRAKEZA3D_SHADEROPENGLLINE3D_H


#include "../../include/OpenGL/ShaderBaseOpenGL.h"
#include "../../include/Objects/Vertex3D.h"
#include "../../include/Misc/Color.h"
#include "../../include/Objects/Vector3D.h"
#include <vector>

class ShaderOpenGLLine3D : public ShaderBaseOpenGL
{
    GLuint VertexArrayID;
    GLuint vertexbuffer;
    glm::vec3 vertices[2];

    GLint matrixProjectionUniform;
    GLint matrixViewUniform;
    GLint colorUniform;

    void destroy() override;

public:
    ShaderOpenGLLine3D();

    void render(
        Vertex3D from,
        Vertex3D to,
        GLuint framebuffer,
        Color c
    );

    void renderLines(
        const std::vector<Vector3D>&,
        GLuint framebuffer,
        Color c
    );
};


#endif //BRAKEZA3D_SHADEROPENGLLINE3D_H
