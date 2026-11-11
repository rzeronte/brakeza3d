#ifndef BRAKEZA3D_MESHINSTANCES_H
#define BRAKEZA3D_MESHINSTANCES_H

#include <cstdint>
#include <string>
#include <vector>
#include <glm/mat4x4.hpp>
#include "Vertex3D.h"

// Lista de instancias de un Mesh3D: el mismo modelo dibujado N veces (arboles, rocas, farolas...) con UNA sola copia de la
// geometria en memoria y un draw instanciado por submesh. Es una capacidad opcional de Mesh3D, como el Octree o el Grid3D:
// sin lista el Mesh3D se comporta como siempre.
//
// Las instancias se agrupan en chunks (zonas) con su caja; cada frame solo se suben a la GPU las de los chunks que se ven
// (frustum) y estan dentro de la distancia maxima. Las matrices del fichero son LOCALES al objeto: la matriz de dibujo es
// modelMatrix del Mesh3D * matriz de la instancia.
//
// Formato del fichero (little-endian), lo escribe tools/export_scatter.py:
//   char magic[4] = "BZIN"; uint32 version = 1; uint32 chunkCount; uint32 instanceCount;
//   por chunk (32 B):     float min[3]; float max[3]; uint32 first; uint32 count;      // caja local, ya con el tamano del modelo
//   por instancia (40 B): float pos[3]; float quat[4] (x,y,z,w); float scale[3];       // ordenadas por chunk
class MeshInstances
{
public:
    struct Chunk {
        Vertex3D min;
        Vertex3D max;
        uint32_t first = 0;
        uint32_t count = 0;
    };

    bool loadFromFile(const std::string &path);

    // Reconstruye getVisible() con las matrices de MUNDO de las instancias de los chunks visibles. maxDistance <= 0: sin limite.
    // Hilo principal, una vez por frame.
    void cull(const glm::mat4 &model, const Vertex3D &cameraPos, float maxDistance);

    [[nodiscard]] bool empty() const                                  { return matrices.empty(); }
    [[nodiscard]] size_t getTotalCount() const                        { return matrices.size(); }
    [[nodiscard]] size_t getChunkCount() const                        { return chunks.size(); }
    [[nodiscard]] size_t getVisibleChunkCount() const                 { return visibleChunks; }
    [[nodiscard]] const std::vector<glm::mat4> &getVisible() const    { return visible; }
    [[nodiscard]] const Vertex3D &getLocalMin() const                 { return localMin; }
    [[nodiscard]] const Vertex3D &getLocalMax() const                 { return localMax; }

private:
    std::vector<Chunk> chunks;
    std::vector<glm::mat4> matrices;   // locales, ordenadas por chunk
    std::vector<glm::mat4> visible;    // de mundo, solo este frame
    size_t visibleChunks = 0;
    Vertex3D localMin = Vertex3D::zero();
    Vertex3D localMax = Vertex3D::zero();
};

#endif //BRAKEZA3D_MESHINSTANCES_H
