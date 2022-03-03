//
// Created by eduardo on 13/2/22.
//

#ifndef BRAKEDA3D_SHADERWATER_H
#define BRAKEDA3D_SHADERWATER_H

#include "../Render/Shader.h"
#include "../Misc/Color.h"
#include "../Misc/Tools.h"
#include "../EngineBuffers.h"

class ShaderWater: public Shader {
public:
    void onUpdate() override {
        Shader::onUpdate();

        if (!isEnabled()) {
            return;
        }

        float LAVA_CLOSENESS = 2.35;
        float LAVA_INTENSITY = 0.45;
        float LAVA_SPEED = 2.55;
        float LAVA_SCALE = 2.35;

        // Default config is used in menu mode
        float intensity_r = 1;
        float intensity_g = 1;
        float intensity_b = 1;

        int type = 1;
        //water = -3 |mud = -4 | lava = -5
        switch (type) {
            default:
            case -3:
                break;
            case -4:
                intensity_r = 0.5;
                intensity_g = 1;
                intensity_b = 0.5;
                break;
            case -5:
                intensity_r = 1;
                intensity_g = 0.5;
                intensity_b = 0.5;
                break;
        }


        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                Color currentPixelColor = Color(EngineBuffers::getInstance()->getVideoBuffer(x, y));

                int r_light = (int) (Tools::getRedValueFromColor(currentPixelColor.getColor()) * intensity_r);
                int g_light = (int) (Tools::getGreenValueFromColor(currentPixelColor.getColor()) * intensity_g);
                int b_light = (int) (Tools::getBlueValueFromColor(currentPixelColor.getColor()) * intensity_b);

                currentPixelColor = Color(r_light, g_light, b_light);

                float cache1 = x / LAVA_CLOSENESS;
                float cache2 = y / LAVA_CLOSENESS;

                int nx = (cache1 + LAVA_INTENSITY * sin(LAVA_SPEED * executionTime + cache2)) * LAVA_SCALE;
                int ny = (cache2 + LAVA_INTENSITY * sin(LAVA_SPEED * executionTime + cache1)) * LAVA_SCALE;


                if (Tools::isPixelInWindow(nx, ny)) {
                    int bufferIndex = nx + ny * w;
                    videoBuffer[bufferIndex] = currentPixelColor.getColor();
                }
            }
        }

        memcpy(&EngineBuffers::getInstance()->videoBuffer, &videoBuffer, sizeof(videoBuffer));
    }
};

#endif //BRAKEDA3D_SHADERWATER_H
