//
// Created by darkh on 19/01/2026.
//

#include "../../../include/GUI/Objects/ThreadGUI.h"

#include "../../../include/Brakeza.h"

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
    ImGui::PushID(title.c_str());

    // Título y separador
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", title.c_str());
    ImGui::Separator();
    ImGui::Spacing();

    // ===== INFORMACIÓN ACTUAL =====
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Estado Actual:");
    ImGui::Indent();

    ImGui::Text("Tareas Activas: %d", pool.getActiveTasks());
    ImGui::Text("Tareas Pendientes: %zu", pool.getPendingTasks());
    ImGui::Text("Callbacks Pendientes: %zu", pool.getPendingCallbacks());
    ImGui::Text("Contador Total: %d", pool.getCont());

    ImGui::Unindent();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // ===== CONFIGURACIÓN =====
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.5f, 1.0f), "Configuración:");
    ImGui::Spacing();

    // Max Callbacks Per Frame
    {
        static int maxCallbacks = static_cast<int>(pool.getMaxCallbacksPerFrame());
        ImGui::Text("Max Callbacks Por Frame:");
        ImGui::SetNextItemWidth(200);

        if (ImGui::SliderInt("##maxCallbacks", &maxCallbacks, 1, 100)) {
            pool.setMaxCallbacksPerFrame(static_cast<size_t>(maxCallbacks));
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Número máximo de callbacks que se procesan\ncada frame en el hilo principal");
        }

        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("##inputCallbacks", &maxCallbacks, 1, 10)) {
            maxCallbacks = std::max(1, maxCallbacks);
            pool.setMaxCallbacksPerFrame(static_cast<size_t>(maxCallbacks));
        }
        ImGui::Spacing();
    }

    // Max Concurrent Tasks
    {
        static int maxConcurrent = static_cast<int>(pool.getMaxConcurrentTasks());
        ImGui::Text("Max Tareas Concurrentes:");
        ImGui::SetNextItemWidth(200);

        if (ImGui::SliderInt("##maxConcurrent", &maxConcurrent, 1, 32)) {
            pool.setMaxConcurrentTasks(static_cast<size_t>(maxConcurrent));
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Número máximo de tareas que pueden\nejecutarse simultáneamente");
        }

        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("##inputConcurrent", &maxConcurrent, 1, 4)) {
            maxConcurrent = std::max(1, maxConcurrent);
            pool.setMaxConcurrentTasks(static_cast<size_t>(maxConcurrent));
        }
        ImGui::Spacing();
    }

    // Max Enqueued Tasks
    {
        static int maxEnqueued = static_cast<int>(pool.getMaxEnqueuedTasks());
        ImGui::Text("Max Tareas En Cola:");
        ImGui::SetNextItemWidth(200);

        if (ImGui::SliderInt("##maxEnqueued", &maxEnqueued, 1, 1000)) {
            pool.setMaxEnqueuedTasks(static_cast<size_t>(maxEnqueued));
        }
        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Número máximo de tareas que pueden\nestar esperando en la cola");
        }

        ImGui::SetNextItemWidth(100);
        if (ImGui::InputInt("##inputEnqueued", &maxEnqueued, 10, 100)) {
            maxEnqueued = std::max(1, maxEnqueued);
            pool.setMaxEnqueuedTasks(static_cast<size_t>(maxEnqueued));
        }
        ImGui::Spacing();
    }

    ImGui::Separator();
    ImGui::Spacing();

    // ===== BOTONES DE CONTROL =====
    if (ImGui::Button("Restaurar Valores por Defecto", ImVec2(250, 0))) {
        pool.setMaxCallbacksPerFrame(10);
        pool.setMaxConcurrentTasks(4);
        pool.setMaxEnqueuedTasks(100);
    }

    ImGui::SameLine();

    if (ImGui::Button("Esperar Todas las Tareas", ImVec2(200, 0))) {
        pool.waitAll();
    }

    ImGui::PopID();
}