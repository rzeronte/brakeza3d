//
// Created by eduardo on 20/04/23.
//

#include "MeshOpenCLRenderer.h"
#include "../ComponentsManager.h"


MeshOpenCLRenderer::MeshOpenCLRenderer(Object3D *parent, std::vector<Triangle*> &triangles)
:
    clQueue(ComponentsManager::get()->getComponentRender()->getClCommandQueue()),
    clDeviceId(ComponentsManager::get()->getComponentRender()->getClDeviceId()),
    clRet(0),
    context(ComponentsManager::get()->getComponentRender()->getClContext()),
    triangles(triangles),
    object(parent)
{
    clBufferTriangles = clCreateBuffer(context, CL_MEM_READ_WRITE, MAX_OPENCL_TRIANGLES * sizeof(OCTriangle), nullptr, nullptr);
    clBufferMeshContext = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(OCLMeshContext), nullptr, nullptr );
    clBufferStencil = clCreateBuffer(context, CL_MEM_READ_ONLY, EngineBuffers::get()->sizeBuffers * sizeof(bool), nullptr, nullptr );
    clBufferLights = clCreateBuffer(context, CL_MEM_READ_WRITE, MAX_OPENCL_LIGHTS * sizeof(OCLight), nullptr, nullptr);
}

void MeshOpenCLRenderer::onUpdate(Texture *texture)
{
    meshContext = Tools::openCLContext(object);

    updateLights();

    const int numTriangles = (int) oclTriangles.size();
    if (numTriangles <= 0) return;

    int numLights = (int) oclLights.size();

    clEnqueueWriteBuffer(clQueue, clBufferTriangles, CL_TRUE, 0, numTriangles * sizeof(OCTriangle), oclTriangles.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(clQueue, clBufferMeshContext, CL_TRUE, 0, sizeof(OCLMeshContext), &meshContext, 0, nullptr, nullptr);
    clEnqueueWriteBuffer(clQueue, clBufferLights, CL_TRUE, 0, numLights * sizeof(OCLight), oclLights.data(), 0, nullptr, nullptr);

    const bool useStencil = object->isStencilBufferEnabled();
    auto kernel = ComponentsManager::get()->getComponentRender()->getRendererKernel();

    if (useStencil) {
        cl_int pattern = 0;
        clEnqueueFillBuffer(clQueue, clBufferStencil, &pattern, sizeof(cl_bool), 0, EngineBuffers::get()->sizeBuffers, 0, nullptr, nullptr);
    }

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->depthBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&clBufferMeshContext);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&clBufferTriangles);
    clSetKernelArg(kernel, 6, sizeof(int), &numTriangles);
    clSetKernelArg(kernel, 7, sizeof(cl_mem), (void *)&texture->getImage()->openClTexture);
    clSetKernelArg(kernel, 8, sizeof(int), &texture->getImage()->getSurface()->w);
    clSetKernelArg(kernel, 9, sizeof(int), &texture->getImage()->getSurface()->h);
    clSetKernelArg(kernel, 10, sizeof(cl_mem), (void *)&clBufferStencil);
    clSetKernelArg(kernel, 11, sizeof(int), &useStencil);
    clSetKernelArg(kernel, 12, sizeof(cl_mem), (void *)&clBufferLights);
    clSetKernelArg(kernel, 13, sizeof(int), &numLights);

    size_t global_item_size = MAX_OPENCL_TRIANGLES;
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);
    //debugKernel();
}

MeshOpenCLRenderer::~MeshOpenCLRenderer()
{
    if (clBufferTriangles != nullptr) clReleaseMemObject(clBufferTriangles);
    if (clBufferMeshContext != nullptr) clReleaseMemObject(clBufferMeshContext);
    if (clBufferStencil != nullptr) clReleaseMemObject(clBufferStencil);
    if (clBufferLights != nullptr) clReleaseMemObject(clBufferLights);
}

void MeshOpenCLRenderer::debugKernel() const
{
    if (!EngineSetup::get()->LOGGING) return;

    if (clRet != CL_SUCCESS) {
        Logging::Log("MeshOpenCLRenderer Error OpenCL kernel: %d",  clRet);

        char buffer[2048];
        clGetProgramBuildInfo(
            ComponentsManager::get()->getComponentRender()->getRendererProgram(),
            clDeviceId,
            CL_PROGRAM_BUILD_LOG,
            sizeof(buffer),
            buffer,
            nullptr
        );

        if (strlen(buffer) > 0 ) {
            Logging::Log( buffer );
        }
    }
}

void MeshOpenCLRenderer::updateTriangles()
{
    for (int i = 0; i < (int) triangles.size(); i++) {
        oclTriangles[i].A = triangles[i]->A;
        oclTriangles[i].B = triangles[i]->B;
        oclTriangles[i].C = triangles[i]->C;
    }
}

void MeshOpenCLRenderer::makeOCLTriangles()
{
    oclTriangles.clear();

    for (auto t : this->triangles) {
        oclTriangles.emplace_back(OCVertex3D(t->A), OCVertex3D(t->B), OCVertex3D(t->C));
    }
}

void MeshOpenCLRenderer::updateLights()
{
    oclLights.clear();

    for (auto l : ComponentsManager::get()->getComponentRender()->getLightPoints()) {
        auto forward = l->AxisForward();
        auto ocl = OCLight(
            Tools::vertexOCL(l->getPosition()),
            Tools::vertexOCL(forward),
            l->p,
            l->kc,
            l->kl,
            l->kq,
            l->specularComponent,
            l->color.getColor(),
            l->specularColor.getColor()
        );
        oclLights.emplace_back(ocl);
    }
}

