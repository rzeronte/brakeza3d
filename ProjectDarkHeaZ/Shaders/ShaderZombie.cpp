//
// Created by eduardo on 5/06/23.
//

#include "ShaderZombie.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Brakeza3D.h"

ShaderZombie::ShaderZombie(bool active, const std::string& filename,Mesh3D* object, MeshOpenCLRenderer *renderer)
:
    ShaderOpenCL(active, "zombie.cl"),
    meshOpenCLRenderer(renderer),
    object(object),
    offsetX(10),
    offsetY(10),
    counter(Counter(0.10f)),
    image(Image(filename))
{
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
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)object->getOpenClRenderer()->getClBufferStencil());
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&EngineBuffers::get()->depthBufferOCL);
    clSetKernelArg(kernel, 6, sizeof(cl_mem), (void *)image.getOpenClTexture());
    clSetKernelArg(kernel, 7, sizeof(int), &offsetX);
    clSetKernelArg(kernel, 8, sizeof(int), &offsetY);

    size_t global_item_size = bufferSize;
    size_t local_item_size = 12;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);
}

void ShaderZombie::updateOffsets()
{
    offsetX = Tools::random(5, 10);
    offsetY = Tools::random(5, 10);
}
