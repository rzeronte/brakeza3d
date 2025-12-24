//
// Created by Eduardo on 29/11/2025.
//

#include "Profiler.h"
#include <algorithm>
#include "../Brakeza.h"
#include "../Components/Components.h"
#include "../Misc/Logging.h"

Profiler *Profiler::instance = nullptr;

Profiler *Profiler::get()
{
    if (instance == nullptr) {
        instance = new Profiler();
    }

    return instance;
}

void Profiler::AddImage(Image *image)
{
    if (image == nullptr || !image->isLoaded() || !isEnabled()) {
        return;
    }

    images.push_back(image);

    // Calcular memoria usada por la superficie SDL
    SDL_Surface* surface = image->getSurface();
    if (surface != nullptr) {
        // Bytes por píxel * ancho * alto
        int surfaceMemory = surface->format->BytesPerPixel * surface->w * surface->h;
        memoryImageUsage += surfaceMemory;
    }
    LOG_MESSAGE("[Profiler] Adding image '%s'", image->getFileName().c_str());
}

void Profiler::CaptureGUIMemoryUsage()
{
    numberOfGUIImages = (int) images.size();
    memoryOfGUIImages = memoryImageUsage;
    setEnabled(true);
}

void Profiler::DrawPool(const std::string &title, ThreadPool &pool)
{
    ImGui::PushID(title.c_str()); // ← ID único para esta pool

    size_t pending = pool.getPendingTasks();
    size_t callbacks = pool.getPendingCallbacks();
    int active = pool.getActiveTasks();
    int totalProcessed = pool.getCont();

    // === Header con color según estado ===
    ImVec4 headerColor = (pending + active > 0) ?
        ImVec4(0.2f, 0.4f, 0.8f, 1.0f) : ImVec4(0.2f, 0.2f, 0.2f, 1.0f);

    ImGui::PushStyleColor(ImGuiCol_Header, headerColor);
    bool isOpen = ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
    ImGui::PopStyleColor();

    if (isOpen) {
        ImGui::Indent();

        // Tabla de estadísticas
        if (ImGui::BeginTable("PoolStats", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
            ImGui::TableSetupColumn("Metric", ImGuiTableColumnFlags_WidthFixed, 150.0f);
            ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            // Active
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Active Tasks");
            ImGui::TableNextColumn();
            ImVec4 activeColor = active > 0 ? ImVec4(0.0f, 1.0f, 0.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
            ImGui::TextColored(activeColor, "%d", active);

            // Pending
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Pending Tasks");
            ImGui::TableNextColumn();
            ImGui::Text("%zu", pending);

            // Callbacks
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Pending Callbacks");
            ImGui::TableNextColumn();
            ImVec4 callbackColor = callbacks > 0 ? ImVec4(1.0f, 0.8f, 0.0f, 1.0f) : ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
            ImGui::TextColored(callbackColor, "%zu", callbacks);

            // Total procesado
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Total Processed");
            ImGui::TableNextColumn();
            ImGui::Text("%d", totalProcessed);

            ImGui::EndTable();
        }

        ImGui::Spacing();

        // === Workload bar (más precisa) ===
        ImGui::Text("Workload:");
        size_t totalWork = pending + active;
        if (totalWork > 0) {
            float activeRatio = (float)active / (float)totalWork;

            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            ImGui::ProgressBar(activeRatio, ImVec2(-1, 25));
            ImGui::PopStyleColor();

            ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
            ImGui::Text("%d active / %zu total", active, totalWork);
        } else {
            ImGui::ProgressBar(0.0f, ImVec2(-1, 25), "IDLE");
        }

        ImGui::Spacing();

        // === Thread utilization (simple y honesta) ===
        ImGui::Text("Thread Utilization:");
        const int numThreads = 4;
        float utilization = (float)active / (float)numThreads;
        utilization = std::min(utilization, 1.0f);

        ImVec4 utilColor;
        if (utilization >= 0.9f) utilColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        else if (utilization >= 0.5f) utilColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        else utilColor = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, utilColor);
        ImGui::ProgressBar(utilization, ImVec2(-1, 25));
        ImGui::PopStyleColor();

        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::Text("%d/%d threads (%.0f%%)", active, numThreads, utilization * 100.0f);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // === Status badge ===
        ImGui::Text("Status: ");
        ImGui::SameLine();
        if (active > 0) {
            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "● PROCESSING");
        } else if (pending > 0) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "● QUEUED");
        } else if (callbacks > 0) {
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "● CALLBACKS PENDING");
        } else {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "● IDLE");
        }

        ImGui::Spacing();

        // === Botones de control ===
        if (pending + active > 0) {
            if (ImGui::Button("Wait for All", ImVec2(120, 0))) {
                pool.waitAll();
            }
            ImGui::SameLine();
            ImGui::TextDisabled("(Blocks until complete)");
        }

        ImGui::Unindent();
    }

    ImGui::PopID(); // ← Cierra el ID único
}

void Profiler::DrawPools()
{
    DrawPool("Pool Compute", Brakeza::get()->PoolCompute());
    DrawPool("Pool images", Brakeza::get()->PoolImages());
    ImGui::PopStyleColor();
}

void Profiler::RemoveImage(const Image *image)
{
    if (image == nullptr || !isEnabled()) {
        return;
    }

    // Buscar y eliminar la imagen del vector
    auto it = std::find(images.begin(), images.end(), image);
    if (it != images.end()) {
        // Restar la memoria antes de eliminar
        SDL_Surface* surface = image->getSurface();
        if (surface != nullptr) {
            int surfaceMemory = surface->format->BytesPerPixel * surface->w * surface->h;
            memoryImageUsage -= surfaceMemory;
        }
        LOG_MESSAGE("[Profiler] Removing image '%s'", image->getFileName().c_str());

        images.erase(it);
    }
}

void Profiler::DrawBreakDownComponent(Measure &pre, Measure &update,Measure &post, double total, float height)
{
    float prePercent    = total > 0 ? (pre.diffTime    / total) : 0.0f;
    float updatePercent = total > 0 ? (update.diffTime / total) : 0.0f;
    float postPercent   = total > 0 ? (post.diffTime   / total) : 0.0f;

    ImVec2 pos = ImGui::GetCursorScreenPos();
    float fullWidth = ImGui::GetColumnWidth();

    ImDrawList* drawList = ImGui::GetWindowDrawList();

    float preW    = fullWidth * prePercent;
    float updateW = fullWidth * updatePercent;
    float postW   = fullWidth * postPercent;

    // Pre (rojo)
    drawList->AddRectFilled(
        ImVec2(pos.x, pos.y),
        ImVec2(pos.x + preW, pos.y + height),
        IM_COL32(255, 50, 50, 255)
    );

    // Update (verde)
    drawList->AddRectFilled(
        ImVec2(pos.x + preW, pos.y),
        ImVec2(pos.x + preW + updateW, pos.y + height),
        IM_COL32(50, 255, 50, 255)
    );

    // Post (azul)
    drawList->AddRectFilled(
        ImVec2(pos.x + preW + updateW, pos.y),
        ImVec2(pos.x + preW + updateW + postW, pos.y + height),
        IM_COL32(50, 120, 255, 255)
    );

    // Reservar espacio para que ImGui avance
    ImGui::Dummy(ImVec2(fullWidth, height));
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
        ImGui::TableSetupColumn("Component", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Pre", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Update", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Post", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Total", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("% (ms)", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Frame % time", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Breakdown", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Pre/Update/Post", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableHeadersRow();

        const float ROW_HEIGHT = cellHeight;

        for (auto c : Components::get()->getComponents()) {
            auto measurePre = componentMeasures[c->getLabel() + ProfilerConstants::SUFFIX_PRE];
            auto measureUpdate = componentMeasures[c->getLabel() + + ProfilerConstants::SUFFIX_UPDATE];
            auto measurePost = componentMeasures[c->getLabel() + + ProfilerConstants::SUFFIX_POST];

            // Calcular tiempo total del componente
            double measureTotal = measurePre.diffTime + measureUpdate.diffTime + measurePost.diffTime;
            double percentage = totalTime > 0 ? (measureTotal / totalTime) * 100.0 : 0.0;

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
            ImGui::TextColored(color, "%s", c->getLabel().c_str());

            // --- PRE (Rojo) ---
            ImGui::TableSetColumnIndex(1);
            spaceHeight = ROW_HEIGHT;
            offsetY = (spaceHeight - textHeight) * 0.5f;
            if (offsetY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "%.4f", measurePre.diffTime * 1000.0f);

            // --- UPDATE (Verde) ---
            ImGui::TableSetColumnIndex(2);
            spaceHeight = ROW_HEIGHT;
            offsetY = (spaceHeight - textHeight) * 0.5f;
            if (offsetY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "%.4f", measureUpdate.diffTime * 1000.0f);

            // --- POST (Azul) ---
            ImGui::TableSetColumnIndex(3);
            spaceHeight = ROW_HEIGHT;
            offsetY = (spaceHeight - textHeight) * 0.5f;
            if (offsetY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
            ImGui::TextColored(ImVec4(0.2f, 0.5f, 1.0f, 1.0f), "%.4f", measurePost.diffTime * 1000.0f);

            // Columna: Total (ms)
            ImGui::TableSetColumnIndex(4);
            spaceHeight = ROW_HEIGHT;
            offsetY = (spaceHeight - textHeight) * 0.5f;
            if (offsetY > 0.0f) ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offsetY);
            ImGui::Text("%.4f", measureTotal * 1000.0f);

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
    ImGui::Text("Size for %d images loaded:  %fKb", getNumberOfImages(), getMemoryImageUsageKB());

    if (ImGui::BeginTable("ImagesTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        for (size_t i = 0; i < images.size(); i++) {
            ImGui::TableNextColumn();

            // Centrar imagen y texto
            ImVec2 imageSize = {100, 100};
            float columnWidth = ImGui::GetColumnWidth();
            float offset = (columnWidth - imageSize.x) * 0.5f;

            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offset);
            ImGui::Image(images[i]->getOGLImTexture(), imageSize);

            // Si quieres centrar también el texto del nombre
            const char* filename = images[i]->getFileName().c_str();
            float textWidth = ImGui::CalcTextSize(filename).x;
            float textOffset = (columnWidth - textWidth) * 0.5f;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
            ImGui::Text("%s", filename);
        }

        ImGui::EndTable();
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

    float sizeH = height;;
    if (ImGui::BeginTable("ComponentPlots", 3, ImGuiTableFlags_SizingStretchSame )) {
        ImGui::TableNextRow();
        // --------------------------------------------
        // Columna 0: Plot PRE (rojo)
        // --------------------------------------------
        ImGui::TableSetColumnIndex(0);
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
        ImGui::PlotLines("",
                         measurePre.frameTimeHistory.data(),
                         measurePre.frameTimeHistory.size(),
                         0,
                         nullptr,
                         0.0f,
                         5.0f,
                         ImVec2(-1, sizeH));
        ImGui::PopStyleColor();

        // --------------------------------------------
        // Columna 2: Plot UPDATE (verde)
        // --------------------------------------------
        ImGui::TableSetColumnIndex(1);
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
        ImGui::PlotLines("",
                         measureUpdate.frameTimeHistory.data(),
                         measureUpdate.frameTimeHistory.size(),
                         0,
                         nullptr,
                         0.0f,
                         5.0f,
                         ImVec2(-1, sizeH));
        ImGui::PopStyleColor();

        // --------------------------------------------
        // Columna 3: Plot POST (azul)
        // --------------------------------------------
        ImGui::TableSetColumnIndex(2);
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.2f, 0.5f, 1.0f, 1.0f));
        ImGui::PlotLines("",
                         measurePost.frameTimeHistory.data(),
                         measurePost.frameTimeHistory.size(),
                         0,
                         nullptr,
                         0.0f,
                         5.0f,
                         ImVec2(-1, sizeH));
        ImGui::PopStyleColor();
    }
    ImGui::EndTable();
}

void Profiler::DrawPlotFrameTime(Measure &measure)
{
    // Gráfica de tiempos del frame
    ImGui::Text("Frame Time: %.3f ms (%.1f FPS)",
                measure.diffTime * 1000.0f,
                1.0f / measure.diffTime
    );

    UpdateHistory(measure);
    ImGui::PlotLines("Frame Time",
                     measure.frameTimeHistory.data(),
                     measure.frameTimeHistory.size(),
                     0,
                     nullptr,
                     0.0f,
                     16.67f,  // 60fps = 16.67ms
                     ImVec2(-1, 40));
}

void Profiler::DrawWinProfiler()
{
    DrawPlotFrameTime(measureFrameTime);
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Components")) {
        DrawComponentsTable(25.0f);
        ImGui::Separator();
        DrawFlameGraph();
    }
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Pools")) {
        DrawPools();
    }
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Images in memory")) {
        DrawImagesTable();
    }
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
    measureFrameTime.diffTime = 0;
}

void Profiler::EndTotalFrameTime()
{
    measureFrameTime.endTime = Ticks();
    measureFrameTime.diffTime = measureFrameTime.endTime - measureFrameTime.startTime;
}

MeasuresMap& Profiler::getComponentMeasures()
{
    return componentMeasures;
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

int Profiler::getMemoryImageUsage() const
{
    return memoryImageUsage - memoryOfGUIImages;
}

float Profiler::getMemoryImageUsageKB() const
{
    return getMemoryImageUsage() / 1024.f;
}

int Profiler::getNumberOfImages() const
{
    return (int) images.size() - numberOfGUIImages;
}

void Profiler::setEnabled(bool v)
{
    enable = v;
}

bool Profiler::isEnabled() const
{
    return enable;
}

double Profiler::Ticks()
{
    return (double)SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency();
}

void Profiler::DrawFlameGraph()
{
    double totalTime = measureFrameTime.diffTime;
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

    for (const auto& [name, measure] : componentMeasures) {
        float percentage = (measure.diffTime / totalTime);
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

void Profiler::UpdateHistory(Measure &measure)
{
    measure.frameTimeHistory.push_back(measure.diffTime * 1000.0f);  // En ms
    if (measure.frameTimeHistory.size() > measure.MAX_HISTORY) {
        measure.frameTimeHistory.erase(measure.frameTimeHistory.begin());
    }
}

void Profiler::DrawComponentsHierarchy()
{
    ImGui::Text("Measures amount: %d", componentMeasures.size());
    ImGui::Separator();
    ImGui::Text("Total: %.3f ms", measureFrameTime.diffTime * 1000.0f);

    for (const auto& [name, measure] : componentMeasures) {
        if (ImGui::TreeNode(name.c_str())) {
            double ms = measure.diffTime * 1000.0f;
            double percentage = (measure.diffTime / measureFrameTime.diffTime) * 100.0;

            ImGui::Text("Time: %.3f ms", ms);
            ImGui::Text("Percentage: %.2f%%", percentage);
            ImGui::ProgressBar(percentage / 100.0f);

            ImGui::TreePop();
        }
    }
}