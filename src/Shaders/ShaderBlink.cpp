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
    auto kernel = ComponentsManager::get()->getComponentRender()->getBlinkKernel();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)object->getOpenClRenderer()->getClBufferStencil());
    clSetKernelArg(kernel, 4, sizeof(float), &this->color.r);
    clSetKernelArg(kernel, 5, sizeof(float), &this->color.g);
    clSetKernelArg(kernel, 6, sizeof(float), &this->color.b);

    size_t global_item_size[2] = {(size_t) EngineSetup::get()->screenWidth, (size_t) EngineSetup::get()->screenHeight};
    size_t local_item_size[2] = {16, 16};    // Tamaño local de trabajo (puede ajustarse según la GPU)

    clEnqueueNDRangeKernel(clQueue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);

    //this->debugKernel("ShaderBlink");
}

void ShaderBlink::setColor(Color color) {
    this->color = color;
}