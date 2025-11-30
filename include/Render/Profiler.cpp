//
// Created by Eduardo on 29/11/2025.
//

#include "Profiler.h"
#include <algorithm>

#include "../Brakeza.h"
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
    Logging::Message("[Profiler] Adding image '%s'", image->getFileName().c_str());
}

void Profiler::CaptureGUIMemoryUsage()
{
    numberOfGUIImages = (int) images.size();
    memoryOfGUIImages = memoryImageUsage;
    setEnabled(true);
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
        Logging::Message("[Profiler] Removing image '%s'", image->getFileName().c_str());

        images.erase(it);
    }
}

void Profiler::DrawComponentsTable()
{
    double totalTime = frameTime.diffTime;

    ImGui::Text("Total Frame Time: %.6f ms", totalTime * 1000.0f);

    if (ImGui::BeginTable("ComponentTimings", 8,
        ImGuiTableFlags_Borders |
        ImGuiTableFlags_RowBg |
        ImGuiTableFlags_Resizable |
        ImGuiTableFlags_Sortable)) {

        // Configurar columnas
        ImGui::TableSetupColumn("Component", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableSetupColumn("Pre (ms)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Update (ms)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Post (ms)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("Total (ms)", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("%", ImGuiTableColumnFlags_WidthFixed, 60.0f);
        ImGui::TableSetupColumn("Bar", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Breakdown", ImGuiTableColumnFlags_WidthFixed, 150.0f);
        ImGui::TableHeadersRow();

        for (auto c : ComponentsManager::get()->Components()) {
            auto measurePre = componentMeasures[c->getLabel() + "_pre"];
            auto measureUpdate = componentMeasures[c->getLabel() + "_update"];
            auto measurePost = componentMeasures[c->getLabel() + "_post"];

            // Calcular tiempo total del componente
            double componentTotal = measurePre.diffTime + measureUpdate.diffTime + measurePost.diffTime;
            double percentage = totalTime > 0 ? (componentTotal / totalTime) * 100.0 : 0.0;

            // Determinar color según porcentaje
            ImVec4 color;
            if (percentage > 30.0) {
                color = ImVec4(1.0f, 0.2f, 0.2f, 1.0f);  // Rojo
            } else if (percentage > 15.0) {
                color = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);  // Amarillo
            } else {
                color = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);  // Verde
            }

            ImGui::TableNextRow();

            // Columna: Nombre del componente
            ImGui::TableSetColumnIndex(0);
            ImGui::TextColored(color, "%s", c->getLabel().c_str());

            // --- PRE (Rojo) ---
            ImGui::TableSetColumnIndex(1);
            ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "%.6f", measurePre.diffTime * 1000.0f);

            // --- UPDATE (Verde) ---
            ImGui::TableSetColumnIndex(2);
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "%.6f", measureUpdate.diffTime * 1000.0f);

            // --- POST (Azul) ---
            ImGui::TableSetColumnIndex(3);
            ImGui::TextColored(ImVec4(0.2f, 0.5f, 1.0f, 1.0f), "%.6f", measurePost.diffTime * 1000.0f);

            // Columna: Total (ms)
            ImGui::TableSetColumnIndex(4);
            ImGui::Text("%.6f", componentTotal * 1000.0f);

            // Columna: Porcentaje
            ImGui::TableSetColumnIndex(5);
            ImGui::Text("%.2f%%", percentage);

            // Columna: Barra de progreso
            ImGui::TableSetColumnIndex(6);
            ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);
            ImGui::ProgressBar(percentage / 100.0f, ImVec2(-1, 0));
            ImGui::PopStyleColor();

            // -------------------------------------------------------------
            // COLUMNA BREAKDOWN — Barra apilada en un solo rectángulo
            // -------------------------------------------------------------
            ImGui::TableSetColumnIndex(7);

            float prePercent    = componentTotal > 0 ? (measurePre.diffTime    / componentTotal) : 0.0f;
            float updatePercent = componentTotal > 0 ? (measureUpdate.diffTime / componentTotal) : 0.0f;
            float postPercent   = componentTotal > 0 ? (measurePost.diffTime   / componentTotal) : 0.0f;

            ImVec2 pos = ImGui::GetCursorScreenPos();
            float fullWidth = ImGui::GetColumnWidth();
            float height = 12.0f;

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

void Profiler::DrawPlot()
{
    // Gráfica de tiempos del frame
    ImGui::Text("Frame Time: %.3f ms (%.1f FPS)",
                frameTime.diffTime * 1000.0f,
                1.0f / frameTime.diffTime);
    UpdateHistory();
    ImGui::PlotLines("Frame Time",
                     frameTimeHistory.data(),
                     frameTimeHistory.size(),
                     0,
                     nullptr,
                     0.0f,
                     16.67f,  // 60fps = 16.67ms
                     ImVec2(0, 80));

    // Línea de referencia para 60fps
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(1,1,0,1), "Target: 16.67ms");
}

void Profiler::DrawPropertiesGUI()
{
    ImGui::Text("Measures amount: %d", (int) componentMeasures.size());

    ImGui::Separator();
    ImGui::Text("Component Performance:");

    if (ImGui::CollapsingHeader("Component Breakdown")) {
        DrawComponentsTable();
    }
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Component FlameGraph")) {
        DrawFlameGraph();
    }
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Plot Components")) {
        DrawPlot();
    }
    if (ImGui::CollapsingHeader("Hierarchy")) {
        DrawComponentsHierarchy();
    }
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Images in memory")) {
        DrawImagesTable();
    }
}

void Profiler::ResetMeasure(MeasuresMap &map, const std::string &label)
{
    componentMeasures[label].startTime = 0;
    componentMeasures[label].endTime = 0;
    componentMeasures[label].diffTime = 0;
}

void Profiler::ResetTotalFrameTime()
{
    frameTime.startTime = ticks();
    frameTime.endTime = 0;
    frameTime.diffTime = 0;
}

void Profiler::EndTotalFrameTime()
{
    frameTime.endTime = ticks();
    frameTime.diffTime = frameTime.endTime - frameTime.startTime;
}

MeasuresMap& Profiler::getComponentMeasures()
{
    return componentMeasures;
}

void Profiler::StartMeasure(MeasuresMap &map, const std::string &name)
{
    map[name].startTime = ticks();
}

void Profiler::EndMeasure(MeasuresMap &map, const std::string &name)
{
    map[name].endTime = ticks();
    map[name].diffTime = map[name].endTime - map[name].startTime;
    map[name].UpdateAverage();
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

double Profiler::ticks()
{
    return (double)SDL_GetPerformanceCounter() / (double)SDL_GetPerformanceFrequency();
}

void Profiler::DrawFlameGraph()
{
    ImGui::Text("Component Breakdown:");

    double totalTime = frameTime.diffTime;
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

void Profiler::UpdateHistory()
{
    frameTimeHistory.push_back(frameTime.diffTime * 1000.0f);  // En ms
    if (frameTimeHistory.size() > MAX_HISTORY) {
        frameTimeHistory.erase(frameTimeHistory.begin());
    }
}

void Profiler::DrawComponentsHierarchy()
{
    ImGui::Text("Measures amount: %d", componentMeasures.size());
    ImGui::Separator();
    ImGui::Text("Total: %.3f ms", frameTime.diffTime * 1000.0f);

    for (const auto& [name, measure] : componentMeasures) {
        if (ImGui::TreeNode(name.c_str())) {
            double ms = measure.diffTime * 1000.0f;
            double percentage = (measure.diffTime / frameTime.diffTime) * 100.0;

            ImGui::Text("Time: %.3f ms", ms);
            ImGui::Text("Percentage: %.2f%%", percentage);
            ImGui::ProgressBar(percentage / 100.0f);

            ImGui::TreePop();
        }
    }
}