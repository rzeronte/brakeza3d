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

// Geometría de sombra combinada (2026-10-10): posiciones de TODAS las mallas de un modelo
// concatenadas en espacio local, en un solo buffer. El shadow pass es solo profundidad -- sin
// texturas ni materiales nada distingue un submesh de otro --, así que dibuja el modelo entero en
// una llamada, instanciada entre todas sus instancias (ver ShaderOGLShadowPass::drawCastersInstanced).
// Se crea al primer uso en el hilo principal (Mesh3D::getShadowMerged). Una copia por modelo:
// vive en ModelData si las instancias lo comparten, o en la propia instancia si no.
struct ShadowMergedGeometry {
    GLuint buffer = 0;
    GLsizei vertexCount = 0;
    bool tried = false;
    glm::vec3 localMin{0.0f};
    glm::vec3 localMax{0.0f};
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
    ShadowMergedGeometry shadowMerged;   // ver ShadowMergedGeometry

    void cloneInto(Mesh3D& target) const;

    // Solo se destruye cuando el ultimo shared_ptr (el de ModelDataCache o el de
    // cualquier Mesh3D::sharedModel que lo mantenga vivo) desaparece -- por eso es un
    // punto seguro para liberar los buffers de GPU compartidos: nunca corre mientras una
    // instancia todavia los este usando para renderizar.
    ~ModelData();
};

#endif
