//
// Created by Eduardo on 29/11/2025.
//

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

    std::vector<float> frameTimeHistory;  // Historial de tiempos
    const int MAX_HISTORY = 120;  // 2 segundos a 60fps
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

    std::vector<Image *> images;
    int memoryImageUsage = 0;

    int numberOfGUIImages = 0;
    int memoryOfGUIImages = 0;
    bool enable = false;

    Measure measureFrameTime;

public:
    Profiler() = default;

    float getMemoryImageUsageKB() const;
    void AddImage(Image *image);
    void RemoveImage(const Image *image);
    void DrawComponentsTable(float cellHeight);
    void DrawImagesTable() const;
    void DrawPlotComponent(Component *c, float height);
    void DrawPlotFrameTime(Measure &measure);
    void CaptureGUIMemoryUsage();
    void DrawPools();
    void DrawWinProfiler();
    void ResetTotalFrameTime();
    void EndTotalFrameTime();
    void setEnabled(bool v);
    void DrawFlameGraph();
    void UpdateHistory(Measure &measure);
    void DrawComponentsHierarchy();
    [[nodiscard]] bool isEnabled() const;
    [[nodiscard]] MeasuresMap& getComponentMeasures();
    [[nodiscard]] int getNumberOfImages() const;
    [[nodiscard]] int getMemoryImageUsage() const;
    static void DrawPool(const std::string &label, ThreadPool &pool);
    static void InitMeasure(MeasuresMap &map, const std::string & label);
    static void StartMeasure(MeasuresMap &map, const std::string& name);
    static void EndMeasure(MeasuresMap &map, const std::string& name);
    static void DrawBreakDownComponent(Measure &pre, Measure &update, Measure &post, double total, float height);
    static double Ticks();
    static Profiler *get();
};


#endif //BRAKEZA3D_PROFILER_H