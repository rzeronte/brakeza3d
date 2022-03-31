//
// Created by eduardo on 13/2/22.
//

#ifndef BRAKEDA3D_SHADERFIRE_H
#define BRAKEDA3D_SHADERFIRE_H

#include "../Render/Shader.h"
#include "../Misc/Color.h"
#include "../EngineBuffers.h"

class ShaderFire: public Shader {
public:

    std::vector<Color> fireColors;

    int rgbs[111] = {
            0x07, 0x07, 0x07,
            0x1F, 0x07, 0x07,
            0x2F, 0x0F, 0x07,
            0x47, 0x0F, 0x07,
            0x57, 0x17, 0x07,
            0x67, 0x1F, 0x07,
            0x77, 0x1F, 0x07,
            0x8F, 0x27, 0x07,
            0x9F, 0x2F, 0x07,
            0xAF, 0x3F, 0x07,
            0xBF, 0x47, 0x07,
            0xC7, 0x47, 0x07,
            0xDF, 0x4F, 0x07,
            0xDF, 0x57, 0x07,
            0xDF, 0x57, 0x07,
            0xD7, 0x5F, 0x07,
            0xD7, 0x5F, 0x07,
            0xD7, 0x67, 0x0F,
            0xCF, 0x6F, 0x0F,
            0xCF, 0x77, 0x0F,
            0xCF, 0x7F, 0x0F,
            0xCF, 0x87, 0x17,
            0xC7, 0x87, 0x17,
            0xC7, 0x8F, 0x17,
            0xC7, 0x97, 0x1F,
            0xBF, 0x9F, 0x1F,
            0xBF, 0x9F, 0x1F,
            0xBF, 0xA7, 0x27,
            0xBF, 0xA7, 0x27,
            0xBF, 0xAF, 0x2F,
            0xB7, 0xAF, 0x2F,
            0xB7, 0xB7, 0x2F,
            0xB7, 0xB7, 0x37,
            0xCF, 0xCF, 0x6F,
            0xDF, 0xDF, 0x9F,
            0xEF, 0xEF, 0xC7,
            0xFF, 0xFF, 0xFF
    };

    ShaderFire() {
        this->fireColors.resize(37);
        makeFireColors();
        fireShaderSetup();
    }

    void fireShaderSetup() const {
        // Set whole screen to 0 (color: 0x07,0x07,0x07)
        int FIRE_WIDTH = w;
        int FIRE_HEIGHT = h;

        int firePixelsBufferSize = FIRE_HEIGHT * FIRE_WIDTH;
        for (int i = 0; i < firePixelsBufferSize; i++) {
            this->videoBuffer[i] = 0;
        }

        // Set bottom line to 37 (color white: 0xFF,0xFF,0xFF)
        for (int i = 0; i < FIRE_WIDTH; i++) {
            this->videoBuffer[(FIRE_HEIGHT - 1) * FIRE_WIDTH + i] = 36;
        }
    }

    void makeFireColors() {
        // Populate pallete
        for (int i = 0; i < 111 / 3; i++) {
            fireColors[i] = Color(
                    rgbs[i * 3 + 0],
                    rgbs[i * 3 + 1],
                    rgbs[i * 3 + 2]
            );

            videoBuffer[100 * 320 + i] = fireColors[i].getColor();
        }
    }

    void update() override {
        Shader::update();

        if (!isEnabled()) {
            return;
        }

        // Set whole screen to 0 (color: 0x07,0x07,0x07)
        int FIRE_WIDTH = w;
        int FIRE_HEIGHT = h;

        for (int x = 0; x < FIRE_WIDTH; x++) {
            for (int y = 1; y < FIRE_HEIGHT; y++) {
                int src = (y * FIRE_WIDTH + x);

                int pixel = this->videoBuffer[src];

                if (pixel == 0) {
                    this->videoBuffer[src - FIRE_WIDTH] = 0;
                } else {
                    int randIdx = Tools::random(0, 3) & 3;
                    int dst = src - randIdx + 1;
                    this->videoBuffer[dst - FIRE_WIDTH] = pixel - (randIdx & 1);
                }
            }
        }

        for (int y = 1; y < FIRE_HEIGHT; y++) {
            for (int x = 0; x < FIRE_WIDTH; x++) {
                int index = y * FIRE_WIDTH + x;
                int fireIndex = this->videoBuffer[index];

                if (fireIndex != 0) {
                    Color fireColor = this->fireColors[fireIndex];
                    EngineBuffers::getInstance()->videoBuffer[index] = fireColor.getColor(); //::black();
                }
            }
        }
    }
};
#endif //BRAKEDA3D_SHADERFIRE_H
