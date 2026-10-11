#ifndef BRAKEZA3D_SPATIALINDEX_H
#define BRAKEZA3D_SPATIALINDEX_H

#include <cstdint>
#include <mutex>
#include <vector>

class Object3D;

// Índice espacial en el plano XZ para consultas "qué hay cerca de (x, z)" desde la lógica de juego.
// Genérico: el motor no sabe qué significan los bits de la máscara (facción, tipo...), solo hace un
// AND entre la máscara del objeto y la de la consulta.
//
// Bajo demanda: un objeto entra en el índice cuando se le pone una máscara distinta de 0
// (Object3D:setSpatialMask desde Lua) y el índice solo se reconstruye en la primera consulta de
// cada frame. Si nadie marca objetos ni consulta, no se hace ningún trabajo.
//
// Es una foto por frame: posiciones del momento de la reconstrucción. Solo hilo principal para
// consultar; el registro tiene su propio mutex porque ~Object3D() se da de baja aquí (nunca
// objectsMutex, ver Brakeza::livePointersMutex).
//
// Estructura: hash espacial con ordenación por conteo (arrays planos, sin reservar memoria tras el
// primer frame). Cada entrada guarda su celda, así que dos celdas que caen en el mismo cubo del hash
// no producen resultados duplicados.
class SpatialIndex
{
public:
    struct Hit {
        unsigned int id;
        float x, y, z;
        float d2;
    };

private:
    struct Entry {
        float x, y, z;
        int cx, cz;
        uint32_t mask;
        unsigned int id;
        bool enabled;
    };

    static constexpr uint32_t BUCKETS = 4096;   // potencia de 2

    float cellSize = 16.0f;
    std::vector<Object3D *> members;            // objetos con máscara != 0 (Object3D::spatialSlot = posición)
    std::vector<Entry> scratch;                 // entradas sin ordenar (reconstrucción)
    std::vector<Entry> entries;                 // entradas ordenadas por cubo
    std::vector<uint32_t> bucketStart;          // BUCKETS + 1 prefijos
    std::vector<uint32_t> cursor;               // posición de escritura por cubo (reconstrucción)
    std::vector<Hit> hits;                      // resultados temporales de query()
    uint64_t builtFrame = UINT64_MAX;
    mutable std::mutex membersMutex;

    [[nodiscard]] int cellOf(float v) const;
    [[nodiscard]] static uint32_t bucket(int cx, int cz);
    void ensureBuilt();
    void rebuild();

    // Llama fn(entry, d2) por cada entrada a distancia <= r que cumple máscara/habilitado
    template<typename F>
    void forEachInRadius(float x, float z, float r, uint32_t mask, bool enabledOnly, F &&fn);

public:
    SpatialIndex();

    // Alta/baja/cambio de máscara (0 = fuera del índice)
    void setMask(Object3D *obj, uint32_t mask);
    // Llamado desde ~Object3D() si el objeto estaba en el índice
    void unregister(Object3D *obj);

    void setCellSize(float size);
    [[nodiscard]] float getCellSize() const { return cellSize; }
    [[nodiscard]] int size() const;

    // La más cercana a distancia <= r (id 0 = ninguna). excludeId: normalmente el propio llamador.
    bool nearest(float x, float z, float r, uint32_t mask, unsigned int excludeId, bool enabledOnly, Hit &out);
    // Hasta maxResults (<= 0 = sin límite) resultados; sorted = de más cerca a más lejos
    const std::vector<Hit> &query(float x, float z, float r, uint32_t mask, int maxResults, bool sorted, bool enabledOnly);
    int count(float x, float z, float r, uint32_t mask, bool enabledOnly);

    // Depuración: cuadrados de las celdas ocupadas y una cruz por objeto indexado
    void drawDebug(float r, float g, float b, float a);
};

#endif //BRAKEZA3D_SPATIALINDEX_H
