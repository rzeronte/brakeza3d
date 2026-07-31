//
// Created by Eduardo on 10/07/2026.
//

#ifndef BRAKEZA3D_SOUND3DGUI_H
#define BRAKEZA3D_SOUND3DGUI_H

#include "../../3D/Sound3D.h"
#include "../../Config.h"
#include "../../Misc/Logging.h"
#include "../AddOns/GUIAddonFilePicker.h"
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
        ImGui::DragFloat("Inner Radius", &o->innerRadius, 0.5f, 0.0f, 10000.0f, "%.1f");
        ImGui::DragFloat("Outer Radius", &o->outerRadius, 0.5f, 0.0f, 10000.0f, "%.1f");
        ImGui::SliderInt("Base Volume",  &o->baseVolume,  0, 128);
        ImGui::Checkbox ("Loop",         &o->loop);

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
            ImGui::TextColored(ImVec4(0.3f,1.f,0.3f,1.f), "Playing  (ch %d)", o->channel);
        else
            ImGui::TextColored(ImVec4(0.6f,0.6f,0.6f,1.f), "Silent");
    }

private:
    static void reloadChunk(Sound3D* o)
    {
        if (o->channel >= 0 && Mix_GetChunk(o->channel) == o->mixChunk)
            Mix_HaltChannel(o->channel);
        if (o->mixChunk) { Mix_FreeChunk(o->mixChunk); o->mixChunk = nullptr; }
        o->channel   = -1;
        o->isPlaying = false;
        if (!o->sourceFile.empty()) {
            o->mixChunk = Mix_LoadWAV(o->sourceFile.c_str());
            if (!o->mixChunk)
                LOG_ERROR("[Sound3DGUI] Mix_LoadWAV failed: %s", Mix_GetError());
        }
    }
};

#endif //BRAKEZA3D_SOUND3DGUI_H
