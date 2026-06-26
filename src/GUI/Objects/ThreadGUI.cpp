//
// Created by darkh on 19/01/2026.
//

#include "../../../include/GUI/Objects/ThreadGUI.h"
#include "../../../include/Brakeza.h"
#include <unordered_map>
#include <algorithm>
#include <thread>

static const int DEFAULT_THREADS    = (int)std::max(1u, std::thread::hardware_concurrency());
static const int DEFAULT_CALLBACKS  = 5;
static const int DEFAULT_CONCURRENT = 4;
static const int DEFAULT_ENQUEUED   = 1000;

void ThreadGUI::MenuWorkers()
{
    if (ImGui::BeginTabBar("WorkerPoolsTabs"))
    {
        if (ImGui::BeginTabItem("Pool Compute"))
        {
            MenuWorker(Brakeza::get()->PoolCompute(), "Pool Compute");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Pool Images"))
        {
            MenuWorker(Brakeza::get()->PoolImages(), "Pool Images");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void ThreadGUI::MenuWorker(ThreadPool &pool, std::string title)
{
    struct PoolState {
        int   targetThreads = DEFAULT_THREADS;
        int   maxCallbacks  = DEFAULT_CALLBACKS;
        int   maxConcurrent = DEFAULT_CONCURRENT;
        int   maxEnqueued   = DEFAULT_ENQUEUED;
        int   maxEnqueuedCallbacks = DEFAULT_ENQUEUED;
        float throughputHistory[120] = {};
        float throughputMax = 1.0f;
        int   historyOffset = 0;
        int   lastCont      = -1;
        bool  initialized   = false;
    };
    static std::unordered_map<ThreadPool*, PoolState> poolStates;
    auto& state = poolStates[&pool];

    if (!state.initialized) {
        state.targetThreads = (int)pool.getNumThreads();
        state.maxCallbacks  = (int)pool.getMaxCallbacksPerFrame();
        state.maxConcurrent = (int)pool.getMaxConcurrentTasks();
        state.maxEnqueued   = (int)pool.getMaxEnqueuedTasks();
        state.maxEnqueuedCallbacks = (int)pool.getMaxEnqueuedCallbacks();
        state.initialized   = true;
    }

    // Throughput delta — escala dinámica
    int currentCont = pool.getCont();
    if (state.lastCont >= 0) {
        float delta = (float)(currentCont - state.lastCont);
        state.throughputHistory[state.historyOffset] = delta;
        state.historyOffset = (state.historyOffset + 1) % 120;
        state.throughputMax = 1.0f;
        for (int i = 0; i < 120; i++)
            if (state.throughputHistory[i] > state.throughputMax)
                state.throughputMax = state.throughputHistory[i];
        state.throughputMax = std::max(state.throughputMax * 1.1f, 1.0f);
    }
    state.lastCont = currentCont;

    ImGui::PushID(title.c_str());

    int    active     = pool.getActiveTasks();
    size_t pending    = pool.getPendingTasks();
    size_t callbacks  = pool.getPendingCallbacks();
    size_t numThreads = pool.getNumThreads();

    // ── Título + badge ────────────────────────────────────────────────────────
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", title.c_str());
    ImGui::SameLine(0, 12);
    if      (active > 0)    ImGui::TextColored(ImVec4(0.0f,1.0f,0.0f,1.0f), "● PROCESSING");
    else if (pending > 0)   ImGui::TextColored(ImVec4(1.0f,1.0f,0.0f,1.0f), "● QUEUED");
    else if (callbacks > 0) ImGui::TextColored(ImVec4(1.0f,0.5f,0.0f,1.0f), "● CALLBACKS PENDING");
    else                    ImGui::TextColored(ImVec4(0.5f,0.5f,0.5f,1.0f), "● IDLE");
    ImGui::Separator();
    ImGui::Spacing();

    // ── Stats en tabla ────────────────────────────────────────────────────────
    if (ImGui::BeginTable("##stats", 4, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchSame)) {
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::TextDisabled("Active");
        ImGui::TextColored(active > 0 ? ImVec4(0.0f,1.0f,0.0f,1.0f) : ImVec4(0.5f,0.5f,0.5f,1.0f), "%d", active);

        ImGui::TableNextColumn();
        ImGui::TextDisabled("Pending");
        ImGui::Text("%zu", pending);

        ImGui::TableNextColumn();
        ImGui::TextDisabled("Callbacks");
        ImGui::TextColored(callbacks > 0 ? ImVec4(1.0f,0.8f,0.0f,1.0f) : ImVec4(0.5f,0.5f,0.5f,1.0f), "%zu", callbacks);

        ImGui::TableNextColumn();
        ImGui::TextDisabled("Processed");
        ImGui::Text("%d", currentCont);

        ImGui::EndTable();
    }
    ImGui::Spacing();

    // ── Barras ────────────────────────────────────────────────────────────────
    size_t totalWork = pending + (size_t)active;
    {
        float ratio = totalWork > 0 ? (float)active / (float)totalWork : 0.0f;
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f,1.0f,0.0f,1.0f));
        ImGui::ProgressBar(ratio, ImVec2(-1, 12), totalWork > 0 ? nullptr : "IDLE");
        ImGui::PopStyleColor();
    }
    {
        float util = numThreads > 0 ? std::min((float)active / (float)numThreads, 1.0f) : 0.0f;
        ImVec4 uc  = util >= 0.9f ? ImVec4(1.0f,0.0f,0.0f,1.0f)
                   : util >= 0.5f ? ImVec4(1.0f,1.0f,0.0f,1.0f)
                                  : ImVec4(0.0f,0.8f,1.0f,1.0f);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, uc);
        char overlay[32]; snprintf(overlay, sizeof(overlay), "%d/%zu threads (%.0f%%)", active, numThreads, util * 100.0f);
        ImGui::ProgressBar(util, ImVec2(-1, 12), overlay);
        ImGui::PopStyleColor();
    }
    ImGui::Spacing();

    // ── Throughput sparkline (escala dinámica) ────────────────────────────────
    ImGui::TextDisabled("Tasks/frame (120 frames):");
    ImGui::PlotLines("##tp", state.throughputHistory, 120, state.historyOffset,
                     nullptr, 0.0f, state.throughputMax, ImVec2(-1, 50));
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ── Worker threads ────────────────────────────────────────────────────────
    ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.0f, 1.0f), "Worker Threads");
    ImGui::Indent();
    ImGui::Text("Pool threads: %zu  (hardware concurrency: %d)", numThreads, DEFAULT_THREADS);
    ImGui::SetNextItemWidth(100);
    ImGui::DragInt("##numThreads", &state.targetThreads, 0.2f, 1, 32, "%d threads");
    ImGui::SameLine();
    if (ImGui::Button("Apply##resize")) {
        pool.resize((size_t)state.targetThreads);
        state.initialized = false;
    }
    ImGui::SameLine();
    ImGui::TextDisabled("(Ctrl+click for exact value)");
    ImGui::Unindent();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ── Configuration ─────────────────────────────────────────────────────────
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "Configuration");
    ImGui::Spacing();

    auto ConfigRow = [](const char* label, int& val, int vmin, int vmax, const char* fmt, const char* tip) -> bool {
        bool changed = false;
        ImGui::Text("%s", label);
        ImGui::SameLine(220);
        ImGui::SetNextItemWidth(160);
        char id[64]; snprintf(id, sizeof(id), "##%s", label);
        if (ImGui::DragInt(id, &val, 0.3f, vmin, vmax, fmt))
            changed = true;
        val = std::max(vmin, std::min(val, vmax));
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", tip);
        return changed;
    };

    if (ConfigRow("Max Callbacks/frame", state.maxCallbacks, 1, 100, "%d",
        "Main thread callbacks processed per frame.\nLow = smoother but higher latency."))
        pool.setMaxCallbacksPerFrame((size_t)state.maxCallbacks);

    ImGui::Spacing();

    if (ConfigRow("Max Concurrent Tasks", state.maxConcurrent, 1, 32, "%d",
        "Tasks executing simultaneously.\nShould be <= Worker Threads."))
        pool.setMaxConcurrentTasks((size_t)state.maxConcurrent);

    // Warning if maxConcurrent > numThreads
    if ((size_t)state.maxConcurrent > numThreads) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f,0.8f,0.0f,1.0f), "⚠");
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Max Concurrent (%d) > Worker Threads (%zu).\nExtra slots will never be used.", state.maxConcurrent, numThreads);
    }

    ImGui::Spacing();

    if (ConfigRow("Max Queue (enqueued)", state.maxEnqueued, 1, 5000, "%d",
        "Maximum queue depth. New tasks are dropped when full.\nIncrease if 'Queue full' appears in the log."))
        pool.setMaxEnqueuedTasks((size_t)state.maxEnqueued);

    ImGui::Spacing();

    if (ConfigRow("Max Callbacks Queue", state.maxEnqueuedCallbacks, 1, 5000, "%d",
        "Maximum callbacks queue depth. Callbacks are dropped when full.\nPrevents memory exhaustion when spawn rate > drain rate."))
        pool.setMaxEnqueuedCallbacks((size_t)state.maxEnqueuedCallbacks);

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ── Botones ───────────────────────────────────────────────────────────────
    if (ImGui::Button("Restore Defaults", ImVec2(170, 0))) {
        pool.setMaxCallbacksPerFrame((size_t)DEFAULT_CALLBACKS);
        pool.setMaxConcurrentTasks((size_t)DEFAULT_CONCURRENT);
        pool.setMaxEnqueuedTasks((size_t)DEFAULT_ENQUEUED);
        pool.setMaxEnqueuedCallbacks((size_t)DEFAULT_ENQUEUED);
        state.maxCallbacks  = DEFAULT_CALLBACKS;
        state.maxConcurrent = DEFAULT_CONCURRENT;
        state.maxEnqueued   = DEFAULT_ENQUEUED;
        state.maxEnqueuedCallbacks = DEFAULT_ENQUEUED;
        state.targetThreads = DEFAULT_THREADS;
    }
    ImGui::SameLine();

    bool busy = (active + (int)pending) > 0;
    if (!busy) ImGui::BeginDisabled();
    if (ImGui::Button("Wait for All", ImVec2(140, 0)))
        pool.waitAll();
    if (!busy) ImGui::EndDisabled();
    if (!busy && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        ImGui::SetTooltip("No active tasks.");

    ImGui::PopID();
}
