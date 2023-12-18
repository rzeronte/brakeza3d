//
// Created by eduardo on 18/07/23.
//

#include "../../include/Shaders/ShaderBlink.h"
#include "../../include/ComponentsManager.h"

ShaderBlink::ShaderBlink(bool active, Mesh3D *o, float step, Color c) :
    ObjectShaderOpenCL(active, o),
    isBlinking(false),
    object(o),
    color(c),
    counter(Counter(step))
{
    setLabel("ShaderBlink");
}

void ShaderBlink::preUpdate()
{
    ShaderOpenCL::preUpdate();
    update();
}

void ShaderBlink::postUpdate()
{
    ShaderOpenCL::postUpdate();
}

void ShaderBlink::update()
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

void ShaderBlink::executeKernelOpenCL()
{
    glDisable(GL_DEPTH);
    glDisable(GL_BLEND);

    ComponentsManager::get()->getComponentWindow()->getShaderOglColor()->render(
        this->object,
        this->object->vertexbuffer,
        this->object->uvbuffer,
        this->object->normalbuffer,
        (int) this->object->vertices.size(),
        false
    );
}

void ShaderBlink::setColor(Color color) {
    this->color = color;
}