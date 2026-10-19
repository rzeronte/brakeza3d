#ifndef BRAKEZA3D_CROWDSOLVER_H
#define BRAKEZA3D_CROWDSOLVER_H

#include <cstdint>
#include <functional>
#include <vector>

class Object3D;

// Resolución de solapes entre "agentes" (objetos con un radio) en el plano XZ. Genérico: el motor no sabe qué es un agente
// (unidad, civil, coche...), solo tiene radio, prioridad y capas.
//
// - Restricción DURA: cada frame, tras ejecutar los scripts, los pares que se solapan se separan hasta tocarse (Gauss-Seidel
//   sobre un hash espacial). Sustituye a las fuerzas suaves de separación, que pierden contra la velocidad de la unidad.
// - Prioridad: al separar un par, el de mayor prioridad se mueve menos (masa = 4^prioridad). El script sube la prioridad de
//   quien debe "mantenerse firme" (disparando, parado) y la baja de quien debe ceder; un agente de prioridad CROWD_MAX_PRIORITY no
//   cede apenas (masa 1024 frente a 1). Prioridad AUTO (-1, por defecto en Lua): la decide el motor cada frame según lo que el
//   script mueva al agente: parado (menos de autoStillSpeed durante autoStillDelay s) = autoStillPriority, en marcha =
//   autoMovingPriority. Así el que se mueve cede ante el que está parado sin que cada IA tenga que decirlo.
// - Capas: un par solo interactúa si cada uno colisiona con la capa del otro (por defecto, todo con todo).
// - La corrección por agente y frame se limita (maxPushSpeed u/s) para no teletransportar a nadie a través de una pared, y un
//   callback opcional de "caminable" revierte (o desliza en un solo eje) las correcciones que dejarían al agente en un sitio
//   prohibido.
//
// Alta/baja desde Lua (Crowd.add / Crowd.remove); ~Object3D() da de baja el objeto. Sin agentes no hace ningún trabajo.
// Solo hilo principal.
//
// Además, utilidades puras de geometría para repartir puestos (formaciones, arcos de ataque) y asignarlos: no tocan agentes.
class CrowdSolver
{
public:
    static constexpr int CROWD_MAX_PRIORITY = 5;

    struct Config {
        int   iterations   = 3;       // pasadas de relajación por frame
        float maxPushSpeed = 6.0f;    // u/s máximos de corrección por agente (0 = sin límite)
        float stiffness    = 0.9f;    // fracción del solape que se corrige por pasada (0..1)
        float margin       = 0.0f;    // holgura añadida a la suma de radios
        bool  enabled      = true;
        float autoStillSpeed        = 0.4f;   // u/s por debajo de los cuales el script "no mueve" al agente
        float autoStillDelay        = 0.25f;  // s seguidos parado para pasar a prioridad de parado
        int   autoMovingPriority    = 1;
        int   autoStillPriority     = 2;
    };

    struct Stats {
        int   agents   = 0;           // agentes activos en el último step
        int   pairs    = 0;           // pares solapados corregidos (suma de todas las pasadas)
        int   moved    = 0;           // agentes desplazados
        int   reverted = 0;           // desplazamientos anulados por el callback de caminable
        float maxPush  = 0.0f;        // mayor corrección aplicada a un agente (u)
        float stepMs   = 0.0f;
    };

    struct Slot { float x, z; };

private:
    struct Agent {
        Object3D *obj;
        float radius;
        int priority;               // -1 = automática (ver Config::auto*)
        uint32_t layer;
        uint32_t collidesWith;
        float lastX = 0.0f, lastZ = 0.0f;   // posición final del frame anterior (para medir lo que mueve el script)
        float stillT = 0.0f;
        bool hasLast = false;
    };

    struct Work {
        Object3D *obj;
        float x, z;          // posición actual (se va corrigiendo)
        float ox, oz;        // posición de partida del frame
        float radius;
        float mass;
        uint32_t layer, collidesWith;
    };

    static constexpr uint32_t BUCKETS = 4096;   // potencia de 2

    std::vector<Agent> agents;                  // Object3D::crowdSlot = posición aquí
    std::vector<Work> work;
    std::vector<uint32_t> bucketStart;          // BUCKETS + 1
    std::vector<uint32_t> cursor;
    std::vector<uint32_t> order;                // índices de work ordenados por cubo
    std::vector<uint32_t> cellBucket;           // cubo de cada work
    std::function<bool(float, float)> walkable;
    Config cfg;
    Stats stats;

    [[nodiscard]] static uint32_t bucketOf(int cx, int cz);

public:
    CrowdSolver();

    // Alta o actualización (si ya estaba, cambia sus valores)
    void add(Object3D *obj, float radius, int priority, uint32_t layer, uint32_t collidesWith);
    void remove(Object3D *obj);            // también lo llama ~Object3D()
    void clear();
    void setPriority(Object3D *obj, int priority);
    void setRadius(Object3D *obj, float radius);
    [[nodiscard]] bool has(const Object3D *obj) const;
    [[nodiscard]] int size() const { return (int) agents.size(); }

    Config &config() { return cfg; }
    [[nodiscard]] const Stats &getStats() const { return stats; }
    void setWalkable(std::function<bool(float, float)> fn) { walkable = std::move(fn); }

    // Una vez por frame, después de los scripts
    void step(float dt);

    // ── Geometría de puestos (sin estado) ────────────────────────────────────────────────────────
    // count puestos en un círculo de radio `radius` alrededor de (cx, cz), empezando en startAngle (rad)
    static std::vector<Slot> ringSlots(float cx, float cz, float radius, int count, float startAngle);
    // count puestos en retícula hexagonal compacta alrededor de (cx, cz), del centro hacia fuera; la retícula se orienta con
    // (dirX, dirZ) = hacia dónde "mira" la formación (la fila delantera es perpendicular a esa dirección)
    static std::vector<Slot> packedSlots(float cx, float cz, int count, float spacing, float dirX, float dirZ);
    // count puestos en arco a distancia `dist` de (tx, tz), centrados en la dirección hacia (fromX, fromZ), por orden de
    // preferencia (centro, derecha, izquierda, ...). Si el anillo se llena, siguientes anillos a dist + spacing, +2 spacing...
    static std::vector<Slot> arcSlots(float tx, float tz, float fromX, float fromZ, float dist, float spacing, int count);
    // Asigna a cada punto de `from` un puesto de `slots` (sin repetir) minimizando el recorrido de forma voraz: se toman los
    // pares más cortos primero. Devuelve, por cada punto, el índice (0-based) de su puesto, o -1 si no hay puestos suficientes.
    static std::vector<int> assignSlots(const std::vector<Slot> &from, const std::vector<Slot> &slots);
};

#endif //BRAKEZA3D_CROWDSOLVER_H
