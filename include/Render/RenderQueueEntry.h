#ifndef BRAKEZA3D_RENDERQUEUEENTRY_H
#define BRAKEZA3D_RENDERQUEUEENTRY_H

#include <GL/glew.h>
#include <vector>
#include <glm/mat4x4.hpp>

class Object3D;

// Fase 3 (Etapa 1) del plan de rendimiento: una entrada por submesh visible, encolada durante
// la Pasada 2 de ComponentRender::onUpdateSceneObjects() en vez de dibujarse al instante. Mismos
// parámetros que ya recibía ShaderOGLRenderDeferred::render() por llamada -- esto NO cambia qué
// se dibuja ni con qué datos, solo cuándo (al final, en bloque, ordenado) en vez de uno a uno
// intercalado con Outline/picking/shaders de objeto.
//
// Alcance original limitado a Mesh3D estático + pipeline Deferred por defecto (ver
// .claude/memory/performance-plan.md, Fase 3); Forward, transparencias y sombras se quedan fuera
// de esta cola. Mesh3DAnimation sí se encola aquí (Etapa 2 de esa misma fase, y más tarde el
// instancing con skinning de la Fase 1 posterior -- ver campos isSkinned/boneMatrices abajo).
struct RenderQueueEntry {
    Object3D *o = nullptr;
    GLuint texId = 0;
    GLuint specTexId = 0;
    GLuint vertexBuffer = 0;
    GLuint uvBuffer = 0;
    GLuint normalBuffer = 0;
    int size = 0;
    float alpha = 1.0f;
    float emission = 0.0f;   // >0 solo en entradas de emissiveQueue (Mesh3D::emissionEnabled)
    GLuint fbo = 0;
    GLuint indexBuffer = 0;
    GLsizei indexCount = 0;

    // Instancing de unidades animadas (Fase 1, ver .claude/plans -- ComponentRender::EnqueueOpaque/
    // FlushOpaqueQueue). boneMatrices apunta a Mesh3DAnimation::boneTransformCachePerMesh[meshIdx],
    // ya calculado este mismo frame por UpdateOpenGLBones() antes de encolar -- solo válido durante
    // este frame, nunca se guarda entre frames.
    bool isSkinned = false;
    GLuint vertexBoneDataBuffer = 0;
    const std::vector<glm::mat4> *boneMatrices = nullptr;
    int boneCount = 0;
};

#endif //BRAKEZA3D_RENDERQUEUEENTRY_H
