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
// Created by Eduardo on 10/07/2026.
//

#ifndef BRAKEZA3D_SOUND3DGUI_H
#define BRAKEZA3D_SOUND3DGUI_H

#include "../../3D/Sound3D.h"
#include "../../Config.h"
#include "../../Misc/Logging.h"
#include "../AddOns/GUIAddonFilePicker.h"
#include "../Widgets/GUIFields.h"
#include <imgui.h>
#include <string>

class Sound3DGUI
{
    static inline GUIFilePicker picker;

public:
    static void DrawPropertiesGUI(Sound3D* o)
    {
        if (!ImGui::CollapsingHeader("Sound3D")) return;

        // ── Source file ────────────────────────────────────────────────────
        ImGui::TextDisabled("Source file");
        char buf[512];
        std::snprintf(buf, sizeof(buf), "%s", o->sourceFile.c_str());
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        if (ImGui::InputText("##srcfile", buf, sizeof(buf)))
            o->sourceFile = buf;

        if (picker.drawTrigger("##browse_snd"))
            picker.open(o->sourceFile, Config::get()->SOUNDS_FOLDER,
                        "../assets/sounds/", {".wav", ".WAV"});
        ImGui::SameLine();
        if (ImGui::Button("Reload##snd"))
            reloadChunk(o);

        if (picker.draw("Sound3D_FilePicker")) {
            o->sourceFile = picker.result;
            reloadChunk(o);
        }

        ImGui::Separator();

        // ── Radii & volume ─────────────────────────────────────────────────
        if (GUIFields::BeginSection("PLAYBACK")) {
            GUIFields::Float("Inner radius", &o->innerRadius, 0.5f, 0.0f, 10000.0f, "%.1f", "Inside this distance the sound plays at full volume.");
            GUIFields::Float("Outer radius", &o->outerRadius, 0.5f, 0.0f, 10000.0f, "%.1f", "Beyond this distance the sound is silent.");
            GUIFields::SliderInt("Base volume", &o->baseVolume, 0, 128, "Volume at full level, 0-128.");
            GUIFields::Check("Loop", &o->loop);
            GUIFields::EndSection();
        }

        ImGui::Separator();

        // ── Debug circles ──────────────────────────────────────────────────
        ImGui::Checkbox("Debug draw (show radii)", &o->debugDraw);
        if (o->debugDraw) {
            ImGui::Indent();
            ImGui::TextColored(ImVec4(0.2f,1.f,0.2f,1.f), "Green = inner (full volume)");
            ImGui::TextColored(ImVec4(1.f,0.55f,0.1f,1.f),"Orange = outer (silence boundary)");
            ImGui::Unindent();
        }

        ImGui::Separator();

        // ── Status ─────────────────────────────────────────────────────────
        if (o->isPlaying)
            ImGui::TextColored(ImVec4(0.3f,1.f,0.3f,1.f), "Playing");
        else
            ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1.f), "Silent");
    }

private:
    static void reloadChunk(Sound3D* o)
    {
        o->ReleaseBuffer();

        if (!o->sourceFile.empty()) {
            o->AttachPcm(Sound3D::DecodeShared(o->sourceFile));
        }
    }
};

#endif //BRAKEZA3D_SOUND3DGUI_H
