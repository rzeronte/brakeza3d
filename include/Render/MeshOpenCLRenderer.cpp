//
// Created by eduardo on 20/04/23.
//

#include "MeshOpenCLRenderer.h"
#include "../ComponentsManager.h"


MeshOpenCLRenderer::MeshOpenCLRenderer(Object3D *parent, std::vector<Triangle*> &triangles)
:
    clQueue(ComponentsManager::get()->getComponentRender()->getClCommandQueue()),
    clDeviceId(ComponentsManager::get()->getComponentRender()->getClDeviceId()),
    context(ComponentsManager::get()->getComponentRender()->getClContext()),
    triangles(triangles),
    object(parent)
{
    clBufferTriangles = clCreateBuffer(this->context, CL_MEM_READ_WRITE, MAX_OPENCL_TRIANGLES * sizeof(OCTriangle), nullptr, nullptr);
    clBufferMeshContext = clCreateBuffer(context, CL_MEM_READ_WRITE, sizeof(OCLMeshContext), nullptr, nullptr );
    clBufferStencil = clCreateBuffer(context, CL_MEM_READ_WRITE, EngineBuffers::get()->sizeBuffers * sizeof(bool), nullptr, nullptr );
}

void MeshOpenCLRenderer::onUpdate(OCLMeshContext *context, Texture *texture)
{
    auto oclTriangles = openCLTriangles();

    int numTriangles = (int) oclTriangles.size();
    if (numTriangles <= 0) return;

    clEnqueueWriteBuffer(clQueue, clBufferTriangles, CL_TRUE, 0, numTriangles * sizeof(OCTriangle), oclTriangles.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(clQueue, clBufferMeshContext, CL_TRUE, 0, sizeof(OCLMeshContext), context, 0, nullptr, nullptr);

    bool useStencil = object->isStencilBufferEnabled();
    auto kernel = ComponentsManager::get()->getComponentRender()->getRendererKernel();

    if (object->isStencilBufferEnabled()) {
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

    size_t global_item_size = MAX_OPENCL_TRIANGLES;
    size_t local_item_size = 512;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);

    debugKernel();
}

MeshOpenCLRenderer::~MeshOpenCLRenderer()
{
    if (clBufferTriangles != nullptr) clReleaseMemObject(clBufferTriangles);
    if (clBufferMeshContext != nullptr) clReleaseMemObject(clBufferMeshContext);
    if (clBufferStencil != nullptr) clReleaseMemObject(clBufferStencil);
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


std::vector<OCTriangle> MeshOpenCLRenderer::openCLTriangles()
{
    Camera3D *cam = ComponentsManager::get()->getComponentCamera()->getCamera();

    std::vector<OCTriangle> openCLTriangles;
    for (auto &t : this->triangles) {
        /*t->updateObjectSpace();
        t->updateNormal();
        t->updateCameraSpace(cam);
        t->updatePerspectiveNDCSpace(cam->getFrustum());
        t->updateScreenSpace();
        t->updateBoundingBox();
        t->updateFullArea();
        t->updateUVCache();*/

        openCLTriangles.emplace_back(OCTriangle(
            Tools::vertexOCL(t->A), Tools::vertexOCL(t->B), Tools::vertexOCL(t->C),
            Tools::vertexOCL(t->Ao), Tools::vertexOCL(t->Bo), Tools::vertexOCL(t->Co),
            Tools::vertexOCL(t->Ac), Tools::vertexOCL(t->Bc), Tools::vertexOCL(t->Cc),
            Tools::vertexOCL(t->An), Tools::vertexOCL(t->Bn), Tools::vertexOCL(t->Cn),
            Tools::pointOCL(t->As), Tools::pointOCL(t->Bs), Tools::pointOCL(t->Cs),
            t->fullArea, t->reciprocalFullArea,
            Tools::vertexOCL(t->normal),
            t->tex_u1_Ac_z, t->tex_u2_Bc_z, t->tex_u3_Cc_z,
            t->tex_v1_Ac_z, t->tex_v2_Bc_z, t->tex_v3_Cc_z,
            t->persp_correct_Az, t->persp_correct_Bz, t->persp_correct_Cz,
            t->maxX, t->minX, t->maxY, t->minY,
            t->clipped
        ));
    }

    return openCLTriangles;
}