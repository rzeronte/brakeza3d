//
// Created by edu on 17/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLCOLOR_H
#define BRAKEZA3D_SHADEROPENGLCOLOR_H


#include "ShaderBaseOpenGL.h"
#include "../Objects/Mesh3D.h"

class ShaderOpenGLColor: public ShaderBaseOpenGL {
    GLuint VertexArrayID;

    GLuint framebuffer;
    GLuint textureColorbuffer;
    GLuint depthBuffer;
public:
    void createBuffer();

    GLuint getFramebuffer() const;

    ShaderOpenGLColor();

    static void setVAOAttributes(GLuint vertexbuffer, GLuint uvbuffer, GLuint normalbuffer);

    void renderColor(
        const glm::mat4 &modelView,
        GLuint vertexbuffer,
        GLuint uvbuffer,
        GLuint normalbuffer,
        int size,
        const Color &color,
        bool clearFramebufer,
        GLuint framebuffer
    ) const;

    void destroy() override;

    void deleteTexture();

    GLuint getTextureColorBuffer() const;

    void renderMesh(Mesh3D* m, const Color &color, bool clearFramebuffer, GLuint framebuffer) const;

};


#endif //BRAKEZA3D_SHADEROPENGLCOLOR_H
