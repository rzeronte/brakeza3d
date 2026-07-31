#include "../../../include/GUI/AddOns/GUIAddonCityLights.h"
#include "../../../include/Brakeza3D.h"
#include "../../../include/3D/LightSpot.h"
#include "../../../include/Components/ComponentScripting.h"
#include "imgui.h"

static float g_constant   = 0.77f;
static float g_linear     = 0.02f;
static float g_cutoff     = 0.90f;
static float g_outer      = 0.60f;

static void setDayTime(float t)
{
    Components::get()->Scripting()->setGlobalScriptVar("CycleDayNight", "RTS_dayTime", sol::make_object(
        Components::get()->Scripting()->getLuaState(), t));
}

static void applyToAllSpots()
{
    for (auto &obj : Brakeza::get()->getSceneObjects()) {
        if (obj->getTypeObject() != ObjectType::LightSpot) continue;
        auto *s = dynamic_cast<LightSpot*>(obj);
        if (!s) continue;
        s->setConstant(g_constant);
        s->setLinear(g_linear);
        s->setCutOff(g_cutoff);
        s->setOuterCutOff(g_outer);
    }
}

void GUIAddonCityLights::Draw()
{
    ImGui::SeparatorText("Time of day");

    struct TimePreset { const char* label; float value; };
    static constexpr TimePreset presets[] = {
        { "Dawn",      0.22f },
        { "Morning",   0.30f },
        { "Noon",      0.50f },
        { "Afternoon", 0.70f },
        { "Dusk",      0.83f },
        { "Night",     0.92f },
    };

    for (auto &p : presets) {
        if (ImGui::Button(p.label, ImVec2(90, 0))) setDayTime(p.value);
        ImGui::SameLine();
    }
    ImGui::NewLine();

    ImGui::SeparatorText("Global spot config");

    ImGui::SliderFloat("Constant",  &g_constant, 0.0f, 2.0f);
    ImGui::SliderFloat("Linear",    &g_linear,   0.0f, 0.2f);
    ImGui::SliderFloat("Cut off",   &g_cutoff,   0.0f, 1.0f);
    ImGui::SliderFloat("Outer cut", &g_outer,    0.0f, 1.0f);

    ImGui::Spacing();
    if (ImGui::Button("Apply to all city lights", ImVec2(-1, 0)))
        applyToAllSpots();
}
