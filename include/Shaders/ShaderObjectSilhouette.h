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
    }

    ShaderObjectSilhouette(Object3D *o) {
        this->object = o;
        this->screenHeight = EngineSetup::get()->screenHeight;
        this->screenWidth = EngineSetup::get()->screenWidth;
        this->color = Color::green();
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

        auto buffer = EngineBuffers::getInstance();
        for (int y = 0; y < screenHeight; y++) {
            for (int x = 0; x < screenWidth; x++) {
                if (isBorderPixel(x, y)) {
                    buffer->setVideoBuffer(x, y, this->color.getColor());
                }
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
        bool isNotFilled = !this->object->getStencilBufferValue(x, y);

        bool topLeft = this->object->getStencilBufferValue(x-1, y-1);
        bool topMiddle = this->object->getStencilBufferValue(x, y-1);
        bool topRight = this->object->getStencilBufferValue(x+1, y-1);

        bool centerLeft = this->object->getStencilBufferValue(x-1, y);
        bool centerRight = this->object->getStencilBufferValue(x+1, y);

        bool bottomLeft = this->object->getStencilBufferValue(x-1, y+1);
        bool bottomMiddle = this->object->getStencilBufferValue(x, y+1);
        bool bottomRight = this->object->getStencilBufferValue(x+1, y+1);

        if (isNotFilled && (
                topLeft || topMiddle || topRight ||
                centerLeft || centerRight ||
                bottomLeft || bottomMiddle || bottomRight
        )) {
            return true;
        }
    }
};

#endif //BRAKEDA3D_SHADEROBJECTSILHOUETTE_H
