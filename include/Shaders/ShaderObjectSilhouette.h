//
// Created by eduardo on 17/2/22.
//

#ifndef BRAKEDA3D_SHADEROBJECTSILHOUETTE_H
#define BRAKEDA3D_SHADEROBJECTSILHOUETTE_H

#include "../Render/Shader.h"
#include "../Misc/Color.h"
#include "../Misc/Tools.h"
#include "../EngineBuffers.h"

class ShaderObjectSilhouette: public Shader {
public:
    ShaderObjectSilhouette() {
        this->screenHeight = EngineSetup::get()->screenHeight;
        this->screenWidth = EngineSetup::get()->screenWidth;
        this->object = nullptr;
        this->color = Color::green();
        setPhaseRender(EngineSetup::ShadersPhaseRender::POSTUPDATE);
    }

    ShaderObjectSilhouette(Object3D *o) {
        this->object = o;
        this->screenHeight = EngineSetup::get()->screenHeight;
        this->screenWidth = EngineSetup::get()->screenWidth;
        this->color = Color::green();
        setPhaseRender(EngineSetup::ShadersPhaseRender::POSTUPDATE);
    }

    void onUpdate() override {
        if (!isEnabled()) {
            return;
        }

        if (this->object == nullptr) {
            return;
        }

        if (!this->object->isStencilBufferEnabled()) {
            return;
        }

        Uint32 *videoBuffer = EngineBuffers::getInstance()->videoBuffer;
        for (int y = 0; y < screenHeight ; y++) {
            for (int x = 0; x < screenWidth; x++) {
                if (isBorderPixel(x, y) && x < screenWidth-1 && y < screenHeight - 1) {
                    *videoBuffer = this->color.getColor();
                }
                videoBuffer++;
            }
        }
    }

    void setObject(Object3D *o) {
        this->object = o;
    }

    void setColor(Color c) {
        this->color = c;
    }

private:
    int screenWidth;
    int screenHeight;

    Object3D* object;
    Color color;

    bool isBorderPixel(int x, int y) {
        bool isFilled = this->object->getStencilBufferValue(x, y);

        if (isFilled) return false;

        bool topLeft = this->object->getStencilBufferValue(std::max(0, x-1), std::max(0, y-1));
        bool topMiddle = this->object->getStencilBufferValue(x, std::max(0, y-1));
        bool topRight = this->object->getStencilBufferValue(std::min(screenWidth, x+1), std::max(0, y-1));

        bool centerLeft = this->object->getStencilBufferValue(std::max(0, x-1), y);
        bool centerRight = this->object->getStencilBufferValue(std::min(screenWidth, x+1), y);

        bool bottomLeft = this->object->getStencilBufferValue(std::max(0, x-1), y+1);
        bool bottomMiddle = this->object->getStencilBufferValue(x, std::min(screenHeight, y+1));
        bool bottomRight = this->object->getStencilBufferValue(std::min(screenWidth, x+1), std::min(screenHeight, y+1));

        if (topLeft || topMiddle || topRight || centerLeft || centerRight || bottomLeft || bottomMiddle || bottomRight) {
            return true;
        }

        return false;
    }
};

#endif //BRAKEDA3D_SHADEROBJECTSILHOUETTE_H
