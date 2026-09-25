#ifndef BRAKEZA3D_ANIMATIONDATA_H
#define BRAKEZA3D_ANIMATIONDATA_H

#include <vector>
#include <string>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "../3D/Vertex3D.h"
#include "../3D/Mesh3DAnimation.h"
#include "../Misc/FilePaths.h"
#include "ModelData.h"

struct AnimationMeshEntry {
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<Vertex3D> triangleVertices;
    int materialIndex = 0;
    std::string name;

    // Geometria GPU de pose de reposo, COMPARTIDA entre todas las instancias de
    // Mesh3DAnimation que cargan el mismo fichero (Fase 2.1 extendida a animacion). Son
    // solo lectura para el paso de transform feedback (ShaderOGLBonesTransforms::render())
    // -- la pose actual de cada instancia vive en feedbackBuffer/feedbackNormalBuffer, que
    // siguen siendo por instancia y no se tocan aqui. vertexBoneDataBuffer (pesos de hueso
    // por vertice, GPU) tambien es igual entre instancias del mismo fichero, así que se
    // comparte tambien. boneData (más abajo) es el equivalente de CPU.
    GLuint vertexBuffer = 0;
    GLuint uvBuffer = 0;
    GLuint normalBuffer = 0;
    GLuint vertexBoneDataBuffer = 0;

    // Pesos de hueso por vértice YA expandidos por cara (mismo shape que
    // Mesh3DAnimation::meshVerticesBoneData[i]) -- derivados solo del aiMesh inmutable, iguales
    // para cualquier instancia del mismo fichero. Cacheados aquí para no re-recorrer Assimp
    // (LoadMeshBones + expansión por cara) en cada instancia nueva de un fichero ya cacheado.
    // A diferencia de vertexBoneDataBuffer (GPU), esto se COPIA a cada instancia (no se
    // referencia) -- Mesh3DAnimation::boneInfo/WorldTransform sí es por instancia y por eso
    // LoadMeshBones sigue corriendo siempre, solo que sin recalcular estos pesos.
    std::vector<VertexBoneData> boneData;
};

struct AnimationData {
    std::vector<AnimationMeshEntry> meshes;
    std::vector<MaterialEntryData> materials;
    std::string sourceFile;

    void cloneInto(Mesh3DAnimation& target) const;

    // Igual que ModelData::~ModelData(): solo corre cuando el ultimo shared_ptr
    // desaparece (el de AnimationDataCache o el de Mesh3DAnimation::sharedAnimModel).
    ~AnimationData();
};

#endif
