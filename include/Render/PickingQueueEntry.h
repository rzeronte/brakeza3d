#ifndef BRAKEZA3D_PICKINGQUEUEENTRY_H
#define BRAKEZA3D_PICKINGQUEUEENTRY_H

#include <GL/glew.h>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <vector>

class Object3D;

// Fase 4b del plan de rendimiento: mismo patron que RenderQueueEntry, pero para el pase de
// picking (ShaderOGLColor). Cada submesh visible con MOUSE_CLICK_SELECT_OBJECT3D activo se
// encola aqui en vez de dibujarse al instante. A diferencia del opaco, el color NO entra en la
// comparacion de batch en ComponentRender::FlushPickingQueue -- es el dato POR-INSTANCIA que
// distingue que objeto concreto se pulso (cada instancia ya tiene un submeshPickingColor unico,
// ver ModelData::cloneInto), y viaja junto a la matriz de modelo en el draw instanciado.
//
// Mesh3DAnimation SÍ se encola aquí desde la Fase 2 de instancing con skinning (ver
// isSkinned/boneMatrices abajo, mismo mecanismo TBO que RenderQueueEntry). Image3D se queda
// fuera (dibujo inmediato, sin tocar).
struct PickingQueueEntry {
    Object3D *o = nullptr;
    GLuint vertexBuffer = 0;
    GLuint uvBuffer = 0;
    GLuint normalBuffer = 0;
    int size = 0;
    glm::vec3 color = glm::vec3(0.0f);
    GLuint fbo = 0;
    GLuint indexBuffer = 0;
    GLsizei indexCount = 0;

    // Instancing con skinning (Fase 2, ver .claude/plans). boneMatrices apunta a
    // Mesh3DAnimation::boneTransformCachePerMesh[meshIdx], ya calculado este mismo frame -- solo
    // válido durante este frame, nunca se guarda entre frames.
    bool isSkinned = false;
    GLuint vertexBoneDataBuffer = 0;
    const std::vector<glm::mat4> *boneMatrices = nullptr;
    int boneCount = 0;
};

#endif //BRAKEZA3D_PICKINGQUEUEENTRY_H
