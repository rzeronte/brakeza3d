//
// Created by eduardo on 18/07/23.
//

#include "../../include/Shaders/ShaderBlink.h"
#include "../../include/ComponentsManager.h"


ShaderBlink::ShaderBlink(bool active, Mesh3D *o, float step, Color c) :
    ShaderOpenCL(active),
    isBlinking(false),
    screenWidth(EngineSetup::get()->screenWidth),
    screenHeight(EngineSetup::get()->screenHeight),
    object(o),
    color(c),
    counter(Counter(step))
{
}

void ShaderBlink::update()
{
    if (!isEnabled()) return;
    if (this->object == nullptr) return;
    if (object->isRemoved()) return;

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
        if (mesh == nullptr) {
            return;
        }
        executeKernelOpenCL();
    }
}

void ShaderBlink::executeKernelOpenCL()
{
    auto kernel = ComponentsManager::get()->getComponentRender()->getBlinkKernel();

    clSetKernelArg(kernel, 0, sizeof(int), &screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &screenHeight);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&object->getOpenClRenderer()->clBufferStencil);
    clSetKernelArg(kernel, 4, sizeof(float), &this->color.r);
    clSetKernelArg(kernel, 5, sizeof(float), &this->color.g);
    clSetKernelArg(kernel, 6, sizeof(float), &this->color.b);

    size_t global_item_size = EngineBuffers::get()->sizeBuffers;
    size_t local_item_size = 256;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr );

    //this->debugKernel("ShaderBlink");
}

void ShaderBlink::setColor(Color color) {
    this->color = color;
}