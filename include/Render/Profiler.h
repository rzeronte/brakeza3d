#ifndef BRAKEZA3D_PROFILER_H
#define BRAKEZA3D_PROFILER_H
#include <unordered_map>
#include <vector>

#include "Image.h"
#include "ThreadPool.h"

#include "../include/Components/Component.h"

struct Measure {
    double startTime = 0.0f;
    double endTime = 0.0f;
    double diffTime = 0.0f;
    std::vector<float> frameTimeHistory;
    const int MAX_HISTORY = 120;
    // Solo scripts (claves _update): ejecuciones reales de onUpdate por segundo y el
    // ticks_per_second configurado (0 = cada frame, -1 = desconocido). Ver Profiler::CountTick.
    int configuredTps = -1;
    int tickCount = 0;
    double tickWindowStart = 0.0;
    float measuredTps = 0.0f;
};

// GL_TIME_ELAPSED nunca se lee en el mismo frame en que se emite (sincronizaria CPU/GPU).
// Cada pase tiene su propio anillo de query objects: al reutilizar un slot (RING_SIZE frames
// despues) el resultado de esa vuelta anterior ya esta listo casi siempre, sin bloquear nunca
// (si no lo esta todavia, simplemente se descarta esa muestra y se reintenta el siguiente ciclo).
struct GpuMeasure {
    static constexpr int RING_SIZE = 4;
    GLuint queryIds[RING_SIZE] = {0, 0, 0, 0};
    bool queryPending[RING_SIZE] = {false, false, false, false};
    int currentSlot = 0;
    double lastGpuMs = 0.0;
    std::vector<float> gpuTimeHistory;
    const int MAX_HISTORY = 120;
};

using GpuMeasuresMap = std::unordered_map<std::string, GpuMeasure>;

namespace ProfilerConstants {
    constexpr const char* SUFFIX_PRE = "_pre";
    constexpr const char* SUFFIX_UPDATE = "_update";
    constexpr const char* SUFFIX_POST = "_post";
}

using MeasuresMap = std::unordered_map<std::string, Measure>;

class Profiler
{
    static Profiler *instance;

    MeasuresMap componentMeasures;
    MeasuresMap scriptMeasures;
    GpuMeasuresMap gpuMeasures;

    bool enable = false;
    static inline bool measuring = true;
    bool scriptDetailEnabled = false;
    bool gpuTimingEnabled = false;

    Measure measureFrameTime;

    int fboChanges = 0;
    int programChanges = 0;
    int lastFboChanges = 0;
    int lastProgramChanges = 0;
    bool countFboSwitches     = false;
    bool countProgramSwitches = false;

    int drawCalls = 0;
    int triangles = 0;
    int lastDrawCalls = 0;
    int lastTriangles = 0;
    bool countDrawCalls = false;

    // Desglose de draw calls por origen (solo con countDrawCalls): clave "etapa | sitio".
    // Etapa = fase del componente en curso (Render_update, Scripting_post...) + la pasada del
    // render si hay una abierta (GBuffer, ShadowPass...: las mismas marcas que el timing GPU).
    // Sitio = quién emite el glDraw* (la clase real del shader en ShaderBaseOpenGL).
    std::string drawScopeComponent = "frame";
    std::string drawScopeStep;
    std::unordered_map<std::string, int> drawCallsBySource;
    std::unordered_map<std::string, int> lastDrawCallsBySource;

public:
    Profiler() = default;

    void DrawComponentsTable(float cellHeight);
    void DrawImagesTable() const;
    void DrawPlotComponent(Component *c, float height);
    void DrawPlotFrameTime(Measure &measure);
    void DrawPools();
    void DrawWinProfiler();
    void DrawCachesTable() const;
    void ResetTotalFrameTime();
    void EndTotalFrameTime();
    void setEnabled(bool v);
    void DrawFlameGraph();
    void DrawRenderDetail();
    void DrawScriptDetail();
    void UpdateHistory(Measure &measure);
    void DrawComponentsHierarchy();
    void DrawPostProcessingChain();
    void DrawFrameBuffers();
    void DrawOpenGLStatus();
    void DrawCollidersTable();
    [[nodiscard]] bool isEnabled() const;
    [[nodiscard]] bool isScriptDetailEnabled() const;
    void setScriptDetailEnabled(bool v);
    void incrementFboChanges();
    void incrementProgramChanges();
    void incrementDrawCall(GLenum mode, GLsizei count, GLsizei instanceCount = 1, const char *site = nullptr);
    void setDrawScopeComponent(const std::string &scope) { if (countDrawCalls) drawScopeComponent = scope; }
    [[nodiscard]] const std::unordered_map<std::string, int>& getLastDrawCallsBySource() const { return lastDrawCallsBySource; }
    [[nodiscard]] int getLastDrawCalls() const;
    void setCountDrawCalls(bool v) { countDrawCalls = v; }   // Lua: ProfilerLua.setCountDrawCalls
    [[nodiscard]] int getLastTriangles() const;
    [[nodiscard]] MeasuresMap& getComponentMeasures();
    [[nodiscard]] MeasuresMap& getScriptMeasures();
    [[nodiscard]] GpuMeasuresMap& getGpuMeasures();
    [[nodiscard]] bool isGpuTimingEnabled() const;
    void setGpuTimingEnabled(bool v);
    [[nodiscard]] int getNumberOfImages() const;
    [[nodiscard]] int getMemoryImageUsage() const;
    [[nodiscard]] float getMemoryImageUsageKB() const;
    static void DrawPool(const std::string &label, ThreadPool &pool);
    // Interruptor de TODAS las mediciones de tiempo (frame, componentes, tramos Loop_*, scripts, GPU).
    // Se apaga con el parámetro de línea de comandos --no-profiler. Apagado, Start/EndMeasure no hacen nada
    // (los contadores de draw calls siguen su propio interruptor).
    static void setMeasuring(bool v) { measuring = v; }
    [[nodiscard]] static bool isMeasuring() { return measuring; }
    static void InitMeasure(MeasuresMap &map, const std::string & label);
    static void StartMeasure(MeasuresMap &map, const std::string& name);
    static void EndMeasure(MeasuresMap &map, const std::string& name);
    static void CountTick(MeasuresMap &map, const std::string& name, int configuredTps, bool ticked);
    void StartGpuMeasure(const std::string& name);
    void EndGpuMeasure(const std::string& name);
    static void DrawBreakDownComponent(Measure &pre, Measure &update, Measure &post, double total, float height);
    static double Ticks();
    static float AverageHistory(const Measure &m);
    static float AverageGpuHistory(const GpuMeasure &m);
    static float PercentileHistory(const std::vector<float> &history, float percentile);
    std::string ExportFrameStatsCSV() const;
    static Profiler *get();
};

#endif
