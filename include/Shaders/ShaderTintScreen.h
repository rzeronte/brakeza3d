//
// Created by eduardo on 17/2/22.
//

#ifndef BRAKEDA3D_SHADERTINTSCREEN_H
#define BRAKEDA3D_SHADERTINTSCREEN_H

#include "Shader.h"
#include "../Misc/Color.h"
#include "../Misc/Tools.h"
#include "../EngineBuffers.h"

class ShaderTintScreen: public Shader {
public:
    ShaderTintScreen() {
        screenHeight = EngineSetup::get()->screenHeight;
        screenWidth = EngineSetup::get()->screenWidth;

    }

    ShaderTintScreen(float intensityR, float intensityG, float intensityB) : intensity_r(intensityR),
                                                                             intensity_g(intensityG),
                                                                             intensity_b(intensityB) {
        screenHeight = EngineSetup::get()->screenHeight;
        screenWidth = EngineSetup::get()->screenWidth;

    }

    void onUpdate() {
        auto buffer = EngineBuffers::getInstance();
        for (int y = 0; y <screenHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                auto currentPixelColor = Color(buffer->getVideoBuffer(x, y));

                int r_light = (int) ((float) Tools::getRedValueFromColor(currentPixelColor.getColor()) * intensity_r);
                int g_light = (int) ((float) Tools::getGreenValueFromColor(currentPixelColor.getColor()) * intensity_g);
                int b_light = (int) ((float) Tools::getBlueValueFromColor(currentPixelColor.getColor()) * intensity_b);

                currentPixelColor = Color(r_light, g_light, b_light);
                buffer->setVideoBuffer(x, y, currentPixelColor.getColor());
            }
        }
    }

    void setTintColorIntensity(float r, float g, float b) {
        this->intensity_r = r;
        this->intensity_g = g;
        this->intensity_b = b;
    }
private:
    float intensity_r;
    float intensity_g;
    float intensity_b;

    int screenWidth;
    int screenHeight;

};

#endif //BRAKEDA3D_SHADERTINTSCREEN_H
