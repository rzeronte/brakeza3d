//
// Created by edu on 17/12/23.
//

#ifndef BRAKEZA3D_SHADEROPENGLCOLOR_H
#define BRAKEZA3D_SHADEROPENGLCOLOR_H

#include <vector>
#include <glm/mat4x4.hpp>
#include "Base/ShaderBaseOpenGL.h"
#include "../3D/Mesh3D.h"

class ShaderOGLColor: public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;
    GLuint framebuffer = 0;
    GLuint textureColorBuffer = 0;
    GLuint depthBuffer= 0;

    // Picking instanciado (mismo patrón que ShaderOGLRenderDeferred, Fase 4): buffers
    // persistentes de matriz por instancia (attributes 3-6) y color por instancia (attribute 7)
    // -- el color ya es único por instancia desde antes (ModelData::cloneInto asigna un
    // submeshPickingId nuevo por copia), esto solo cambia CÓMO se sube a la GPU.
    GLuint instanceModelBuffer = 0;
    GLuint instanceColorBuffer = 0;

    // Picking instanciado con skinning (Fase 2): TBO propio de este shader (no se comparte con el
    // de ShaderOGLRenderDeferred), mismo patrón que ahí -- creado una vez en PrepareMainThread(),
    // respecificado con glBufferData en cada batch.
    GLuint boneMatrixBuffer = 0;
    GLuint boneMatrixTexture = 0;
public:
    void CreateBuffer();
    ShaderOGLColor();

    void PrepareMainThread() override;

    void LoadUniforms() override;
    void RenderColor(
        const glm::mat4 &modelView,
        GLuint vertexBuffer,
        GLuint uvBuffer,
        GLuint normalBuffer,
        int size,
        const Color &c,
        bool clearFBO,
        GLuint fbo,
        GLuint indexBuffer = 0,
        GLsizei indexCount = 0
    ) const;
    void RenderTint(
        const glm::mat4 &model,
        GLuint vertexBuffer,
        GLuint uvBuffer,
        GLuint normalBuffer,
        int size,
        const Color &c,
        float alpha,
        GLuint fbo,
        GLuint indexBuffer = 0,
        GLsizei indexCount = 0
    ) const;
    void RenderColorInstanced(
        GLuint vertexBuffer,
        GLuint uvBuffer,
        GLuint normalBuffer,
        int size,
        GLuint fbo,
        GLuint indexBuffer,
        GLsizei indexCount,
        const std::vector<glm::mat4> &instanceModels,
        const std::vector<glm::vec3> &instanceColors
    ) const;
    void RenderColorInstancedSkinned(
        GLuint vertexBuffer,
        GLuint uvBuffer,
        GLuint normalBuffer,
        GLuint vertexBoneDataBuffer,
        int size,
        GLuint fbo,
        int bonesPerInstance,
        const std::vector<glm::mat4> &instanceModels,
        const std::vector<glm::vec3> &instanceColors,
        const std::vector<glm::mat4> &allBoneMatrices
    ) const;
    void Destroy() override;
    void DeleteTexture() const;
    void renderMesh(Mesh3D* m, bool useFeedbackBuffer, const Color &color, bool clearFramebuffer, GLuint fbo) const;
    [[nodiscard]] GLuint getTextureColorBuffer() const;
    [[nodiscard]] GLuint getFramebuffer() const;
};


#endif //BRAKEZA3D_SHADEROPENGLCOLOR_H
