//
// Created by eduardo on 5/06/23.
//

#include "ShaderZombie.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"

ShaderZombie::ShaderZombie(bool active, const std::string& filename,Mesh3D* object, MeshOpenCLRenderer *renderer)
:
    ShaderOpenCL(active, "zombie.cl"),
    meshOpenCLRenderer(renderer),
    object(object),
    offsetY(0),
    offsetX(0),
    image(Image(filename))
{
    offsetX = 10;
    offsetY = 10;
    counter = Counter(0.10f);
    clBufferImage = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, this->bufferSize * sizeof(Uint32), this->image.pixels(), nullptr);
}

void ShaderZombie::update()
{
    Shader::update();
    counter.update();

    if (counter.isFinished()) {
        counter.setEnabled(true);
        updateOffsets();
    }
    const int numTriangles = (int) this->meshOpenCLRenderer->getOclTriangles().size();
    if (numTriangles <= 0) return;

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&object->getOpenClRenderer()->clBufferStencil);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&EngineBuffers::get()->depthBufferOCL);
    clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)&clBufferImage);
    clSetKernelArg(kernel, 7, sizeof(int), &offsetX);
    clSetKernelArg(kernel, 8, sizeof(int), &offsetY);

    size_t global_item_size = bufferSize;
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);
    debugKernel("ShaderZombie");
}

void ShaderZombie::updateOffsets()
{
    offsetX = Tools::random(5, 10);
    offsetY = Tools::random(5, 10);
}
