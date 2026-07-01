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

    bool enable = false;
    bool scriptDetailEnabled = false;

    Measure measureFrameTime;

    int fboChanges = 0;
    int programChanges = 0;
    int lastFboChanges = 0;
    int lastProgramChanges = 0;
    bool countFboSwitches     = false;
    bool countProgramSwitches = false;

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
    [[nodiscard]] MeasuresMap& getComponentMeasures();
    [[nodiscard]] MeasuresMap& getScriptMeasures();
    [[nodiscard]] int getNumberOfImages() const;
    [[nodiscard]] int getMemoryImageUsage() const;
    [[nodiscard]] float getMemoryImageUsageKB() const;
    static void DrawPool(const std::string &label, ThreadPool &pool);
    static void InitMeasure(MeasuresMap &map, const std::string & label);
    static void StartMeasure(MeasuresMap &map, const std::string& name);
    static void EndMeasure(MeasuresMap &map, const std::string& name);
    static void DrawBreakDownComponent(Measure &pre, Measure &update, Measure &post, double total, float height);
    static double Ticks();
    static float AverageHistory(const Measure &m);
    static Profiler *get();
};

#endif
