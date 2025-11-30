//
// Created by Eduardo on 29/11/2025.
//

#ifndef BRAKEZA3D_PROFILER_H
#define BRAKEZA3D_PROFILER_H
#include <unordered_map>
#include <vector>

#include "Image.h"

#include "../include/Components/Component.h"

struct ProfilerMeasure {
    double startTime = 0.0f;
    double endTime = 0.0f;
    double diffTime = 0.0f;

    std::vector<double> history;
    double averageTime = 0.0;
    static const int MAX_SAMPLES = 60;  // Promedio de 60 frames (1 segundo a 60fps)

    void UpdateAverage() {
        history.push_back(diffTime);

        // Mantener solo los últimos MAX_SAMPLES
        if (history.size() > MAX_SAMPLES) {
            history.erase(history.begin());
        }

        // Calcular promedio
        double sum = 0.0;
        for (double time : history) {
            sum += time;
        }
        averageTime = history.empty() ? 0.0 : sum / history.size();
    }
};

using MeasuresMap = std::unordered_map<std::string, ProfilerMeasure>;

class Profiler
{
    static Profiler *instance;

    MeasuresMap componentMeasures;

    std::vector<Image *> images;
    int memoryImageUsage = 0;

    int numberOfGUIImages = 0;
    int memoryOfGUIImages = 0;
    bool enable = false;

    ProfilerMeasure frameTime;

    std::vector<float> frameTimeHistory;  // Historial de tiempos
    const int MAX_HISTORY = 120;  // 2 segundos a 60fps
public:
    Profiler() = default;

    static Profiler *get();
    float getMemoryImageUsageKB() const;

    void AddImage(Image *image);
    void RemoveImage(const Image *image);
    void DrawComponentsTable();
    void DrawImagesTable() const;

    void DrawPlot();

    void CaptureGUIMemoryUsage();
    void DrawPropertiesGUI();
    void ResetMeasure(MeasuresMap &map, const std::string & label);
    void ResetTotalFrameTime();
    void EndTotalFrameTime();
    void setEnabled(bool v);
    [[nodiscard]] bool isEnabled() const;
    static void StartMeasure(MeasuresMap &map, const std::string& name);
    static void EndMeasure(MeasuresMap &map, const std::string& name);
    static double ticks();

    void DrawFlameGraph();

    [[nodiscard]] MeasuresMap& getComponentMeasures();
    [[nodiscard]] int getNumberOfImages() const;
    [[nodiscard]] int getMemoryImageUsage() const;

    void UpdateHistory();

    void DrawComponentsHierarchy();
};


#endif //BRAKEZA3D_PROFILER_H