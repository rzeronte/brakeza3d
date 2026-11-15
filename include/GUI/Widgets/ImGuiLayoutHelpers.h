/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by Eduardo on 11/01/2026.
//

#ifndef BRAKEZA3D_IMGUILAYOUTHELPERS_H
#define BRAKEZA3D_IMGUILAYOUTHELPERS_H

#pragma once
#include <imgui.h>
#include <vector>

namespace ImGuiSpring {

    inline void BeginHorizontal() {
        // Solo marcamos el inicio, no hacemos nada más
        ImGui::BeginGroup();
    }

    inline void Spring(float width) {
        // Crear un espacio invisible del ancho especificado
        ImGui::SameLine();
        ImGui::Dummy(ImVec2(width, 0));
        ImGui::SameLine();
    }

    inline void EndHorizontal() {
        ImGui::EndGroup();
    }

} // namespace ImGuiSpring

#endif //BRAKEZA3D_IMGUILAYOUTHELPERS_H