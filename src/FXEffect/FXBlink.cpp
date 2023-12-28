//
// Created by eduardo on 18/07/23.
//

#include "../../include/FXEffect/FXBlink.h"
#include "../../include/ComponentsManager.h"

FXBlink::FXBlink(bool active, Mesh3D *o, float step, Color c) :
    FXEffectOpenGLObject(active, o),
    isBlinking(false),
    object(o),
    color(c),
    counter(Counter(step))
{
    setLabel("FXBlink");
}

void FXBlink::preUpdate()
{
    FXEffectOpenGL::preUpdate();
    update();
}

void FXBlink::postUpdate()
{
    FXEffectOpenGL::postUpdate();
}

void FXBlink::update()
{
    if (!isEnabled() || this->object == nullptr) return;

    counter.update();

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

void FXBlink::executeKernelOpenCL()
{
    glDisable(GL_BLEND);

    ComponentsManager::get()->getComponentWindow()->getShaderOglColor()->render(
            object,
            object->vertexbuffer,
            object->uvbuffer,
            object->normalbuffer,
            (int) object->vertices.size(),
            false
    );

    glEnable(GL_BLEND);
}

void FXBlink::setColor(Color color) {
    this->color = color;
}