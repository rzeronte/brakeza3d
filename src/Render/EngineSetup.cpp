
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Render/Tools.h"

EngineSetup* EngineSetup::instance = 0;

EngineSetup* EngineSetup::getInstance()
{
    if (instance == 0) {
        instance = new EngineSetup();
    }

    return instance;
}

EngineSetup::EngineSetup()
{
    LIGHT_PATTERNS[0]  = "m"; // 0 normal
    LIGHT_PATTERNS[1]  = "mmnmmommommnonmmonqnmmo"; // 1 FLICKER (first variety)
    LIGHT_PATTERNS[2]  = "abcdefghijklmnopqrstuvwxyzyxwvutsrqponmlkjihgfedcba"; // 2 SLOW STRONG PULSE
    LIGHT_PATTERNS[3]  = "mmmmmaaaaammmmmaaaaaabcdefgabcdefg"; // 3 CANDLE (first variety)
    LIGHT_PATTERNS[4]  = "mamamamamama"; // 4 FAST STROBE
    LIGHT_PATTERNS[5]  = "jklmnopqrstuvwxyzyxwvutsrqponmlkj"; // 5 GENTLE PULSE 1
    LIGHT_PATTERNS[6]  = "nmonqnmomnmomomno"; // 6 FLICKER (second variety)
    LIGHT_PATTERNS[7]  = "mmmaaaabcdefgmmmmaaaammmaamm"; // 7 CANDLE (second variety)
    LIGHT_PATTERNS[8]  = "mmmaaammmaaammmabcdefaaaammmmabcdefmmmaaaa"; // 8 CANDLE (third variety)
    LIGHT_PATTERNS[9]  = "aaaaaaaazzzzzzzz"; // 9 SLOW STROBE (fourth variety)
    LIGHT_PATTERNS[10] = "mmamammmmammamamaaamammma"; // 10 FLUORESCENT FLICKER
    LIGHT_PATTERNS[11] = "abcdefghijklmnopqrrqponmlkjihgfedcba"; // 11 SLOW PULSE NOT FADE TO BLACK

    FOG_IMGUI_COLOR.x = Tools::getRedValueFromColor(this->FOG_COLOR);
    FOG_IMGUI_COLOR.y = Tools::getGreenValueFromColor(this->FOG_COLOR);
    FOG_IMGUI_COLOR.z = Tools::getBlueValueFromColor(this->FOG_COLOR);
}

void EngineSetup::setResolution(int w, int h)
{
    EngineSetup::getInstance()->screenWidth = w;
    EngineSetup::getInstance()->screenHeight = h;
}