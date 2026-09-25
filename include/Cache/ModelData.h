#ifndef BRAKEZA3D_MODELDATA_H
#define BRAKEZA3D_MODELDATA_H

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "../3D/Vertex3D.h"
#include "../Misc/FilePaths.h"

class Mesh3D;

struct MeshEntryData {
    std::vector<glm::vec4> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;
    std::vector<Vertex3D> triangleVertices;
    int materialIndex = 0;
    std::string name;

    // Geometria GPU COMPARTIDA entre todas las instancias de Mesh3D que cargan el mismo
    // fichero (ver ModelData::sourceFile / ModelDataCache). Se suben una sola vez, la
    // primera vez que una instancia llama a FillOGLBuffers() (hilo principal); las
    // siguientes instancias solo copian estos GLuint, sin nuevas llamadas GL. Fase 2.1
    // del plan de rendimiento -- solo mallas estaticas, Mesh3DAnimation usa su propia
    // cache (AnimationData/animationDataCache), no toca esto.
    GLuint vertexBuffer = 0;
    GLuint uvBuffer = 0;
    GLuint normalBuffer = 0;

    // Fase 2.2: indice compartido, deduplicando vertices identicos (posicion+uv+normal) del
    // stream expandido de arriba. indexCount es el numero de indices (== numero de vertices
    // original, para conservar el mismo orden/cantidad de triangulos que glDrawArrays).
    GLuint indexBuffer = 0;
    GLsizei indexCount = 0;
};

struct MaterialEntryData {
    bool hasDiffuseTexture = false;
    std::string diffuseTexturePath;
    bool hasSpecularTexture = false;
    std::string specularTexturePath;
};

struct ModelData {
    std::vector<MeshEntryData> meshes;
    std::vector<MaterialEntryData> materials;
    std::string sourceFile;

    void cloneInto(Mesh3D& target) const;

    // Solo se destruye cuando el ultimo shared_ptr (el de ModelDataCache o el de
    // cualquier Mesh3D::sharedModel que lo mantenga vivo) desaparece -- por eso es un
    // punto seguro para liberar los buffers de GPU compartidos: nunca corre mientras una
    // instancia todavia los este usando para renderizar.
    ~ModelData();
};

#endif
