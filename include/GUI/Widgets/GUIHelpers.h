//
// Pequeños ayudantes de ImGui que se repetían por todo el GUI.
//

#ifndef BRAKEZA3D_GUIHELPERS_H
#define BRAKEZA3D_GUIHELPERS_H

#include <imgui.h>
#include "../GUI.h"

namespace GUIHelpers
{
    inline constexpr float kDialogButtonWidth = 120.0f;

    // Centra la próxima ventana/popup en el viewport principal (al aparecer)
    inline void CenterNextWindow(ImGuiCond cond = ImGuiCond_Appearing)
    {
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), cond, ImVec2(0.5f, 0.5f));
    }

    // Botón de diálogo con el ancho estándar (120 px)
    inline bool DialogButton(const char* label, float width = kDialogButtonWidth)
    {
        return ImGui::Button(label, ImVec2(width, 0));
    }

    inline bool CancelButton(float width = kDialogButtonWidth) { return DialogButton("Cancel", width); }

    // Dos columnas dentro de una tabla ya abierta: la izquierda se estira, la derecha se ajusta a su contenido
    inline void SetupLeftRightColumns()
    {
        ImGui::TableSetupColumn("Left", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Right", ImGuiTableColumnFlags_WidthFixed);
    }

    // Destino de arrastrar-soltar para imágenes del explorador de archivos.
    // Llama a onDrop(ruta) cuando se suelta una; devuelve true en ese caso.
    // Debe llamarse justo después del widget que actúa como zona de suelta.
    template <typename Fn>
    inline bool ImageDropTarget(Fn&& onDrop)
    {
        bool dropped = false;
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(GUIType::DragDropTarget::IMAGE_ITEM)) {
                onDrop(static_cast<const char*>(payload->Data));
                dropped = true;
            }
            ImGui::EndDragDropTarget();
        }
        return dropped;
    }
}

#endif //BRAKEZA3D_GUIHELPERS_H
