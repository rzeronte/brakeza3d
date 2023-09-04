#include "../../include/Shaders/ShaderEdgeObject.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Brakeza3D.h"

ShaderEdgeObject::ShaderEdgeObject(bool active, Color c)
:
    ShaderOpenCL(active, "edge.opencl"),
    object(nullptr),
    color(c)
{
}

void ShaderEdgeObject::update()
{
    Shader::update();

    if (!isEnabled()) return;

    if (this->object == nullptr) {
        return;
    }

    if (!this->object->isStencilBufferEnabled()) {
        return;
    }

    auto mesh = dynamic_cast<Mesh3D*> (object);
    if (mesh == nullptr) {
        return;
    }

    executeKernelOpenCL();
}

void ShaderEdgeObject::executeKernelOpenCL()
{
    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)this->object->getOpenClRenderer()->getClBufferStencil());
    clSetKernelArg(kernel, 5, sizeof(float), &this->color.r);
    clSetKernelArg(kernel, 6, sizeof(float), &this->color.g);
    clSetKernelArg(kernel, 7, sizeof(float), &this->color.b);

    size_t global_item_size = this->bufferSize;
    size_t local_item_size = 16;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr );

    this->debugKernel("ShaderEdge");
}

ShaderEdgeObject::~ShaderEdgeObject() {

}

void ShaderEdgeObject::setColor(Color c) {
    this->color = c;
}

void ShaderEdgeObject::setObject(Mesh3D *object) {
    ShaderEdgeObject::object = object;
}
