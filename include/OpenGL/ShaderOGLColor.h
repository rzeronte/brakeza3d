//
// Created by edu on 17/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLCOLOR_H
#define BRAKEZA3D_SHADEROPENGLCOLOR_H

#include "ShaderBaseOpenGL.h"
#include "../Objects/Mesh3D.h"

class ShaderOGLColor: public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;
    GLuint framebuffer = 0;
    GLuint textureColorBuffer= 0;
    GLuint depthBuffer= 0;
public:
    void createBuffer();
    ShaderOGLColor();
    void renderColor(
        const glm::mat4 &modelView,
        GLuint vertexBuffer,
        GLuint uvBuffer,
        GLuint normalBuffer,
        int size,
        const Color &color,
        bool clearFramebuffer,
        GLuint fbo
    ) const;
    void destroy() override;
    void deleteTexture() const;
    void renderMesh(Mesh3D* m, bool useFeedbackBuffer, const Color &color, bool clearFramebuffer, GLuint fbo) const;
    [[nodiscard]] GLuint getTextureColorBuffer() const;
    [[nodiscard]] GLuint getFramebuffer() const;
};


#endif //BRAKEZA3D_SHADEROPENGLCOLOR_H
