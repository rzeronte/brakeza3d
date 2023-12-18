//
// Created by eduardo on 18/07/23.
//

#include "../../include/Shaders/ShaderBlink.h"
#include "../../include/ComponentsManager.h"


ShaderBlink::ShaderBlink(bool active, Mesh3D *o, float step, Color c) :
    ShaderOpenCL(active),
    isBlinking(false),
    object(o),
    color(c),
    counter(Counter(step))
{
}

void ShaderBlink::update()
{
    if (!isEnabled() || this->object == nullptr || object->isRemoved()) return;

    counter.update();

    if (!this->object->isStencilBufferEnabled()) return;

    if (isBlinking) {
        if (counter.isFinished()) {
            isBlinking = false;
            counter.setEnabled(true);
        }
    } else {
        if (counter.isFinished()) {
            isBlinking = true;
            counter.setEnabled(true);
        }
    }

    if (isBlinking) {
        auto mesh = dynamic_cast<Mesh3D*> (object);
        if (mesh != nullptr) {
            executeKernelOpenCL();
        }
    }
}

void ShaderBlink::executeKernelOpenCL()
{
}

void ShaderBlink::setColor(Color color) {
    this->color = color;
}