//
// Created by eduardo on 5/3/22.
//

#ifndef BRAKEDA3D_SHADERBLINK_H
#define BRAKEDA3D_SHADERBLINK_H
#include "../Render/Shader.h"
#include "../Misc/Color.h"
#include "../Misc/Tools.h"
#include "../EngineBuffers.h"

#define DEFAULT_BLINK_SECONDS 1
class ShaderBlink: public Shader {
    bool blink = false;
public:
    ShaderBlink() {
        this->screenHeight = EngineSetup::get()->screenHeight;
        this->screenWidth = EngineSetup::get()->screenWidth;
        this->object = nullptr;
        this->color = Color::green();
        setStep(DEFAULT_BLINK_SECONDS);
        setPhaseRender(EngineSetup::ShadersPhaseRender::POSTUPDATE);
        this->counter.setStep(step);
        this->counter.setEnabled(true);
    }

    ShaderBlink(Object3D *o) {
        this->object = o;
        this->screenHeight = EngineSetup::get()->screenHeight;
        this->screenWidth = EngineSetup::get()->screenWidth;
        this->color = Color::green();
        setStep(DEFAULT_BLINK_SECONDS);
        setPhaseRender(EngineSetup::ShadersPhaseRender::POSTUPDATE);
    }

    void onUpdate() override {
        if (!isEnabled()) {
            return;
        }

        if (this->object == nullptr) {
            return;
        }

        counter.update();

        if (!this->object->isStencilBufferEnabled()) {
            return;
        }

        if (blink) {
            if (counter.isFinished()) {
                blink = false;
                counter.setEnabled(true);
            }
        } else {
            if (counter.isFinished()) {
                blink = true;
                counter.setEnabled(true);
            }
        }

        if (blink) {
            auto buffer = EngineBuffers::getInstance();
            for (int y = 0; y < screenHeight; y++) {
                for (int x = 0; x < screenWidth; x++) {
                    if (this->object->getStencilBufferValue(x, y)) {
                        buffer->setVideoBuffer(x, y, this->color.getColor());
                    }
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

    Color getColor() {
        return this->color;
    }

    void setStep(float s) {
        this->step = s;
        this->counter.setStep(step);
        this->counter.setEnabled(true);
    }

    float step;
private:
    int screenWidth;
    int screenHeight;

    Object3D* object;
    Color color;
    Counter counter;
};
#endif //BRAKEDA3D_SHADERBLINK_H
