//
// Campos de inspector compartidos: secciones en tabla de 2 columnas (etiqueta fija | control que llena el resto),
// con ayuda al pasar el ratón por la etiqueta. Todos los inspectores que los usen se ven igual.
//
// Uso:
//   if (GUIFields::BeginSection("EMISSION")) {
//       GUIFields::Float("Rate", &c.RATE, 1.0f, 0.0f, 5000.0f, "%.1f /s", "Particles per second.");
//       GUIFields::Check("Loop", &o->loop);
//       GUIFields::EndSection();
//   }
//
// Cada campo devuelve true si el usuario lo ha modificado este frame. Para controles a medida:
//   GUIFields::Row("Mesh", "ayuda");  // deja el cursor en la columna del control
//   ImGui::SetNextItemWidth(-FLT_MIN); ImGui::...("##v", ...);
//

#ifndef BRAKEZA3D_GUIFIELDS_H
#define BRAKEZA3D_GUIFIELDS_H

#include <cfloat>
#include <cstdarg>
#include <initializer_list>
#include <vector>
#include <imgui.h>
#include "../../Render/UndoManager.h"

namespace GUIFields
{
    inline constexpr float kLabelWidth = 130.0f;

    // Título de sección + tabla de 2 columnas. El título identifica la tabla y abre un ámbito de IDs,
    // así que dos secciones pueden repetir etiquetas ("Size", "Radius"...) sin colisionar.
    // valueWidth > 0 fija el ancho de la columna del control: necesario en menús/popups, que se
    // autodimensionan y colapsarían una columna que se estira. Con 0 (por defecto) el control llena el resto.
    inline bool BeginSection(const char* title, float labelWidth = kLabelWidth, float valueWidth = 0.0f)
    {
        ImGui::SeparatorText(title);
        ImGui::PushID(title);
        if (!ImGui::BeginTable("##fields", 2, ImGuiTableFlags_SizingFixedFit)) {
            ImGui::PopID();
            return false;
        }
        ImGui::TableSetupColumn("label", ImGuiTableColumnFlags_WidthFixed, labelWidth);
        if (valueWidth > 0.0f) ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthFixed, valueWidth);
        else                   ImGui::TableSetupColumn("value", ImGuiTableColumnFlags_WidthStretch);
        return true;
    }

    // Icono opcional delante de la etiqueta de la PRÓXIMA fila (estilo de los menús del editor)
    inline ImTextureID g_nextRowIcon = nullptr;
    inline ImVec2      g_nextRowIconSize = ImVec2(0, 0);
    inline void NextRowIcon(ImTextureID icon, ImVec2 size)
    {
        g_nextRowIcon = icon;
        g_nextRowIconSize = size;
    }

    // Cada fila abre su ámbito de IDs; se cierra sola al empezar la siguiente fila o al terminar la sección
    inline bool g_rowOpen = false;

    // Cierra el ámbito de IDs de la fila actual (opcional: lo hacen solos Row y EndSection)
    inline void EndRow()
    {
        if (g_rowOpen) {
            UndoManager::get().onFieldItem();   // el último widget es el campo recién dibujado
            ImGui::PopID();
            g_rowOpen = false;
        }
    }

    inline void EndSection()
    {
        EndRow();
        ImGui::EndTable();
        ImGui::PopID();
    }

    // Nueva fila: etiqueta (con ayuda opcional) y deja el cursor en la columna del control
    inline void Row(const char* label, const char* help = nullptr)
    {
        EndRow();
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        ImGui::AlignTextToFramePadding();
        if (g_nextRowIcon != nullptr) {
            ImGui::Image(g_nextRowIcon, g_nextRowIconSize);
            ImGui::SameLine();
            g_nextRowIcon = nullptr;
        }
        ImGui::TextUnformatted(label);
        if (help != nullptr && ImGui::IsItemHovered()) ImGui::SetTooltip("%s", help);
        ImGui::TableSetColumnIndex(1);
        ImGui::PushID(label);
        g_rowOpen = true;
        ImGui::SetNextItemWidth(-FLT_MIN);
    }

    inline bool Float(const char* label, float* v, float speed, float vmin, float vmax,
                      const char* fmt = "%.2f", const char* help = nullptr)
    {
        Row(label, help);
        const bool r = ImGui::DragFloat("##v", v, speed, vmin, vmax, fmt);
        EndRow();
        return r;
    }

    inline bool FloatRange(const char* label, float* lo, float* hi, float speed, float vmin, float vmax,
                           const char* fmtLo = "min %.2f", const char* fmtHi = "max %.2f", const char* help = nullptr)
    {
        Row(label, help);
        const bool r = ImGui::DragFloatRange2("##v", lo, hi, speed, vmin, vmax, fmtLo, fmtHi);
        EndRow();
        return r;
    }

    inline bool Float2(const char* label, float* v, float speed, float vmin, float vmax,
                       const char* fmt = "%.2f", const char* help = nullptr)
    {
        Row(label, help);
        const bool r = ImGui::DragFloat2("##v", v, speed, vmin, vmax, fmt);
        EndRow();
        return r;
    }

    inline bool Float3(const char* label, float* v, float speed, float vmin, float vmax,
                       const char* fmt = "%.2f", const char* help = nullptr)
    {
        Row(label, help);
        const bool r = ImGui::DragFloat3("##v", v, speed, vmin, vmax, fmt);
        EndRow();
        return r;
    }

    inline bool Int(const char* label, int* v, float speed, int vmin, int vmax,
                    const char* fmt = "%d", const char* help = nullptr)
    {
        Row(label, help);
        const bool r = ImGui::DragInt("##v", v, speed, vmin, vmax, fmt);
        EndRow();
        return r;
    }

    inline bool IntRange(const char* label, int* lo, int* hi, float speed, int vmin, int vmax,
                         const char* fmtLo = "min %d", const char* fmtHi = "max %d", const char* help = nullptr)
    {
        Row(label, help);
        const bool r = ImGui::DragIntRange2("##v", lo, hi, speed, vmin, vmax, fmtLo, fmtHi);
        EndRow();
        return r;
    }

    inline bool Slider(const char* label, float* v, float vmin, float vmax, const char* fmt = "%.2f",
                       const char* help = nullptr, ImGuiSliderFlags flags = 0)
    {
        Row(label, help);
        const bool r = ImGui::SliderFloat("##v", v, vmin, vmax, fmt, flags);
        EndRow();
        return r;
    }

    // Varios floats sueltos (no contiguos en memoria) en una misma fila, repartiendo el ancho a partes iguales.
    // Devuelve una máscara de bits: bit i = el campo i se modificó este frame.
    inline int FloatN(const char* label, std::initializer_list<float*> values, float speed, float vmin, float vmax,
                      const char* fmt = "%.3f", const char* help = nullptr)
    {
        Row(label, help);
        const int n = static_cast<int>(values.size());
        const float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        const float itemW = (ImGui::GetContentRegionAvail().x - spacing * static_cast<float>(n - 1)) / static_cast<float>(n);
        int mask = 0, i = 0;
        for (float* v : values) {
            if (i > 0) ImGui::SameLine(0.0f, spacing);
            ImGui::PushID(i);
            ImGui::SetNextItemWidth(itemW);
            if (ImGui::DragFloat("##v", v, speed, vmin, vmax, fmt)) mask |= (1 << i);
            ImGui::PopID();
            i++;
        }
        EndRow();
        return mask;
    }

    // Float con límites opcionales (min == max == 0 -> sin límites, como DragFloat)
    inline bool FloatFree(const char* label, float* v, float speed, const char* fmt = "%.3f", const char* help = nullptr)
    {
        return Float(label, v, speed, 0.0f, 0.0f, fmt, help);
    }

    // Texto de solo lectura en la columna del valor
    inline void Text(const char* label, const char* fmt, ...) IM_FMTARGS(2);
    inline void Text(const char* label, const char* fmt, ...)
    {
        Row(label);
        va_list args;
        va_start(args, fmt);
        ImGui::TextV(fmt, args);
        va_end(args);
        EndRow();
    }

    inline bool SliderInt(const char* label, int* v, int vmin, int vmax, const char* help = nullptr)
    {
        Row(label, help);
        const bool r = ImGui::SliderInt("##v", v, vmin, vmax);
        EndRow();
        return r;
    }

    inline bool Check(const char* label, bool* v, const char* help = nullptr)
    {
        Row(label, help);
        const bool r = ImGui::Checkbox("##v", v);
        EndRow();
        return r;
    }

    // Lista de nombres estática. El índice se pasa y se devuelve como int (castear al enum).
    inline bool Combo(const char* label, int* index, const char* const* names, int count, const char* help = nullptr)
    {
        Row(label, help);
        const bool r = ImGui::Combo("##v", index, names, count);
        EndRow();
        return r;
    }

    // Color RGB (sin alfa). Edita un array de 3 floats.
    inline bool Color3(const char* label, float* rgb, const char* help = nullptr)
    {
        Row(label, help);
        const bool r = ImGui::ColorEdit3("##v", rgb, ImGuiColorEditFlags_NoOptions);
        EndRow();
        return r;
    }

    // Color RGBA. Edita un array de 4 floats.
    inline bool Color4(const char* label, float* rgba, const char* help = nullptr)
    {
        Row(label, help);
        const bool r = ImGui::ColorEdit4("##v", rgba, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoOptions);
        EndRow();
        return r;
    }

    // Texto editable sobre un buffer de char propio
    inline bool String(const char* label, char* buf, size_t bufSize, const char* help = nullptr,
                       ImGuiInputTextFlags flags = 0)
    {
        Row(label, help);
        const bool r = ImGui::InputText("##v", buf, bufSize, flags);
        EndRow();
        return r;
    }

    // Lista de nombres dinámica (p. ej. construida cada frame). El índice se pasa y se devuelve como int.
    inline bool ComboItems(const char* label, int* index, const std::vector<const char*>& names,
                           const char* help = nullptr)
    {
        Row(label, help);
        const bool r = ImGui::Combo("##v", index, names.data(), static_cast<int>(names.size()));
        EndRow();
        return r;
    }

    // Sección con ámbito (RAII): cierra la tabla sola al salir del bloque, aunque haya return/continue.
    //   GUIFields::Section s("EMISSION");
    //   if (s) { GUIFields::Float(...); }
    class Section
    {
    public:
        explicit Section(const char* title, float labelWidth = kLabelWidth, float valueWidth = 0.0f) : open(BeginSection(title, labelWidth, valueWidth)) {}
        ~Section() { if (open) EndSection(); }
        Section(const Section&) = delete;
        Section& operator=(const Section&) = delete;
        explicit operator bool() const { return open; }

    private:
        bool open;
    };
}

#endif //BRAKEZA3D_GUIFIELDS_H
