//
// Created by eduardo on 10/05/23.
//

#include "../../include/Shaders/ShaderBilinear.h"
#include "../../include/EngineSetup.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Brakeza3D.h"

ShaderBilinear::ShaderBilinear(bool active) : ShaderOpenCL(active, "bilinear.cl")
{
    videoOutputBufferOCL = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
        EngineBuffers::get()->sizeBuffers * sizeof(Uint32),
        videoBuffer,
        nullptr
    );
}

void ShaderBilinear::update()
{
    Shader::update();
    executeKernelOpenCL();
}

void ShaderBilinear::executeKernelOpenCL()
{

    clEnqueueCopyBuffer(
        clQueue,
        EngineBuffers::get()->videoBufferOCL, // source buffer
        videoOutputBufferOCL, // destination buffer
        0, // source offset
        0, // destination offset
        this->bufferSize * sizeof(Uint32), // size of data to copy
        0, nullptr, nullptr
    );
    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&videoOutputBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);

    size_t global_item_size[2] = {640, 480}; // Tamaño global de trabajo (ancho x alto)
    size_t local_item_size[2] = {16, 16};    // Tamaño local de trabajo (puede ajustarse según la GPU)

    clEnqueueNDRangeKernel(clQueue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);


    debugKernel("ShaderBilinear");
}
