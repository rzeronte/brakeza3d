//
// Created by Eduardo on 29/11/2025.
//

#include "Profiler.h"
#include <algorithm>
#include <cstdint>
#include <string>

#if defined(_WIN32)
  #include <windows.h>
#elif defined(__linux__)
  #include <sys/sysinfo.h>
#elif defined(__APPLE__)
  #include <sys/types.h>
  #include <sys/sysctl.h>
  #include <mach/mach.h>
#endif

static void GetSystemMemoryMB(uint64_t &outTotal, uint64_t &outFree)
{
#if defined(_WIN32)
    MEMORYSTATUSEX s;
    s.dwLength = sizeof(s);
    GlobalMemoryStatusEx(&s);
    outTotal = s.ullTotalPhys  / (1024 * 1024);
    outFree  = s.ullAvailPhys  / (1024 * 1024);
#elif defined(__linux__)
    struct sysinfo s{};
    sysinfo(&s);
    outTotal = (s.totalram  * s.mem_unit) / (1024 * 1024);
    outFree  = (s.freeram   * s.mem_unit) / (1024 * 1024);
#elif defined(__APPLE__)
    int mib[2] = { CTL_HW, HW_MEMSIZE };
    uint64_t total = 0;
    size_t len = sizeof(total);
    sysctl(mib, 2, &total, &len, nullptr, 0);
    outTotal = total / (1024 * 1024);
    vm_statistics64_data_t vmStats{};
    mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
    host_statistics64(mach_host_self(), HOST_VM_INFO64,
                      reinterpret_cast<host_info64_t>(&vmStats), &count);
    outFree = (static_cast<uint64_t>(vmStats.free_count) * PAGE_SIZE) / (1024 * 1024);
#else
    outTotal = 0;
    outFree  = 0;
#endif
}
#include "../Config.h"
#include "../Brakeza.h"
#include "../Components/Components.h"
#include "../GUI/Objects/FileSystemGUI.h"
#include "../Misc/Logging.h"
#include "../OpenGL/Base/ShaderBaseCustom.h"
#include "../OpenGL/Base/ShaderBaseOpenGL.h"
#include "../OpenGL/Code/ShaderOGLCustomCodePostprocessing.h"
#include "../OpenGL/Nodes/ShaderNodesPostProcessing.h"
#include "../Cache/ImageCache.h"
#include "../Cache/ModelDataCache.h"
#include "../Cache/AnimationDataCache.h"
#include "../Cache/ScriptDataCache.h"
#include "../Cache/SceneCache.h"

Profiler *Profiler::instance = nullptr;

Profiler *Profiler::get()
{
    if (instance == nullptr) {
        instance = new Profiler();
    }

    return instance;
}



void Profiler::DrawPool(const std::string &title, ThreadPool &pool)
{
    struct PoolDisplayState {
        float throughputHistory[120] = {};
        int   historyOffset = 0;
        int   lastCont      = -1;
    };
    static std::unordered_map<ThreadPool*, PoolDisplayState> states;
    auto& state = states[&pool];

    // Throughput delta por frame
    int currentCont = pool.getCont();
    if (state.lastCont >= 0) {
        state.throughputHistory[state.historyOffset] = (float)(currentCont - state.lastCont);
        state.historyOffset = (state.historyOffset + 1) % 120;
    }
    state.lastCont = currentCont;

    size_t numThreads = pool.getNumThreads();
    size_t pending    = pool.getPendingTasks();
    size_t callbacks  = pool.getPendingCallbacks();
    int    active     = pool.getActiveTasks();

    ImGui::PushID(title.c_str());

    ImVec4 headerColor = (pending + active > 0)
        ? ImVec4(0.2f, 0.4f, 0.8f, 1.0f)
        : ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Header, headerColor);
    bool isOpen = ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
    ImGui::PopStyleColor();

    if (!isOpen) { ImGui::PopID(); return; }

    ImGui::Indent();

    // ── Tabla de stats ────────────────────────────────────────────────────────
    if (ImGui::BeginTable("PoolStats", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("Metric", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Value",  ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        auto Row = [](const char* label) {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%s", label);
            ImGui::TableNextColumn();
        };

        Row("Active Tasks");
        ImGui::TextColored(active > 0 ? ImVec4(0.0f,1.0f,0.0f,1.0f) : ImVec4(0.5f,0.5f,0.5f,1.0f), "%d", active);

        Row("Pending Tasks");
        ImGui::Text("%zu", pending);

        Row("Pending Callbacks");
        ImGui::TextColored(callbacks > 0 ? ImVec4(1.0f,0.8f,0.0f,1.0f) : ImVec4(0.5f,0.5f,0.5f,1.0f), "%zu", callbacks);

        Row("Total Processed");
        ImGui::Text("%d", currentCont);

        Row("Worker Threads");
        ImGui::Text("%zu", numThreads);

        Row("Max Concurrent");
        ImGui::Text("%zu", pool.getMaxConcurrentTasks());

        Row("Max Enqueued");
        ImGui::Text("%zu", pool.getMaxEnqueuedTasks());

        ImGui::EndTable();
    }

    ImGui::Spacing();

    // ── Workload bar ──────────────────────────────────────────────────────────
    ImGui::Text("Workload:");
    size_t totalWork = pending + (size_t)active;
    if (totalWork > 0) {
        float ratio = (float)active / (float)totalWork;
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        ImGui::ProgressBar(ratio, ImVec2(-1, 18));
        ImGui::PopStyleColor();
        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("%d active / %zu total", active, totalWork);
    } else {
        ImGui::ProgressBar(0.0f, ImVec2(-1, 18), "IDLE");
    }

    ImGui::Spacing();

    // ── Thread utilization ────────────────────────────────────────────────────
    ImGui::Text("Thread Utilization:");
    float util = numThreads > 0 ? std::min((float)active / (float)numThreads, 1.0f) : 0.0f;
    ImVec4 utilColor = util >= 0.9f ? ImVec4(1.0f,0.0f,0.0f,1.0f)
                     : util >= 0.5f ? ImVec4(1.0f,1.0f,0.0f,1.0f)
                                    : ImVec4(0.0f,1.0f,0.0f,1.0f);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, utilColor);
    ImGui::ProgressBar(util, ImVec2(-1, 18));
    ImGui::PopStyleColor();
    ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
    ImGui::Text("%d/%zu threads (%.0f%%)", active, numThreads, util * 100.0f);

    ImGui::Spacing();

    // ── Throughput sparkline ──────────────────────────────────────────────────
    ImGui::Text("Tasks completed/frame (last 120):");
    ImGui::PlotLines("##throughput", state.throughputHistory, 120, state.historyOffset,
                     nullptr, 0.0f, 5.0f, ImVec2(-1, 40));

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ── Status badge ──────────────────────────────────────────────────────────
    ImGui::Text("Status:");
    ImGui::SameLine();
    if      (active > 0)    ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f), "● PROCESSING");
    else if (pending > 0)   ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "● QUEUED");
    else if (callbacks > 0) ImGui::TextColored(ImVec4(1.0f,0.5f,0.0f,1.0f), "● CALLBACKS PENDING");
    else                    ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1.0f), "● IDLE");

    ImGui::Spacing();

    if (ImGui::Button("Open Threads window"))
        Brakeza::get()->GUI()->getWindowStatus(GUIType::THREADS)->isOpen = true;

    ImGui::Unindent();
    ImGui::PopID();
}

void Profiler::DrawPools()
{
    DrawPool("Pool Compute", Brakeza::get()->PoolCompute());
    DrawPool("Pool images", Brakeza::get()->PoolImages());
}



void Profiler::DrawBreakDownComponent(Measure &pre, Measure &update, Measure &post, double total, float height)
{
    float preAvg    = AverageHistory(pre);
    float updateAvg = AverageHistory(update);
    float postAvg   = AverageHistory(post);

    float prePercent    = total > 0 ? (preAvg    / total) : 0.0f;
    float updatePercent = total > 0 ? (updateAvg / total) : 0.0f;
    float postPercent   = total > 0 ? (postAvg   / total) : 0.0f;

    ImVec2 pos = ImGui::GetCursorScreenPos();
    float fullWidth = ImGui::GetColumnWidth();
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    float preW    = fullWidth * prePercent;
    float updateW = fullWidth * updatePercent;
    float postW   = fullWidth * postPercent;

    struct Segment { float x; float w; ImU32 col; float pct; };
    Segment segs[] = {
        { pos.x,              preW,    IM_COL32(255, 50,  50,  255), prePercent    },
        { pos.x + preW,       updateW, IM_COL32(50,  200, 50,  255), updatePercent },
        { pos.x + preW + updateW, postW, IM_COL32(50, 120, 255, 255), postPercent  },
    };

    for (auto& s : segs) {
        if (s.w < 1.0f) continue;
        drawList->AddRectFilled(ImVec2(s.x, pos.y), ImVec2(s.x + s.w, pos.y + height), s.col);
        if (s.w > 28.0f) {
            char label[16];
            snprintf(label, sizeof(label), "%.0f%%", s.pct * 100.0f);
            drawList->AddText(ImVec2(s.x + 3.0f, pos.y + (height - ImGui::GetTextLineHeight()) * 0.5f),
                              IM_COL32(0, 0, 0, 230), label);
        }
    }

    ImGui::Dummy(ImVec2(fullWidth, height));

    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Pre: %.3f ms (%.1f%%)\nUpdate: %.3f ms (%.1f%%)\nPost: %.3f ms (%.1f%%)",
            preAvg,    prePercent    * 100.0f,
            updateAvg, updatePercent * 100.0f,
            postAvg,   postPercent   * 100.0f);
    }
}

void Profiler::DrawComponentsTable(float cellHeight)
{
    double totalTime = measureFrameTime.diffTime;

    if (ImGui::BeginTable("ComponentTimings", 9,
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Resizable
    )) {

        // Configurar columnas
        ImGui::TableSetupColumn("Component", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Pre", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Update", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Post", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Total", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("% (ms)", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Frame % time", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Breakdown", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Pre/Update/Post", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        const float ROW_HEIGHT = cellHeight;

        for (auto c : Components::get()->getComponents()) {
            auto measurePre = componentMeasures[c->getLabel() + ProfilerConstants::SUFFIX_PRE];
            auto measureUpdate = componentMeasures[c->getLabel() + ProfilerConstants::SUFFIX_UPDATE];
            auto measurePost = componentMeasures[c->getLabel() + ProfilerConstants::SUFFIX_POST];

            float preAvg    = AverageHistory(measurePre);
            float updateAvg = AverageHistory(measureUpdate);
            float postAvg   = AverageHistory(measurePost);

            double measureTotal = preAvg + updateAvg + postAvg;
            float frameAvg = AverageHistory(measureFrameTime);
            double percentage = frameAvg > 0 ? (measureTotal / frameAvg) * 100.0 : 0.0;

            // Determinar color según porcentaje
            ImVec4 color;
            if (percentage > 30.0) {
                color = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);  // Rojo
            } else if (percentage > 15.0) {
                color = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);  // Amarillo
            } else {
                color = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);  // Verde
            }

            ImGui::TableNextRow(ImGuiTableRowFlags_None, ROW_HEIGHT);

            float textHeight = ImGui::GetTextLineHeight();
            float spaceHeight, offsetY;

            // Columna: Nombre del componente
            ImGui::TableSetColumnIndex(0);
            spaceHeight = ROW_HEIGHT; // Usar la altura fija en lugar de GetContentRegionAvail()
            offsetY = (spaceHeight - textHeight) * 0.5f;
            if (offsetY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
            ImGui::Image(FileSystemGUI::Icon(c->isEnabled() ? IconGUI::CHECKED : IconGUI::UNCHECKED), GUIType::Sizes::ICONS_OBJECTS_ALLOWED);
            ImGui::SameLine();
            ImGui::TextColored(color, "%s", c->getLabel().c_str());

            // --- PRE (Rojo) ---
            ImGui::TableSetColumnIndex(1);
            spaceHeight = ROW_HEIGHT;
            offsetY = (spaceHeight - textHeight) * 0.5f;
            if (offsetY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "%.4f", preAvg);

            // --- UPDATE (Verde) ---
            ImGui::TableSetColumnIndex(2);
            spaceHeight = ROW_HEIGHT;
            offsetY = (spaceHeight - textHeight) * 0.5f;
            if (offsetY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "%.4f", updateAvg);

            // --- POST (Azul) ---
            ImGui::TableSetColumnIndex(3);
            spaceHeight = ROW_HEIGHT;
            offsetY = (spaceHeight - textHeight) * 0.5f;
            if (offsetY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
            ImGui::TextColored(ImVec4(0.2f, 0.5f, 1.0f, 1.0f), "%.4f", postAvg);

            // Columna: Total (ms)
            ImGui::TableSetColumnIndex(4);
            spaceHeight = ROW_HEIGHT;
            offsetY = (spaceHeight - textHeight) * 0.5f;
            if (offsetY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
            ImGui::Text("%.4f", measureTotal);

            // Columna: Porcentaje
            ImGui::TableSetColumnIndex(5);
            spaceHeight = ROW_HEIGHT;
            offsetY = (spaceHeight - textHeight) * 0.5f;
            if (offsetY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
            ImGui::Text("%.2f%%", percentage);

            // Columna: Barra de progreso
            ImGui::TableSetColumnIndex(6);
            spaceHeight = ROW_HEIGHT;
            float progressHeight = spaceHeight-6; // La altura que pusiste en ProgressBar
            offsetY = ((spaceHeight - progressHeight) * 0.5f) + 2;
            if (offsetY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
            ImGui::ProgressBar(percentage / 100.0f, ImVec2(-1, progressHeight));
            ImGui::PopStyleColor();

            // breakdown
            ImGui::TableSetColumnIndex(7);
            DrawBreakDownComponent(measurePre, measureUpdate, measurePost, measureTotal, ROW_HEIGHT);

            // plots
            ImGui::TableSetColumnIndex(8);
            DrawPlotComponent(c, ROW_HEIGHT);
        }
        ImGui::EndTable();
    }
}

void Profiler::DrawImagesTable() const
{
    int count = getNumberOfImages();
    float memKB = getMemoryImageUsageKB();
    ImGui::Text("Cached images: %d  (%.1f KB)", count, memKB);

    if (count == 0) return;

    if (ImGui::BeginTable("ImagesTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        imageCache.visit([&](const FilePath::ImageFile& path, Image* img) {
            if (!img || !img->isLoaded()) return;

            ImGui::TableNextColumn();

            ImVec2 imageSize = {100, 100};
            float columnWidth = ImGui::GetColumnWidth();
            float offset = (columnWidth - imageSize.x) * 0.5f;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
            ImGui::Image(img->getOGLImTexture(), imageSize);

            const char* filename = img->getFileName().c_str();
            float textWidth = ImGui::CalcTextSize(filename).x;
            float textOffset = (columnWidth - textWidth) * 0.5f;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
            ImGui::Text("%s", filename);
        });

        ImGui::EndTable();
    }
}

void Profiler::DrawCachesTable() const
{
    int imageCount = getNumberOfImages();
    float imageMemKB = getMemoryImageUsageKB();
    auto modelStats = modelDataCache.getStats();
    auto animStats = animationDataCache.getStats();

    if (ImGui::BeginTable("CacheSummary", 5,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("Cache");
        ImGui::TableSetupColumn("Entries");
        ImGui::TableSetupColumn("Hits");
        ImGui::TableSetupColumn("Misses");
        ImGui::TableSetupColumn("Memory / Detail");
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("  Images");
        ImGui::TableNextColumn(); ImGui::Text("%d", imageCount);
        ImGui::TableNextColumn(); ImGui::Text("%zu", imageCache.getHits());
        ImGui::TableNextColumn(); ImGui::Text("%zu", imageCache.getMisses());
        ImGui::TableNextColumn(); ImGui::Text("%.1f KB", imageMemKB);

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("  Models");
        ImGui::TableNextColumn(); ImGui::Text("%zu", modelDataCache.size());
        ImGui::TableNextColumn(); ImGui::Text("%zu", modelDataCache.getHits());
        ImGui::TableNextColumn(); ImGui::Text("%zu", modelDataCache.getMisses());
        ImGui::TableNextColumn(); ImGui::Text("%zu m / %zu t", modelStats.totalMeshes, modelStats.totalTriangles);

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("  Animations");
        ImGui::TableNextColumn(); ImGui::Text("%zu", animationDataCache.size());
        ImGui::TableNextColumn(); ImGui::Text("%zu", animationDataCache.getHits());
        ImGui::TableNextColumn(); ImGui::Text("%zu", animationDataCache.getMisses());
        ImGui::TableNextColumn(); ImGui::Text("%zu m / %zu t", animStats.totalMeshes, animStats.totalTriangles);

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("  Scripts");
        ImGui::TableNextColumn(); ImGui::Text("%zu", scriptDataCache.size());
        ImGui::TableNextColumn(); ImGui::Text("%zu", scriptDataCache.getHits());
        ImGui::TableNextColumn(); ImGui::Text("%zu", scriptDataCache.getMisses());
        ImGui::TableNextColumn(); ImGui::Text("");

        ImGui::TableNextRow();
        ImGui::TableNextColumn(); ImGui::Text("  Scenes (Assimp)");
        ImGui::TableNextColumn(); ImGui::Text("%zu", sceneCache.size());
        ImGui::TableNextColumn(); ImGui::Text("-");
        ImGui::TableNextColumn(); ImGui::Text("-");
        ImGui::TableNextColumn(); ImGui::Text("shared importers alive");

        ImGui::EndTable();
    }

    if (ImGui::SmallButton("Reset cache stats")) {
        imageCache.resetStats();
        modelDataCache.resetStats();
        animationDataCache.resetStats();
        scriptDataCache.resetStats();
    }

    ImGui::SameLine();
    if (ImGui::SmallButton("Clear Images")) {
        imageCache.clear();
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Clear Meshes")) {
        modelDataCache.clear();
        animationDataCache.clear();
    }
    ImGui::SameLine();
    if (ImGui::SmallButton("Clear Scripts")) {
        scriptDataCache.clear();
    }

    ImGui::Separator();

    // --- Scrollable image grid ---
    if (ImGui::CollapsingHeader("Images##cache", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Spacing();
        ImGui::BeginChild("ImagesScroll", ImVec2(0, 220), false, ImGuiWindowFlags_HorizontalScrollbar);
        {
            if (ImGui::BeginTable("ImagesGrid", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
                imageCache.visit([&](const FilePath::ImageFile& path, Image* img) {
                    if (!img || !img->isLoaded()) return;

                    ImGui::TableNextColumn();

                    ImVec2 imageSize = {100, 100};
                    float columnWidth = ImGui::GetColumnWidth();
                    float offset = (columnWidth - imageSize.x) * 0.5f;

                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
                    ImGui::Image(img->getOGLImTexture(), imageSize);

                    const char* filename = img->getFileName().c_str();
                    float textWidth = ImGui::CalcTextSize(filename).x;
                    float textOffset = (columnWidth - textWidth) * 0.5f;
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
                    ImGui::Text("%s", filename);

                    size_t hits = imageCache.getHitsForKey(path);
                    ImVec4 hitsCol = hits > 100 ? ImVec4(1.0f,0.4f,0.4f,1.0f)
                                   : hits > 10  ? ImVec4(1.0f,0.8f,0.2f,1.0f)
                                                : ImVec4(0.5f,0.9f,0.5f,1.0f);
                    char hitsBuf[32];
                    snprintf(hitsBuf, sizeof(hitsBuf), "hits: %zu", hits);
                    float hitsW = ImGui::CalcTextSize(hitsBuf).x;
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (columnWidth - hitsW) * 0.5f);
                    ImGui::TextColored(hitsCol, "%s", hitsBuf);
                });

                ImGui::EndTable();
            }
        }
        ImGui::EndChild();
        ImGui::Spacing();
    }

    // --- Cached models detail ---
    if (ImGui::CollapsingHeader("Models##cache", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Spacing();
        ImGui::BeginChild("ModelsScroll", ImVec2(0, 200), false, ImGuiWindowFlags_HorizontalScrollbar);
        {
            if (ImGui::BeginTable("ModelsTable", 5,
                ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable))
            {
                ImGui::TableSetupColumn("Model");
                ImGui::TableSetupColumn("Meshes");
                ImGui::TableSetupColumn("Triangles");
                ImGui::TableSetupColumn("Vertices");
                ImGui::TableSetupColumn("Materials");
                ImGui::TableHeadersRow();

                modelDataCache.visit([&](const FilePath::ModelFile& path, const ModelData& data) {
                    size_t totalTris = 0, totalVerts = 0;
                    for (const auto& m : data.meshes) {
                        totalTris += m.triangleVertices.size() / 3;
                        totalVerts += m.vertices.size();
                    }

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn(); ImGui::Text("%s", path.c_str());
                    ImGui::TableNextColumn(); ImGui::Text("%zu", data.meshes.size());
                    ImGui::TableNextColumn(); ImGui::Text("%zu", totalTris);
                    ImGui::TableNextColumn(); ImGui::Text("%zu", totalVerts);
                    ImGui::TableNextColumn(); ImGui::Text("%zu", data.materials.size());
                });

                ImGui::EndTable();
            }

            if (animationDataCache.size() > 0) {
                ImGui::Spacing();
                ImGui::Separator();
                ImGui::Text("Animated models");
                ImGui::Spacing();

                if (ImGui::BeginTable("AnimModelsTable", 5,
                    ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable))
                {
                    ImGui::TableSetupColumn("Model");
                    ImGui::TableSetupColumn("Meshes");
                    ImGui::TableSetupColumn("Triangles");
                    ImGui::TableSetupColumn("Vertices");
                    ImGui::TableSetupColumn("Materials");
                    ImGui::TableHeadersRow();

                    animationDataCache.visit([&](const FilePath::ModelFile& path, const AnimationData& data) {
                        size_t totalTris = 0, totalVerts = 0;
                        for (const auto& m : data.meshes) {
                            totalTris += m.triangleVertices.size() / 3;
                            totalVerts += m.vertices.size();
                        }

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn(); ImGui::Text("%s", path.c_str());
                        ImGui::TableNextColumn(); ImGui::Text("%zu", data.meshes.size());
                        ImGui::TableNextColumn(); ImGui::Text("%zu", totalTris);
                        ImGui::TableNextColumn(); ImGui::Text("%zu", totalVerts);
                        ImGui::TableNextColumn(); ImGui::Text("—");
                    });

                    ImGui::EndTable();
                }
            }
        }
        ImGui::EndChild();
    }
}

void Profiler::DrawPlotComponent(Component *c, float height)
{
    // Acceso directo a las medidas del historial
    auto& measurePre    = componentMeasures[c->getLabel() + ProfilerConstants::SUFFIX_PRE];
    auto& measureUpdate = componentMeasures[c->getLabel() + ProfilerConstants::SUFFIX_UPDATE];
    auto& measurePost   = componentMeasures[c->getLabel() + ProfilerConstants::SUFFIX_POST];

    // Actualizar historiales
    UpdateHistory(measurePre);
    UpdateHistory(measureUpdate);
    UpdateHistory(measurePost);

    float sizeH = height;
    std::string tableId = "ComponentPlots##" + c->getLabel();
    if (ImGui::BeginTable(tableId.c_str(), 3, ImGuiTableFlags_SizingStretchSame)) {
        ImGui::TableNextRow();
        // --------------------------------------------
        // Columna 0: Plot PRE (rojo)
        // --------------------------------------------
        ImGui::TableSetColumnIndex(0);
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        ImGui::PlotLines("",
                         measurePre.frameTimeHistory.data(),
                         (int)measurePre.frameTimeHistory.size(),
                         0,
                         nullptr,
                         0.0f,
                         5.0f,
                         ImVec2(-1, sizeH));
        ImGui::PopStyleColor();

        // --------------------------------------------
        // Columna 1: Plot UPDATE (verde)
        // --------------------------------------------
        ImGui::TableSetColumnIndex(1);
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
        ImGui::PlotLines("",
                         measureUpdate.frameTimeHistory.data(),
                         (int)measureUpdate.frameTimeHistory.size(),
                         0,
                         nullptr,
                         0.0f,
                         5.0f,
                         ImVec2(-1, sizeH));
        ImGui::PopStyleColor();

        // --------------------------------------------
        // Columna 2: Plot POST (azul)
        // --------------------------------------------
        ImGui::TableSetColumnIndex(2);
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.2f, 0.5f, 1.0f, 1.0f));
        ImGui::PlotLines("",
                         measurePost.frameTimeHistory.data(),
                         (int)measurePost.frameTimeHistory.size(),
                         0,
                         nullptr,
                         0.0f,
                         5.0f,
                         ImVec2(-1, sizeH));
        ImGui::PopStyleColor();

        ImGui::EndTable();
    }
}

void Profiler::DrawPlotFrameTime(Measure &measure)
{
    float avg = AverageHistory(measure);
    float fps = avg > 0.0f ? 1000.0f / avg : 0.0f;

    ImVec4 fpsColor = fps >= 55.0f ? ImVec4(0.2f, 0.9f, 0.3f, 1.0f)
                    : fps >= 30.0f ? ImVec4(0.9f, 0.8f, 0.1f, 1.0f)
                                   : ImVec4(0.9f, 0.2f, 0.2f, 1.0f);

    ImGui::TextColored(fpsColor, "%.1f FPS", fps);
    ImGui::SameLine();
    ImGui::TextDisabled("  %.3f ms/frame", avg);

    ImGui::PushStyleColor(ImGuiCol_PlotLines,       ImVec4(0.3f, 0.7f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_PlotLinesHovered,ImVec4(0.5f, 0.9f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg,         ImVec4(0.08f, 0.08f, 0.12f, 1.0f));
    ImGui::PlotLines("##ft",
                     measure.frameTimeHistory.data(),
                     (int)measure.frameTimeHistory.size(),
                     0, nullptr,
                     0.0f, 33.0f,
                     ImVec2(-1, 55));
    ImGui::PopStyleColor(3);
}

void Profiler::DrawFrameBuffers()
{
    auto* window = Components::get()->Window();
    if (!window) return;

    static float texScale = 2.5f;
    static GLuint zoomTex = 0;
    static std::string zoomLabel;

    const float BASE_W   = 96.0f;
    const float BASE_H   = 54.0f;
    const float THUMB_W  = BASE_W * texScale;
    const float THUMB_H  = BASE_H * texScale;
    const float CARD_PAD = 10.0f;

    ImGui::SetNextItemWidth(160.0f);
    ImGui::SliderFloat("Preview size", &texScale, 0.5f, 5.0f, "%.1fx");
    ImGui::SameLine();
    ImGui::TextDisabled("click to zoom");
    ImGui::Spacing();

    // Zoom popup
    if (ImGui::BeginPopup("##fb_texzoom")) {
        float maxW = ImGui::GetIO().DisplaySize.x * 0.85f;
        float maxH = ImGui::GetIO().DisplaySize.y * 0.85f;
        float w = maxW, h = w * 9.0f / 16.0f;
        if (h > maxH) { h = maxH; w = h * 16.0f / 9.0f; }
        ImGui::TextColored(ImVec4(0.8f,0.8f,0.3f,1.0f), "%s", zoomLabel.c_str());
        ImGui::TextDisabled("Tex #%u", zoomTex);
        ImGui::Image((ImTextureID)(intptr_t)zoomTex, ImVec2(w, h), ImVec2(0,1), ImVec2(1,0));
        ImGui::EndPopup();
    }

    // Horizontal texture card — label + optional sub + thumbnail
    auto TexCard = [&](GLuint tex, const char* label, const char* sub = nullptr) {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        ImGui::BeginGroup();
        ImGui::TextColored(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), "%s", label);
        if (sub) ImGui::TextDisabled("%s", sub);
        if (tex) {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::Image((ImTextureID)(intptr_t)tex, ImVec2(THUMB_W, THUMB_H), ImVec2(0,1), ImVec2(1,0));
            dl->AddRect(p, ImVec2(p.x + THUMB_W, p.y + THUMB_H), IM_COL32(180, 180, 180, 200), 0.0f, 0, 1.0f);
            if (ImGui::IsItemHovered()) ImGui::SetTooltip("Tex #%u\n%s", tex, label);
            if (ImGui::IsItemClicked()) { zoomTex = tex; zoomLabel = label; ImGui::OpenPopup("##fb_texzoom"); }
        } else {
            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::Dummy(ImVec2(THUMB_W, THUMB_H));
            dl->AddRect(p, ImVec2(p.x + THUMB_W, p.y + THUMB_H), IM_COL32(80, 80, 80, 180), 0.0f, 0, 1.0f);
            ImGui::TextDisabled("—");
        }
        ImGui::EndGroup();
        ImGui::SameLine(0.0f, CARD_PAD);
    };

    auto& buf = window->getGlobalBuffers();
    auto& gb  = window->getGBuffer();
    auto& pb  = window->getPickingColorFramebuffer();

    // ── Layer Buffers ──────────────────────────────────
    ImGui::SeparatorText("Layer Buffers  (composed → window)");
    ImGui::Spacing();

    auto fboSub = [](GLuint fbo) -> std::string {
        return fbo ? "FBO " + std::to_string(fbo) : "";
    };
    TexCard(buf.globalTexture,     "Global",     fboSub(buf.globalFBO).c_str());
    TexCard(buf.sceneTexture,      "Scene",      fboSub(buf.sceneFBO).c_str());
    TexCard(buf.backgroundTexture, "Background", fboSub(buf.backgroundFBO).c_str());
    TexCard(buf.foregroundTexture, "Foreground", fboSub(buf.foregroundFBO).c_str());
    TexCard(buf.uiTexture,         "UI",         fboSub(buf.uiFBO).c_str());
    ImGui::NewLine();

    // ── GBuffer ────────────────────────────────────────
    ImGui::Spacing();
    ImGui::SeparatorText("GBuffer  (deferred rendering)");
    ImGui::Spacing();

    TexCard(gb.positions, "Positions");
    TexCard(gb.normals,   "Normals");
    TexCard(gb.albedo,    "Albedo");

    // Depth — no displayable, show placeholder
    ImGui::BeginGroup();
    ImGui::TextColored(ImVec4(0.85f,0.85f,0.85f,1.0f), "Depth");
    if (gb.depth) ImGui::TextDisabled("Tex #%u", gb.depth); else ImGui::TextDisabled("—");
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f,0.08f,0.1f,1.0f));
    ImGui::Dummy(ImVec2(THUMB_W, THUMB_H));
    ImGui::PopStyleColor();
    if (ImGui::IsItemHovered() && gb.depth)
        ImGui::SetTooltip("Depth texture — not displayable as RGBA");
    ImGui::TextDisabled("(depth)");
    ImGui::EndGroup();
    ImGui::SameLine(0.0f, CARD_PAD);
    ImGui::NewLine();

    // ── Picking Buffer ─────────────────────────────────
    ImGui::Spacing();
    ImGui::SeparatorText("Picking Buffer");
    ImGui::Spacing();

    TexCard(pb.rbgTexture, "Color ID", fboSub(pb.FBO).c_str());
    ImGui::NewLine();
}

void Profiler::DrawOpenGLStatus()
{
    auto* render = Components::get()->Render();
    if (!render) return;

    // ── GL Caps ──
    ImGui::SeparatorText("Capabilities");
    if (ImGui::BeginTable("gl_caps", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Value",    ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        GLint val;
        auto Row = [&](const char* label, auto fmt) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(label);
            ImGui::TableSetColumnIndex(1); fmt();
        };

        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &val);
        Row("GL_MAX_TEXTURE_SIZE", [&]{ ImGui::Text("%d", val); });

        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &val);
        Row("GL_MAX_COLOR_ATTACHMENTS", [&]{ ImGui::Text("%d", val); });

        glGetIntegerv(GL_MAX_DRAW_BUFFERS, &val);
        Row("GL_MAX_DRAW_BUFFERS", [&]{ ImGui::Text("%d", val); });

        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &val);
        Row("GL_MAX_VERTEX_ATTRIBS", [&]{ ImGui::Text("%d", val); });

        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &val);
        Row("GL_MAX_TEXTURE_IMAGE_UNITS", [&]{ ImGui::Text("%d", val); });

        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &val);
        Row("GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", [&]{ ImGui::Text("%d", val); });

        glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &val);
        Row("GL_MAX_UNIFORM_LOCATIONS", [&]{ ImGui::Text("%d", val); });

        Row("GL_VENDOR",   [&]{ ImGui::TextUnformatted((const char*)glGetString(GL_VENDOR)); });
        Row("GL_RENDERER", [&]{ ImGui::TextUnformatted((const char*)glGetString(GL_RENDERER)); });
        Row("GL_VERSION",  [&]{ ImGui::TextUnformatted((const char*)glGetString(GL_VERSION)); });

        glGetIntegerv(GL_MAJOR_VERSION, &val);
        Row("GL_MAJOR_VERSION", [&]{ ImGui::Text("%d", val); });
        glGetIntegerv(GL_MINOR_VERSION, &val);
        Row("GL_MINOR_VERSION", [&]{ ImGui::Text("%d", val); });

        ImGui::EndTable();
    }

    // ── Context switches (per frame) ──
    ImGui::SeparatorText("Context Switches (this frame)");
    ImGui::Checkbox("Count FBO switches",     &countFboSwitches);
    ImGui::Checkbox("Count Program switches", &countProgramSwitches);
    if (ImGui::BeginTable("gl_ctx_switches", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Type",  ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Count", ImGuiTableColumnFlags_WidthFixed, 70.0f);
        ImGui::TableHeadersRow();

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("FBO binds");
        ImGui::TableSetColumnIndex(1); ImGui::Text("%d", lastFboChanges);

        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Program binds");
        ImGui::TableSetColumnIndex(1); ImGui::Text("%d", lastProgramChanges);

        ImGui::EndTable();
    }

    // ── Scene Shaders (custom) ──
    ImGui::SeparatorText("Scene Shaders  (loaded in project)");
    auto& sceneShaders = render->getSceneShaders();
    if (sceneShaders.empty()) {
        ImGui::TextDisabled("No custom scene shaders loaded.");
    } else {
        if (ImGui::BeginTable("gl_scene_shaders", 6,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Resizable))
        {
            ImGui::TableSetupColumn("Label",    ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Program",  ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Type",     ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Enabled",  ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("Class",    ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("InternalTex", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableHeadersRow();

            for (auto* s : sceneShaders) {
                if (!s) continue;
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(s->getLabel().c_str());
                ImGui::TableSetColumnIndex(1);
                if (s->getProgramID()) ImGui::Text("%u", s->getProgramID()); else ImGui::TextDisabled("—");
                ImGui::TableSetColumnIndex(2); ImGui::TextUnformatted(ShaderBaseCustom::getShaderTypeString(s->getType()).c_str());
                ImGui::TableSetColumnIndex(3);
                if (s->isEnabled()) ImGui::TextColored(ImVec4(0,1,0,1), "yes"); else ImGui::TextColored(ImVec4(1,0,0,1), "no");

                ImGui::TableSetColumnIndex(4);
                if (dynamic_cast<ShaderOGLCustomCodePostprocessing*>(s))
                    ImGui::TextUnformatted("CodePostProc");
                else if (dynamic_cast<ShaderNodesPostProcessing*>(s))
                    ImGui::TextUnformatted("NodesPostProc");
                else
                    ImGui::TextUnformatted("Other");

                ImGui::TableSetColumnIndex(5);
                auto* codeShader = dynamic_cast<ShaderBaseCustomOGLCode*>(s);
                GLuint internalTex = codeShader ? codeShader->getInternalTexture() : 0;
                if (internalTex) ImGui::Text("%u", internalTex); else ImGui::TextDisabled("0");
            }
            ImGui::EndTable();
        }
    }

    // ── Built-in shader programs ──
    ImGui::SeparatorText("Built-in Programs");
    auto* shaders = render->getShaders();
    auto* window = Components::get()->Window();
    if (shaders && window) {
        auto progID = [](ShaderBaseOpenGL* p) -> GLuint {
            return p ? p->getProgramID() : 0u;
        };

        // Cache de FBOs del sistema
        GLuint sceneFBO      = window->getSceneFramebuffer();
        GLuint foregroundFBO = window->getForegroundFramebuffer();
        GLuint gbufferFBO    = window->getGBuffer().FBO;
        GLuint dirShadowFBO  = shaders->shaderShadowPass
                                  ? shaders->shaderShadowPass->getDirectionalLightDepthMapFBO() : 0;
        GLuint spotShadowFBO = shaders->shaderShadowPass
                                  ? shaders->shaderShadowPass->getSpotLightsDepthMapsFBO() : 0;

        // Lambda que devuelve el label del FBO asociado a cada shader
        auto fboLabel = [&](ShaderBaseOpenGL* ptr) -> std::string {
            if (!ptr) return "—";
            if (ptr == shaders->shaderOGLGBuffer) {
                return gbufferFBO ? "gbuffer FBO (" + std::to_string(gbufferFBO) + ")" : "—";
            }
            if (ptr == shaders->shaderOGLRender) {
                bool lights = Config::get()->ENABLE_LIGHTS;
                GLuint fbo = lights ? gbufferFBO : sceneFBO;
                if (!fbo) return "—";
                return (lights ? "gbufferFBO (" : "sceneFBO (") + std::to_string(fbo) + ")";
            }
            if (ptr == shaders->shaderOGLLightPass) {
                return sceneFBO ? "sceneFBO (" + std::to_string(sceneFBO) + ")" : "—";
            }
            if (ptr == shaders->shaderShadowPass) {
                std::string s;
                if (dirShadowFBO)  s += "dirLightFBO ("  + std::to_string(dirShadowFBO)  + ") ";
                if (spotShadowFBO) s += "spotLightsFBO (" + std::to_string(spotShadowFBO) + ")";
                return s.empty() ? "—" : s;
            }
            if (ptr == shaders->shaderOGLDepthMap) {
                return dirShadowFBO ? "dirLightFBO (" + std::to_string(dirShadowFBO) + ")" : "—";
            }
            if (ptr == shaders->shaderShadowPassDebugLight) {
                return foregroundFBO ? "foregroundFBO (" + std::to_string(foregroundFBO) + ")" : "—";
            }
            // Shaders que escriben al foregroundFBO
            if (ptr == shaders->shaderOGLOutline   ||
                ptr == shaders->shaderOGLLine3D     ||
                ptr == shaders->shaderGroundCircle  ||
                ptr == shaders->shaderGroundDecal   ||
                ptr == shaders->shaderAxisQuad      ||
                ptr == shaders->shaderOGLParticles  ||
                ptr == shaders->shaderOGLImage      ||
                ptr == shaders->shaderOGLColor      ||
                ptr == shaders->shaderOGLLine       ||
                ptr == shaders->shaderOGLWireframe  ||
                ptr == shaders->shaderOGLPoints) {
                return foregroundFBO ? "foregroundFBO (" + std::to_string(foregroundFBO) + ")" : "—";
            }
            // Shaders que escriben al sceneFBO
            if (ptr == shaders->shaderOGLGrid     ||
                ptr == shaders->shaderOGLShading) {
                return sceneFBO ? "sceneFBO (" + std::to_string(sceneFBO) + ")" : "—";
            }
            return "—";
        };

        struct BuiltinRow {
            const char* name;
            GLuint      prog;
            std::string fbo;
        };
        std::vector<BuiltinRow> rows;
        rows.reserve(23);
        auto R = [&](const char* n, ShaderBaseOpenGL* ptr) {
            rows.push_back({n, progID(ptr), fboLabel(ptr)});
        };
        R("RenderForward",   shaders->shaderOGLRender);
        R("Image",           shaders->shaderOGLImage);
        R("Line",            shaders->shaderOGLLine);
        R("Wireframe",       shaders->shaderOGLWireframe);
        R("Line3D",          shaders->shaderOGLLine3D);
        R("Shading",         shaders->shaderOGLShading);
        R("Points",          shaders->shaderOGLPoints);
        R("Outline",         shaders->shaderOGLOutline);
        R("Color",           shaders->shaderOGLColor);
        R("Particles",       shaders->shaderOGLParticles);
        R("DepthMap",        shaders->shaderOGLDepthMap);
        R("BonesTransforms", shaders->shaderOGLBonesTransforms);
        R("GBuffer",         shaders->shaderOGLGBuffer);
        R("LightPass",       shaders->shaderOGLLightPass);
        R("ShadowPass",      shaders->shaderShadowPass);
        R("ShadowDebug",     shaders->shaderShadowPassDebugLight);
        R("Grid",            shaders->shaderOGLGrid);
        R("GroundCircle",    shaders->shaderGroundCircle);
        R("GroundDecal",     shaders->shaderGroundDecal);
        R("AxisQuad",        shaders->shaderAxisQuad);

        if (ImGui::BeginTable("gl_builtin_progs", 3,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
        {
            ImGui::TableSetupColumn("Program", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("ID",      ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("FBO",     ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (auto& r : rows) {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(r.name);
                ImGui::TableSetColumnIndex(1);
                if (r.prog) ImGui::Text("%u", r.prog); else ImGui::TextDisabled("—");
                ImGui::TableSetColumnIndex(2); ImGui::TextUnformatted(r.fbo.c_str());
            }
            ImGui::EndTable();
        }
    }
}


void Profiler::DrawScriptDetail()
{
    bool active = scriptDetailEnabled;
    if (ImGui::Checkbox("Enable script timing", &active))
        setScriptDetailEnabled(active);

    if (!scriptDetailEnabled) {
        ImGui::SameLine();
        ImGui::TextDisabled("(off — zero overhead)");
        return;
    }

    if (scriptMeasures.empty()) {
        ImGui::TextDisabled("No data yet. Run scripts with timing enabled.");
        return;
    }

    struct ScriptEntry {
        std::string baseKey;
        std::string displayName;
        char typeTag;   // 'G'=project, 'S'=scene, 'O'=object
        float updateMs = 0.f;
        float postMs   = 0.f;
        float totalMs() const { return updateMs + postMs; }
    };

    std::unordered_map<std::string, ScriptEntry> entries;

    for (auto& [key, measure] : scriptMeasures) {
        bool isUpdate = key.size() > 7 && key.compare(key.size() - 7, 7, "_update") == 0;
        bool isPost   = key.size() > 5 && key.compare(key.size() - 5, 5, "_post")   == 0;
        if (!isUpdate && !isPost) continue;

        std::string base = key.substr(0, key.size() - (isUpdate ? 7 : 5));
        char tag = (base.size() > 1 && base[1] == 'G') ? 'G'
                 : (base.size() > 1 && base[1] == 'S') ? 'S'
                 : 'O';

        if (!entries.count(base)) {
            std::string dname = base.size() > 4 ? base.substr(4) : base;
            entries[base] = {base, dname, tag};
        }
        float ms = AverageHistory(measure);
        if (isUpdate) entries[base].updateMs = ms;
        else          entries[base].postMs   = ms;
    }

    if (entries.empty()) {
        ImGui::TextDisabled("No entries yet.");
        return;
    }

    std::vector<ScriptEntry*> globals, objects;
    for (auto& [k, e] : entries) {
        if (e.typeTag == 'O') objects.push_back(&e);
        else                  globals.push_back(&e);
    }
    auto byTime = [](const ScriptEntry* a, const ScriptEntry* b) { return a->totalMs() > b->totalMs(); };
    std::sort(globals.begin(), globals.end(), byTime);
    std::sort(objects.begin(), objects.end(), byTime);

    float totalMs = 0.f;
    for (auto& [k, e] : entries) totalMs += e.totalMs();

    // Stacked bar for global/scene scripts
    if (!globals.empty()) {
        ImDrawList* dl       = ImGui::GetWindowDrawList();
        ImVec2      barStart = ImGui::GetCursorScreenPos();
        float       barW     = ImGui::GetContentRegionAvail().x;
        const float BAR_H    = 16.f;
        ImU32 colors[] = {
            IM_COL32(130,200,100,220), IM_COL32(100,180,220,220),
            IM_COL32(220,170, 80,220), IM_COL32(200, 90, 90,220),
            IM_COL32(160,120,220,220), IM_COL32( 80,210,170,220),
        };
        float xOff = 0.f;
        int ci = 0;
        for (auto* e : globals) {
            float pct = totalMs > 0.f ? e->totalMs() / totalMs : 0.f;
            float w = barW * pct;
            if (w < 1.f) { ci++; continue; }
            dl->AddRectFilled(
                ImVec2(barStart.x + xOff,     barStart.y),
                ImVec2(barStart.x + xOff + w, barStart.y + BAR_H),
                colors[ci % 6]);
            if (w > 40.f) {
                char lbl[32]; snprintf(lbl, sizeof(lbl), "%.1f%%", pct * 100.f);
                dl->AddText(ImVec2(barStart.x + xOff + 3.f, barStart.y + 2.f),
                            IM_COL32(0,0,0,255), lbl);
            }
            xOff += w;
            ci++;
        }
        ImGui::Dummy(ImVec2(barW, BAR_H + 4.f));
        ImGui::Spacing();
    }

    // Helper: draw one section table
    auto DrawTable = [&](const char* tableId, const std::vector<ScriptEntry*>& list) {
        if (list.empty()) { ImGui::TextDisabled("  (none)"); return; }
        if (!ImGui::BeginTable(tableId, 6,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
            return;
        ImGui::TableSetupColumn("Script",       ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("T",            ImGuiTableColumnFlags_WidthFixed,   16.f);
        ImGui::TableSetupColumn("onUpdate ms",  ImGuiTableColumnFlags_WidthFixed,   80.f);
        ImGui::TableSetupColumn("postUpdate ms",ImGuiTableColumnFlags_WidthFixed,   80.f);
        ImGui::TableSetupColumn("Total ms",     ImGuiTableColumnFlags_WidthFixed,   80.f);
        ImGui::TableSetupColumn("History",      ImGuiTableColumnFlags_WidthFixed,  120.f);
        ImGui::TableHeadersRow();

        for (auto* e : list) {
            float total = e->totalMs();
            float pct   = totalMs > 0.f ? (total / totalMs) * 100.f : 0.f;
            ImVec4 col  = pct > 30.f ? ImVec4(1.f,0.3f,0.3f,1.f)
                        : pct > 15.f ? ImVec4(1.f,0.8f,0.2f,1.f)
                                     : ImVec4(0.6f,0.9f,0.6f,1.f);

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::TextColored(col, "%s", e->displayName.c_str());
            ImGui::TableSetColumnIndex(1);
            if      (e->typeTag == 'G') ImGui::TextDisabled("P");
            else if (e->typeTag == 'S') ImGui::TextDisabled("S");
            else                        ImGui::TextDisabled("O");
            ImGui::TableSetColumnIndex(2); ImGui::Text("%.3f", e->updateMs);
            ImGui::TableSetColumnIndex(3); ImGui::Text("%.3f", e->postMs);
            ImGui::TableSetColumnIndex(4); ImGui::TextColored(col, "%.3f", total);

            ImGui::TableSetColumnIndex(5);
            {
                auto it = scriptMeasures.find(e->baseKey + "_update");
                if (it != scriptMeasures.end()) {
                    UpdateHistory(it->second);
                    char pid[80]; snprintf(pid, sizeof(pid), "##sc%s", e->baseKey.c_str());
                    ImGui::PushStyleColor(ImGuiCol_PlotLines, col);
                    ImGui::PlotLines(pid,
                        it->second.frameTimeHistory.data(),
                        (int)it->second.frameTimeHistory.size(),
                        0, nullptr, 0.0f, 5.0f, ImVec2(-1, 22));
                    ImGui::PopStyleColor();
                }
                auto itPost = scriptMeasures.find(e->baseKey + "_post");
                if (itPost != scriptMeasures.end()) UpdateHistory(itPost->second);
            }
        }
        ImGui::EndTable();
    };

    ImGui::SeparatorText("Global / Scene Scripts  (P=project, S=scene)");
    DrawTable("##gs_tbl", globals);

    ImGui::Spacing();
    ImGui::SeparatorText("Object Scripts");
    DrawTable("##os_tbl", objects);
}

void Profiler::DrawWinProfiler()
{
    DrawPlotFrameTime(measureFrameTime);
    ImGui::Spacing();

    {
        static uint64_t totalMB = 0, freeMB = 0;
        static float    refreshTimer = 0.0f;
        refreshTimer += ImGui::GetIO().DeltaTime;
        if (refreshTimer >= 1.0f) {
            GetSystemMemoryMB(totalMB, freeMB);
            refreshTimer = 0.0f;
        }
        const uint64_t usedMB    = totalMB - freeMB;
        const float    usedRatio = totalMB > 0 ? static_cast<float>(usedMB) / static_cast<float>(totalMB) : 0.0f;

        ImVec4 barColor = usedRatio < 0.65f
            ? ImVec4(0.20f, 0.72f, 0.30f, 1.0f)
            : usedRatio < 0.85f
                ? ImVec4(0.90f, 0.70f, 0.10f, 1.0f)
                : ImVec4(0.85f, 0.18f, 0.18f, 1.0f);

        char label[64];
        snprintf(label, sizeof(label), "%llu / %llu MB  (%.0f%% used)",
                 (unsigned long long)usedMB,
                 (unsigned long long)totalMB,
                 usedRatio * 100.0f);

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, barColor);
        ImGui::ProgressBar(usedRatio, ImVec2(-1.0f, 0.0f), label);
        ImGui::PopStyleColor();
        ImGui::Spacing();
    }

    // Helper: styled collapsing header
    auto Section = [](const char* label, ImVec4 baseColor) -> bool {
        ImVec4 hov  = { baseColor.x + 0.08f, baseColor.y + 0.08f, baseColor.z + 0.08f, 1.0f };
        ImVec4 act  = { baseColor.x + 0.16f, baseColor.y + 0.16f, baseColor.z + 0.16f, 1.0f };
        ImGui::PushStyleColor(ImGuiCol_Header,        baseColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, hov);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive,  act);
        bool open = ImGui::CollapsingHeader(label);
        ImGui::PopStyleColor(3);
        return open;
    };

    if (Section("  Components", ImVec4(0.15f, 0.30f, 0.55f, 1.0f))) {
        ImGui::Spacing();
        DrawComponentsTable(25.0f);
        ImGui::Spacing();
        DrawFlameGraph();
        ImGui::Spacing();
    }

    if (Section("  Render Pipeline", ImVec4(0.20f, 0.18f, 0.45f, 1.0f))) {
        ImGui::Spacing();
        DrawRenderDetail();
        ImGui::Spacing();
    }

    if (Section("  Scripts", ImVec4(0.38f, 0.22f, 0.12f, 1.0f))) {
        ImGui::Spacing();
        DrawScriptDetail();
        ImGui::Spacing();
    }

    if (Section("  Thread Pools", ImVec4(0.18f, 0.42f, 0.22f, 1.0f))) {
        ImGui::Spacing();
        DrawPools();
        ImGui::Spacing();
    }

    if (Section("  Cache", ImVec4(0.12f, 0.46f, 0.30f, 1.0f))) {
        ImGui::Spacing();
        DrawCachesTable();
        ImGui::Spacing();
    }

    if (Section("  Frame Buffers", ImVec4(0.35f, 0.18f, 0.48f, 1.0f))) {
        ImGui::Spacing();
        DrawFrameBuffers();
        ImGui::Spacing();
    }

    if (Section("  Post-Processing Chain", ImVec4(0.10f, 0.38f, 0.42f, 1.0f))) {
        ImGui::Spacing();
        DrawPostProcessingChain();
        ImGui::Spacing();
    }

    if (Section("  Colliders", ImVec4(0.15f, 0.55f, 0.25f, 1.0f))) {
        ImGui::Spacing();
        DrawCollidersTable();
        ImGui::Spacing();
    }

    if (Section("  OpenGL Status", ImVec4(0.40f, 0.14f, 0.14f, 1.0f))) {
        ImGui::Spacing();
        DrawOpenGLStatus();
        ImGui::Spacing();
    }
}

void Profiler::DrawPostProcessingChain()
{
    auto* window = Components::get()->Window();
    if (!window) return;

    auto* ppm = window->getPostProcessingManager();
    if (!ppm) return;

    static float texScale = 1.0f;
    static GLuint zoomTex = 0;
    static std::string zoomLabel;

    ImGui::SetNextItemWidth(200.0f);
    ImGui::SliderFloat("Texture scale", &texScale, 0.25f, 4.0f, "%.2fx");
    ImGui::SameLine();
    ImGui::TextDisabled("(click any texture to zoom)");

    const float PREVIEW_W = 160.0f * texScale;
    const float PREVIEW_H =  90.0f * texScale;
    const float THUMB_W   =  56.0f * texScale;
    const float THUMB_H   =  32.0f * texScale;

    // Helper: muestra una imagen clicable; abre zoom popup al hacer click
    auto TexImage = [&](GLuint tex, float w, float h, const char* label) {
        if (!tex) { ImGui::TextDisabled("—"); return; }
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::Image((ImTextureID)(intptr_t)tex, ImVec2(w, h), ImVec2(0,1), ImVec2(1,0));
        ImGui::GetWindowDrawList()->AddRect(p, ImVec2(p.x + w, p.y + h), IM_COL32(180, 180, 180, 200), 0.0f, 0, 1.0f);
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("Click to zoom  |  Tex #%u", tex);
        if (ImGui::IsItemClicked()) { zoomTex = tex; zoomLabel = label; ImGui::OpenPopup("##texzoom"); }
    };

    // Popup de zoom — se dibuja una sola vez fuera de cualquier tabla
    if (ImGui::BeginPopup("##texzoom")) {
        float maxW = ImGui::GetIO().DisplaySize.x * 0.85f;
        float maxH = ImGui::GetIO().DisplaySize.y * 0.85f;
        // mantener proporción 16:9
        float w = maxW, h = w * 9.0f / 16.0f;
        if (h > maxH) { h = maxH; w = h * 16.0f / 9.0f; }
        ImGui::Text("%s  (Tex #%u)", zoomLabel.c_str(), zoomTex);
        ImGui::Image((ImTextureID)(intptr_t)zoomTex, ImVec2(w, h), ImVec2(0,1), ImVec2(1,0));
        ImGui::EndPopup();
    }

    int count = ppm->getShaderCount();

    if (count == 0) {
        ImGui::TextDisabled("(no shaders in chain)");
        return;
    }

    // ---- Calcular flujo ----
    struct FlowStep {
        int         shaderIndex;
        std::string shaderLabel;
        bool        shaderEnabled;
        GLuint      inputTex;
        std::string inputLabel;
        GLuint      outputTex;   // PING/PONG, o la misma input si disabled
        std::string outputLabel;
        bool        isBlit;
        FlowStep() : shaderIndex(-1), shaderEnabled(false), inputTex(0), outputTex(0), isBlit(false) {}
    };

    std::vector<FlowStep> flow;
    {
        GLuint curIn = ppm->GetSceneColorTexture();
        std::string curInLabel = "sceneColor";
        bool anyEnabled = false;

        for (int i = 0; i < count; i++) {
            auto* sh = ppm->getShader(i);

            FlowStep step;
            step.shaderIndex   = i;
            step.shaderLabel   = sh->getLabel();
            step.shaderEnabled = sh->isEnabled();
            step.inputTex      = curIn;
            step.inputLabel    = curInLabel;

            if (!sh->isEnabled()) {
                step.outputTex   = curIn;
                step.outputLabel = curInLabel + " (pass)";
            } else {
                anyEnabled = true;
                GLuint outTex = ppm->getShaderTexture(i);
                step.outputTex   = outTex;
                step.outputLabel = "Tex[" + std::to_string(i) + "] " + sh->getLabel();
                curIn            = outTex;
                curInLabel       = step.outputLabel;
            }
            flow.push_back(step);
        }

        if (anyEnabled) {
            FlowStep blit;
            blit.shaderLabel   = "renderTexture → globalFBO";
            blit.shaderEnabled = true;
            blit.shaderIndex   = -1;
            blit.inputTex      = curIn;
            blit.inputLabel    = curInLabel;
            blit.outputTex     = window->getGlobalTexture();
            blit.outputLabel   = "globalFBO";
            blit.isBlit        = true;
            flow.push_back(blit);
        }
    }

    // ---- Diagrama de flujo ----
    ImGui::SeparatorText("Chain flow");
    ImGui::TextDisabled("sceneColor / Tex[N] = texturas intermedias por shader. renderTexture copia la última textura a globalFBO.");
    ImGui::Spacing();

    if (ImGui::BeginTable("chain_flow", 6,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Input",  ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Tex IN", ImGuiTableColumnFlags_WidthFixed, THUMB_W);
        ImGui::TableSetupColumn("Shader", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("ms",     ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Tex OUT",ImGuiTableColumnFlags_WidthFixed, THUMB_W);
        ImGui::TableSetupColumn("Output", ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableHeadersRow();

        for (auto& s : flow) {
            ImGui::TableNextRow();

            // Input label
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s\n(#%u)", s.inputLabel.c_str(), s.inputTex);

            // Input thumbnail
            ImGui::TableSetColumnIndex(1);
            TexImage(s.inputTex, THUMB_W, THUMB_H, s.inputLabel.c_str());

            // Shader name (color: verde=enabled, rojo=error, azul=blit, gris=disabled)
            ImGui::TableSetColumnIndex(2);
            if (s.isBlit) {
                ImGui::TextColored(ImVec4(0.5f, 0.7f, 1.0f, 1.0f), "%s", s.shaderLabel.c_str());
            } else if (s.shaderEnabled) {
                ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "[%d] %s", s.shaderIndex, s.shaderLabel.c_str());
            } else {
                ImGui::TextDisabled("[%d] %s  (disabled)", s.shaderIndex, s.shaderLabel.c_str());
            }

            // Render time (unavailable with current PostProcessingManager)
            ImGui::TableSetColumnIndex(3);
            ImGui::TextDisabled("—");

            // Output thumbnail
            ImGui::TableSetColumnIndex(4);
            TexImage(s.outputTex, THUMB_W, THUMB_H, s.outputLabel.c_str());

            // Output label
            ImGui::TableSetColumnIndex(5);
            if (s.isBlit)
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.3f, 1.0f), "%s (blit)", s.outputLabel.c_str());
            else if (s.outputLabel == "globalFBO")
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.3f, 1.0f), "%s\n(#%u)", s.outputLabel.c_str(), s.outputTex);
            else
                ImGui::Text("%s\n(#%u)", s.outputLabel.c_str(), s.outputTex);
        }
        ImGui::EndTable();
    }


    // ---- FBOs brutos (IDs + previews) ----
    ImGui::SeparatorText("FBO raw info");
    if (ImGui::BeginTable("ppm_fbos", 5,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingFixedFit))
    {
        ImGui::TableSetupColumn("Buffer",  ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("FBO ID",  ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Tex ID",  ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Status",  ImGuiTableColumnFlags_WidthFixed);
        ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed, THUMB_W);
        ImGui::TableHeadersRow();

        // Scene-level textures
        struct FBORow { const char* name; GLuint fbo; GLuint tex; };
        FBORow fixedRows[] = {
            { "Scene Color", 0,                        ppm->GetSceneColorTexture() },
            { "Scene Depth", 0,                        ppm->GetSceneDepthTexture() },
        };
        auto drawFBORow = [&](const char* label, GLuint fbo, GLuint tex) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(label);
            ImGui::TableSetColumnIndex(1);
            if (fbo) ImGui::Text("%u", fbo); else ImGui::TextDisabled("—");
            ImGui::TableSetColumnIndex(2);
            if (tex) ImGui::Text("%u", tex); else ImGui::TextDisabled("—");
            ImGui::TableSetColumnIndex(3);
            if (fbo) {
                glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
                GLenum fbStatus = glCheckFramebufferStatus(GL_READ_FRAMEBUFFER);
                glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
                if (fbStatus == GL_FRAMEBUFFER_COMPLETE)
                    ImGui::TextColored(ImVec4(0.3f,1.0f,0.3f,1.0f), "COMPLETE");
                else
                    ImGui::TextColored(ImVec4(1.0f,0.3f,0.3f,1.0f), "ERR 0x%X", fbStatus);
            } else {
                ImGui::TextDisabled("(no FBO)");
            }
            ImGui::TableSetColumnIndex(4);
            TexImage(tex, THUMB_W, THUMB_H, label);
        };

        // Per-shader FBOs (dynamic)
        for (int i = 0; i < count; i++) {
            auto* sh = ppm->getShader(i);
            std::string label = "Tex[" + std::to_string(i) + "] " + sh->getLabel();
            GLuint tex = ppm->getShaderTexture(i);
            GLuint fbo = ppm->getShaderFBO(i);
            drawFBORow(label.c_str(), fbo, tex);
        }

        // Fixed scene textures
        for (auto& r : fixedRows) {
            drawFBORow(r.name, r.fbo, r.tex);
        }
        ImGui::EndTable();
    }

    // ---- Shaders de la chain (detalle) ----
    ImGui::SeparatorText("Shader detail");

    for (auto& s : flow) {
        if (s.isBlit) continue;

        auto* shader = ppm->getShader(s.shaderIndex);

        ImGui::PushID(s.shaderIndex);
        ImVec4 hdrColor = s.shaderEnabled
            ? ImVec4(0.20f, 0.55f, 0.20f, 1.0f)
            : ImVec4(0.40f, 0.40f, 0.40f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Header,        hdrColor);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(hdrColor.x+0.1f, hdrColor.y+0.1f, hdrColor.z+0.1f, 1.0f));

        char nodeLabel[128];
        snprintf(nodeLabel, sizeof(nodeLabel), "[%d] %s  (%s)",
                 s.shaderIndex, s.shaderLabel.c_str(),
                 ShaderBaseCustom::getShaderTypeString(shader->getType()).c_str());

        bool open = ImGui::CollapsingHeader(nodeLabel);
        ImGui::PopStyleColor(2);

        if (open) {
            if (ImGui::BeginTable("shader_meta", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit)) {
                ImGui::TableSetupColumn("Field", ImGuiTableColumnFlags_WidthFixed);
                ImGui::TableSetupColumn("Value");

                auto addRow = [](const char* field, const char* value) {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted(field);
                    ImGui::TableSetColumnIndex(1); ImGui::TextUnformatted(value);
                };

                char tmp[256];
                snprintf(tmp, sizeof(tmp), "%d", s.shaderIndex);
                addRow("Index",    tmp);
                addRow("Label",    s.shaderLabel.c_str());
                addRow("Type",     ShaderBaseCustom::getShaderTypeString(shader->getType()).c_str());
                addRow("Enabled",  s.shaderEnabled ? "YES" : "NO");

                if (s.shaderEnabled) {
                    addRow("GL error", "N/A");
                    addRow("Render time", "N/A");
                }

                addRow("Input",    s.inputLabel.c_str());
                addRow("Output",   s.outputLabel.c_str());

                // Program ID
                {
                    GLuint progId = 0;
                    if (auto* glSh = dynamic_cast<ShaderBaseOpenGL*>(shader))
                        progId = glSh->getProgramID();
                    if (progId == 0)
                        snprintf(tmp, sizeof(tmp), "0  *** NOT COMPILED ***");
                    else
                        snprintf(tmp, sizeof(tmp), "%u", progId);
                    addRow("Program ID", tmp);
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Input Tex ID (chain)");
                ImGui::TableSetColumnIndex(1);
                if (s.shaderEnabled) ImGui::Text("%u", s.inputTex); else ImGui::TextDisabled("(skipped)");

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Output texture");
                ImGui::TableSetColumnIndex(1);
                if (s.shaderEnabled) {
                    ImGui::Text("%u", s.outputTex);
                } else {
                    ImGui::TextDisabled("(disabled)");
                }

                // Input texture dimensions — save/restore binding to avoid disrupting engine state
                if (s.shaderEnabled && s.inputTex) {
                    GLint prevTex = 0, tw = 0, th = 0;
                    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTex);
                    glBindTexture(GL_TEXTURE_2D, s.inputTex);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &tw);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &th);
                    glBindTexture(GL_TEXTURE_2D, prevTex);
                    snprintf(tmp, sizeof(tmp), "%d x %d", tw, th);
                    addRow("Input Tex size", tmp);
                }

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::TextUnformatted("Output Tex ID");
                ImGui::TableSetColumnIndex(1);
                if (s.shaderEnabled && s.outputTex)
                    ImGui::Text("%u", s.outputTex);
                else if (s.shaderEnabled && !s.outputTex)
                    ImGui::TextColored(ImVec4(1,1,0,1), "window FBO");
                else
                    ImGui::TextDisabled("(skipped)");

                // Output texture dimensions
                if (s.shaderEnabled && s.outputTex) {
                    GLint prevTex = 0, tw = 0, th = 0;
                    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTex);
                    glBindTexture(GL_TEXTURE_2D, s.outputTex);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH,  &tw);
                    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &th);
                    glBindTexture(GL_TEXTURE_2D, prevTex);
                    snprintf(tmp, sizeof(tmp), "%d x %d", tw, th);
                    addRow("Output Tex size", tmp);
                }

                ImGui::EndTable();
            }

            if (s.shaderEnabled) {
                ImGui::Spacing();
                int previewCols = (s.outputTex != 0) ? 2 : 1;
                if (ImGui::BeginTable("tex_preview", previewCols,
                    ImGuiTableFlags_SizingFixedFit,
                    ImVec2(PREVIEW_W * previewCols + 16.0f * previewCols, 0))) {
                    ImGui::TableSetupColumn("col0", ImGuiTableColumnFlags_WidthFixed, PREVIEW_W);
                    if (previewCols == 2)
                        ImGui::TableSetupColumn("col1", ImGuiTableColumnFlags_WidthFixed, PREVIEW_W);
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Text("IN  %s (#%u)", s.inputLabel.c_str(), s.inputTex);
                    TexImage(s.inputTex, PREVIEW_W, PREVIEW_H, s.inputLabel.c_str());

                    if (s.outputTex) {
                        ImGui::TableSetColumnIndex(1);
                        ImGui::Text("OUT %s (#%u)", s.outputLabel.c_str(), s.outputTex);
                        TexImage(s.outputTex, PREVIEW_W, PREVIEW_H, s.outputLabel.c_str());
                    }
                    ImGui::EndTable();
                }
                if (!s.outputTex)
                    ImGui::TextColored(ImVec4(1,1,0,1), "OUT -> window FBO (no intermediate texture)");
            }
        }

        ImGui::PopID();
    }
}

void Profiler::DrawCollidersTable()
{
    auto* collisions = Components::get()->Collisions();
    if (!collisions || !collisions->isEnabled()) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Collision system disabled");
        return;
    }

    auto* world = collisions->getDynamicsWorld();
    if (!world) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Dynamics world not available");
        return;
    }

    // ── Aggregate stats ─────────────────────────────────────────────────────
    int total = 0, ghost = 0, body = 0, kinematic = 0;
    int simple = 0, capsule = 0, trimesh = 0;
    int groupCounts[7] = {0};

    for (auto& obj : Brakeza::get()->getSceneObjects()) {
        if (obj->isRemoved() || !obj->isEnabled() || !obj->isCollisionsEnabled()) continue;

        total++;
        switch (obj->getCollisionMode()) {
            case GHOST: ghost++; break;
            case BODY: body++; break;
            case KINEMATIC: kinematic++; break;
            default: break;
        }
        switch (obj->getCollisionShape()) {
            case SIMPLE_SHAPE: simple++; break;
            case CAPSULE_SHAPE: capsule++; break;
            case TRIANGLE_MESH_SHAPE: trimesh++; break;
            default: break;
        }
        int g = obj->getCollisionGroup();
        if (g & Config::Player)          groupCounts[0]++;
        if (g & Config::Enemy)           groupCounts[1]++;
        if (g & Config::Projectile)      groupCounts[2]++;
        if (g & Config::ProjectileEnemy) groupCounts[3]++;
        if (g & Config::Health)          groupCounts[4]++;
        if (g & Config::Weapon)          groupCounts[5]++;
        if (g & Config::StaticWorld)     groupCounts[6]++;
    }

    int numManifolds = world->getDispatcher()->getNumManifolds();

    ImGui::Text("Total %d", total); ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.2f,0.8f,0.2f,1), "  Ghost %d", ghost); ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.2f,0.5f,1.0f,1), "  Body %d", body); ImGui::SameLine();
    ImGui::TextColored(ImVec4(1.0f,0.8f,0.2f,1), "  Kinematic %d", kinematic); ImGui::SameLine();
    if (numManifolds > 0)
        ImGui::TextColored(ImVec4(1,0.6f,0,1), "  | Manifolds %d", numManifolds);
    else
        ImGui::TextDisabled("  | Manifolds %d", numManifolds);

    ImGui::Spacing();

    const char* groupLabels[] = {"Player", "Enemy", "Proj", "ProjEnemy", "Health", "Weapon", "StaticWorld"};
    ImGui::Text("Groups: ");
    ImGui::SameLine();
    for (int i = 0; i < 7; i++) {
        if (groupCounts[i] > 0) {
            ImGui::TextColored(ImVec4(0.7f,0.9f,0.7f,1), "%s:%d  ", groupLabels[i], groupCounts[i]);
            ImGui::SameLine();
        }
    }
    ImGui::TextDisabled("Shapes: Simple %d  Capsule %d  TriMesh %d", simple, capsule, trimesh);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ── Per-object table ────────────────────────────────────────────────────
    if (ImGui::BeginTable("colliders_table", 8,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_Sortable))
    {
        ImGui::TableSetupColumn("Name",     ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type",     ImGuiTableColumnFlags_WidthFixed, 70);
        ImGui::TableSetupColumn("Mode",     ImGuiTableColumnFlags_WidthFixed, 65);
        ImGui::TableSetupColumn("Shape",    ImGuiTableColumnFlags_WidthFixed, 55);
        ImGui::TableSetupColumn("Group",    ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableSetupColumn("Mask",     ImGuiTableColumnFlags_WidthFixed, 90);
        ImGui::TableSetupColumn("Enabled",  ImGuiTableColumnFlags_WidthFixed, 45);
        ImGui::TableSetupColumn("Overlaps", ImGuiTableColumnFlags_WidthFixed, 50);
        ImGui::TableHeadersRow();

        for (auto& obj : Brakeza::get()->getSceneObjects()) {
            if (obj->isRemoved() || !obj->isEnabled() || !obj->isCollisionsEnabled()) continue;

            ImGui::TableNextRow();

            // Name
            ImGui::TableSetColumnIndex(0);
            ImGui::Text("%s", obj->getName().c_str());

            // Type
            ImGui::TableSetColumnIndex(1);
            switch (obj->getTypeObject()) {
                case ObjectType::Mesh3D:           ImGui::Text("Mesh3D"); break;
                case ObjectType::Mesh3DAnimation:  ImGui::Text("Animated"); break;
                case ObjectType::LightPoint:       ImGui::Text("LightPt"); break;
                case ObjectType::LightSpot:        ImGui::Text("LightSp"); break;
                case ObjectType::ParticleEmitter:  ImGui::Text("Particles"); break;
                case ObjectType::Image3D:          ImGui::Text("Img3D"); break;
                case ObjectType::Image2D:          ImGui::Text("Img2D"); break;
                case ObjectType::Swarm:            ImGui::Text("Swarm"); break;
                default:                           ImGui::Text("Object3D"); break;
            }

            // Mode
            ImGui::TableSetColumnIndex(2);
            switch (obj->getCollisionMode()) {
                case GHOST:     ImGui::TextColored(ImVec4(0.2f,0.8f,0.2f,1), "Ghost"); break;
                case BODY:      ImGui::TextColored(ImVec4(0.2f,0.5f,1.0f,1), "Body"); break;
                case KINEMATIC: ImGui::TextColored(ImVec4(1.0f,0.8f,0.2f,1), "Kinematic"); break;
                default:        ImGui::TextDisabled("None"); break;
            }

            // Shape
            ImGui::TableSetColumnIndex(3);
            switch (obj->getCollisionShape()) {
                case SIMPLE_SHAPE:        ImGui::Text("Simple"); break;
                case CAPSULE_SHAPE:       ImGui::Text("Capsule"); break;
                case TRIANGLE_MESH_SHAPE: ImGui::Text("TriMesh"); break;
                default:                  ImGui::TextDisabled("—"); break;
            }

            // Group bits
            ImGui::TableSetColumnIndex(4);
            {
                int cg = obj->getCollisionGroup();
                std::string g;
                if (cg == -1) { g = "All"; }
                else {
                    if (cg & Config::Player)          g += "P ";
                    if (cg & Config::Enemy)           g += "E ";
                    if (cg & Config::Projectile)      g += "Proj ";
                    if (cg & Config::ProjectileEnemy) g += "ProjE ";
                    if (cg & Config::Health)          g += "Health ";
                    if (cg & Config::Weapon)          g += "Weapon ";
                    if (cg & Config::StaticWorld)     g += "StaticW ";
                    if (g.empty()) g = std::to_string(cg);
                }
                ImGui::Text("%s", g.c_str());
            }

            // Mask bits
            ImGui::TableSetColumnIndex(5);
            {
                int cm = obj->getCollisionMask();
                std::string m;
                if (cm == -1) { m = "All"; }
                else {
                    if (cm & Config::Player)          m += "P ";
                    if (cm & Config::Enemy)           m += "E ";
                    if (cm & Config::Projectile)      m += "Proj ";
                    if (cm & Config::ProjectileEnemy) m += "ProjE ";
                    if (cm & Config::Health)          m += "Health ";
                    if (cm & Config::Weapon)          m += "Weapon ";
                    if (cm & Config::StaticWorld)     m += "StaticW ";
                    if (m.empty()) m = std::to_string(cm);
                }
                ImGui::Text("%s", m.c_str());
            }

            // Enabled
            ImGui::TableSetColumnIndex(6);
            if (obj->isCollisionsEnabled())
                ImGui::TextColored(ImVec4(0,1,0,1), "Y");
            else
                ImGui::TextColored(ImVec4(1,0,0,1), "N");

            // Overlaps
            ImGui::TableSetColumnIndex(7);
            if (obj->getCollisionMode() == GHOST && obj->getGhostObject()) {
                int n = obj->getGhostObject()->getNumOverlappingObjects();
                if (n > 0)
                    ImGui::TextColored(ImVec4(1,0.8f,0,1), "%d", n);
                else
                    ImGui::Text("%d", n);
            } else {
                ImGui::TextDisabled("—");
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                Components::get()->Render()->setSelectedObject(obj);
            }
        }
        ImGui::EndTable();
    }

    ImGui::Spacing();

    if (ImGui::Button("Toggle Bullet Debug Wireframe")) {
        Config::get()->BULLET_DEBUG_MODE = !Config::get()->BULLET_DEBUG_MODE;
        collisions->setEnableDebugMode(Config::get()->BULLET_DEBUG_MODE);
    }
    ImGui::SameLine();
    ImGui::TextDisabled(Config::get()->BULLET_DEBUG_MODE ? "(ON)" : "(OFF)");
}

void Profiler::InitMeasure(MeasuresMap &map, const std::string &label)
{
    map[label].startTime = 0;
    map[label].endTime = 0;
    map[label].diffTime = 0;
}

void Profiler::ResetTotalFrameTime()
{
    measureFrameTime.startTime = Ticks();
    measureFrameTime.endTime = 0;
    // diffTime keeps the previous frame's value so the GUI (drawn early in the frame)
    // can display accurate timing before EndTotalFrameTime() runs.
    // Same pattern for switch counters: snapshot before reset so DrawOpenGLStatus reads last frame's data.
    lastFboChanges     = fboChanges;
    lastProgramChanges = programChanges;
    fboChanges     = 0;
    programChanges = 0;
}

void Profiler::incrementFboChanges()     { if (countFboSwitches) fboChanges++; }
void Profiler::incrementProgramChanges() { if (countProgramSwitches) programChanges++; }

void Profiler::EndTotalFrameTime()
{
    measureFrameTime.endTime = Ticks();
    measureFrameTime.diffTime = measureFrameTime.endTime - measureFrameTime.startTime;
    UpdateHistory(measureFrameTime);
}

MeasuresMap& Profiler::getComponentMeasures()
{
    return componentMeasures;
}

MeasuresMap& Profiler::getScriptMeasures()
{
    return scriptMeasures;
}

void Profiler::StartMeasure(MeasuresMap &map, const std::string &name)
{
    map[name].startTime = Ticks();
}

void Profiler::EndMeasure(MeasuresMap &map, const std::string &name)
{
    map[name].endTime = Ticks();
    map[name].diffTime = map[name].endTime - map[name].startTime;

}

int Profiler::getNumberOfImages() const
{
    return (int) imageCache.size();
}

int Profiler::getMemoryImageUsage() const
{
    return (int) imageCache.getMemoryUsage();
}

float Profiler::getMemoryImageUsageKB() const
{
    return getMemoryImageUsage() / 1024.f;
}

void Profiler::setEnabled(bool v)
{
    if (!v) {
        scriptMeasures.clear();
        scriptDetailEnabled = false;
    }
    enable = v;
}

bool Profiler::isEnabled() const
{
    return enable;
}

bool Profiler::isScriptDetailEnabled() const
{
    return scriptDetailEnabled;
}

void Profiler::setScriptDetailEnabled(bool v)
{
    if (!v) scriptMeasures.clear();
    scriptDetailEnabled = v;
}

double Profiler::Ticks()
{
    return (double)SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency();
}

void Profiler::DrawFlameGraph()
{
    float totalTime = AverageHistory(measureFrameTime);  // ms, averaged
    float barHeight = 30.0f;
    ImVec2 cursor = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    float barWidth = ImGui::GetContentRegionAvail().x;

    float xOffset = 0.0f;

    // Colores predefinidos
    ImU32 colors[] = {
        IM_COL32(255, 100, 100, 255),  // Rojo
        IM_COL32(100, 255, 100, 255),  // Verde
        IM_COL32(100, 100, 255, 255),  // Azul
        IM_COL32(255, 255, 100, 255),  // Amarillo
        IM_COL32(255, 100, 255, 255),  // Magenta
        IM_COL32(100, 255, 255, 255),  // Cyan
    };

    int colorIndex = 0;

    for (auto& [name, measure] : componentMeasures) {
        float ms = AverageHistory(measure);
        float percentage = totalTime > 0.0f ? (ms / totalTime) : 0.0f;
        float width = barWidth * percentage;

        // Dibujar rectángulo
        ImVec2 rectMin(cursor.x + xOffset, cursor.y);
        ImVec2 rectMax(cursor.x + xOffset + width, cursor.y + barHeight);

        drawList->AddRectFilled(rectMin, rectMax, colors[colorIndex % 6]);
        drawList->AddRect(rectMin, rectMax, IM_COL32(0, 0, 0, 255));

        // Texto si hay espacio
        if (width > 50) {
            char label[64];
            snprintf(label, sizeof(label), "%s\n%.2f%%", name.c_str(), percentage * 100.0f);
            drawList->AddText(ImVec2(rectMin.x + 5, rectMin.y + 5),
                            IM_COL32(0, 0, 0, 255), label);
        }

        xOffset += width;
        colorIndex++;
    }

    ImGui::Dummy(ImVec2(barWidth, barHeight + 5));
}

float Profiler::AverageHistory(const Measure &m)
{
    if (m.frameTimeHistory.empty()) return (float)(m.diffTime * 1000.0);
    float sum = 0.0f;
    for (float v : m.frameTimeHistory) sum += v;
    return sum / (float)m.frameTimeHistory.size();
}

void Profiler::UpdateHistory(Measure &measure)
{
    measure.frameTimeHistory.push_back(measure.diffTime * 1000.0f);  // En ms
    if ((int)measure.frameTimeHistory.size() > measure.MAX_HISTORY) {
        measure.frameTimeHistory.erase(measure.frameTimeHistory.begin());
    }
}

void Profiler::DrawRenderDetail()
{
    // Pipeline steps in execution order with display names and colors
    struct PipelineStep {
        const char* key;
        const char* label;
        ImVec4      color;
    };
    static const PipelineStep steps[] = {
        { "Scripts",                  "Scripts (Lua)",        { 0.95f, 0.75f, 0.20f, 1.0f } },
        { "GBuffer",                  "GBuffer (meshes)",     { 0.30f, 0.80f, 0.40f, 1.0f } },
        { "ShadowPass",               "Shadow Pass",          { 0.55f, 0.45f, 0.85f, 1.0f } },
        { "LightPass",                "Light Pass",           { 1.00f, 0.55f, 0.10f, 1.0f } },
        { "FlipBuffersToGlobal",      "Flip Buffers",         { 0.30f, 0.70f, 1.00f, 1.0f } },
        { "PostProcessingShadersChain","Post-Processing",     { 1.00f, 0.35f, 0.65f, 1.0f } },
    };
    constexpr int N = sizeof(steps) / sizeof(steps[0]);

    float frameAvg = AverageHistory(measureFrameTime);

    // ── Stacked bar ──────────────────────────────────────────────────────────
    ImDrawList* dl       = ImGui::GetWindowDrawList();
    ImVec2      barStart = ImGui::GetCursorScreenPos();
    float       barW     = ImGui::GetContentRegionAvail().x;
    const float BAR_H    = 18.0f;

    float xOff = 0.0f;
    for (int i = 0; i < N; i++) {
        auto it = componentMeasures.find(steps[i].key);
        if (it == componentMeasures.end()) continue;
        float ms  = AverageHistory(it->second);
        float pct = frameAvg > 0.0f ? (ms / frameAvg) : 0.0f;
        float w   = barW * pct;
        if (w < 1.0f) continue;
        ImVec4 c = steps[i].color;
        dl->AddRectFilled(
            ImVec2(barStart.x + xOff,     barStart.y),
            ImVec2(barStart.x + xOff + w, barStart.y + BAR_H),
            IM_COL32((int)(c.x*255), (int)(c.y*255), (int)(c.z*255), 220)
        );
        if (w > 38.0f) {
            char lbl[32];
            snprintf(lbl, sizeof(lbl), "%.1f%%", pct * 100.0f);
            dl->AddText(ImVec2(barStart.x + xOff + 3.0f, barStart.y + 2.0f),
                        IM_COL32(0,0,0,255), lbl);
        }
        xOff += w;
    }
    ImGui::Dummy(ImVec2(barW, BAR_H + 4.0f));
    ImGui::Spacing();

    // ── Table ────────────────────────────────────────────────────────────────
    if (ImGui::BeginTable("render_detail", 4,
        ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableSetupColumn("Step",      ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Avg (ms)",  ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("% frame",   ImGuiTableColumnFlags_WidthFixed, 70.0f);
        ImGui::TableSetupColumn("History",   ImGuiTableColumnFlags_WidthFixed, 120.0f);
        ImGui::TableHeadersRow();

        for (int i = 0; i < N; i++) {
            auto it = componentMeasures.find(steps[i].key);
            Measure dummy{};
            Measure& m = (it != componentMeasures.end()) ? it->second : dummy;
            UpdateHistory(m);

            float ms  = AverageHistory(m);
            float pct = frameAvg > 0.0f ? (ms / frameAvg) * 100.0f : 0.0f;

            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(steps[i].color, "  %s", steps[i].label);

            ImGui::TableSetColumnIndex(1);
            ImGui::Text("%.3f", ms);

            ImGui::TableSetColumnIndex(2);
            ImVec4 pctCol = pct > 30.0f ? ImVec4(1,0.2f,0.2f,1) : pct > 15.0f ? ImVec4(1,0.8f,0.2f,1) : ImVec4(0.6f,0.9f,0.6f,1);
            ImGui::TextColored(pctCol, "%.1f%%", pct);

            ImGui::TableSetColumnIndex(3);
            ImGui::PushStyleColor(ImGuiCol_PlotLines, steps[i].color);
            char pid[32]; snprintf(pid, sizeof(pid), "##rp%d", i);
            ImGui::PlotLines(pid, m.frameTimeHistory.data(), (int)m.frameTimeHistory.size(),
                             0, nullptr, 0.0f, 10.0f, ImVec2(-1, 22));
            ImGui::PopStyleColor();
        }
        ImGui::EndTable();
    }
}

void Profiler::DrawComponentsHierarchy()
{
    ImGui::Text("Measures amount: %d", (int)componentMeasures.size());
    ImGui::Separator();
    float frameAvg = AverageHistory(measureFrameTime);
    ImGui::Text("Total: %.3f ms", frameAvg);

    for (const auto& [name, measure] : componentMeasures) {
        if (ImGui::TreeNode(name.c_str())) {
            float ms = AverageHistory(measure);
            double percentage = frameAvg > 0 ? (ms / frameAvg) * 100.0 : 0.0;

            ImGui::Text("Time: %.3f ms", ms);
            ImGui::Text("Percentage: %.2f%%", percentage);
            ImGui::ProgressBar(percentage / 100.0f);

            ImGui::TreePop();
        }
    }
}