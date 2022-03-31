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
    float scale = 1.5f;
    float speed = 6.0f;
    // the amount of shearing (shifting of a single column or row)
    // 1.0 = entire screen height offset (to both sides, meaning it's 2.0 in total)
    float xDistMag = 0.04f;
    float yDistMag = 0.0f;

    // cycle multiplier for a given screen height
    // 2*PI = you see a complete sine wave from top..bottom
    float xSineCycles = 6.28f;
    float ySineCycles = 6.28f;

    void update() override {
        Shader::update();

        if (!isEnabled()) {
            return;
        }

        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                Color currentPixelColor(EngineBuffers::getInstance()->getVideoBuffer(x, y));

                float tx = (float) x / scale;
                float ty = (float) y / scale;

                float time = executionTime *speed;
                float xAngle = time + ty * ySineCycles;
                float yAngle = time + tx * xSineCycles;

                float distortOffsetX = sin(xAngle) * xDistMag;
                float distortOffsetY = sin(yAngle) * yDistMag;

                int nx = (int)(x + distortOffsetX);
                int ny = (int)(y + distortOffsetY);

                if (Tools::isPixelInWindow(nx, ny)) {
                    int bufferIndex = nx + ny * w;
                    videoBuffer[bufferIndex] = currentPixelColor.getColor();
                    EngineBuffers::getInstance()->setVideoBuffer(nx, ny, currentPixelColor.getColor());
                }
            }
        }

        //memcpy(&EngineBuffers::getInstance()->videoBuffer, &videoBuffer, sizeof(videoBuffer));
    }
};

#endif //BRAKEDA3D_SHADERWATER_H
