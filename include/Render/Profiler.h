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
    void incrementDrawCall(GLenum mode, GLsizei count, GLsizei instanceCount = 1);
    [[nodiscard]] int getLastDrawCalls() const;
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
    static void InitMeasure(MeasuresMap &map, const std::string & label);
    static void StartMeasure(MeasuresMap &map, const std::string& name);
    static void EndMeasure(MeasuresMap &map, const std::string& name);
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
