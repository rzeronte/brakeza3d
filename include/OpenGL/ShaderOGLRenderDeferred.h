#ifndef BRAKEZA3D_SHADEROPENGLGBUFFER_H
#define BRAKEZA3D_SHADEROPENGLGBUFFER_H

#include <vector>
#include <glm/mat4x4.hpp>
#include "Base/ShaderBaseOpenGL.h"
#include "../3D/Mesh3D.h"

class ShaderOGLRenderDeferred : public ShaderBaseOpenGL
{
    GLuint VertexArrayID = 0;

    GLuint matrixModelUniform = 0;
    GLuint drawOffsetUniform = 0;
    GLuint textureDiffuseUniform = 0;
    GLuint textureSpecularUniform = 0;
    GLuint alphaUniform = 1.0f;
    GLuint emissionUniform = 0;

    // Fase 4: buffer persistente de matrices por instancia (attributes 3-6 del VAO
    // compartido). Se respecifica con glBufferData en cada batch instanciado -- el binding de
    // atributos se configura una sola vez en PrepareMainThread() y sigue siendo válido mientras
    // el identificador del buffer no cambie.
    GLuint instanceModelBuffer = 0;
    GLuint useInstancingUniform = 0;

    // Instancing de unidades animadas (Fase 1): matrices de huesos de TODAS las instancias del
    // batch empaquetadas en un buffer plano (4 texels vec4 por matriz), leído en el vertex shader
    // vía samplerBuffer+texelFetch (GL 3.3 core no tiene SSBO, y un array de atributos no escala a
    // ~100 huesos/instancia -- ver .claude/memory/ tras esta sesión). boneMatrixBuffer/Texture se
    // crean una vez en PrepareMainThread(), igual que instanceModelBuffer.
    GLuint boneMatrixBuffer = 0;
    GLuint boneMatrixTexture = 0;
    GLuint useSkinningUniform = 0;
    GLuint bonesPerInstanceUniform = 0;
    GLuint boneMatricesSamplerUniform = 0;
public:
    ShaderOGLRenderDeferred();
    void PrepareMainThread() override;
    void LoadUniforms() override;
    void render(Object3D *o, GLuint texId, GLuint specTexId, GLuint vertexBuffer, GLuint uvBuffer, GLuint normalBuffer, int size, float alpha, GLuint fbo, GLuint indexBuffer = 0, GLsizei indexCount = 0, float emission = 0.0f) const;
    void renderInstanced(GLuint texId, GLuint specTexId, GLuint vertexBuffer, GLuint uvBuffer, GLuint normalBuffer, int size, float alpha, const Vertex3D &drawOffset, GLuint fbo, GLuint indexBuffer, GLsizei indexCount, const std::vector<glm::mat4> &instanceModels, float emission = 0.0f) const;
    void renderInstancedSkinned(GLuint texId, GLuint specTexId, GLuint vertexBuffer, GLuint uvBuffer, GLuint normalBuffer, GLuint vertexBoneDataBuffer, int size, float alpha, const Vertex3D &drawOffset, GLuint fbo, int bonesPerInstance, const std::vector<glm::mat4> &instanceModels, const std::vector<glm::mat4> &allBoneMatrices, float emission = 0.0f) const;
    void renderMesh(Mesh3D *o, bool useFeedbackBuffer, GLuint fbo) const;
    void Destroy() override;
};

#endif //BRAKEZA3D_SHADEROPENGLGBUFFER_H