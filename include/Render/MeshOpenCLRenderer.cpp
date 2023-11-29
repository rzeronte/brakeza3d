//
// Created by eduardo on 20/04/23.
//

#include "MeshOpenCLRenderer.h"
#include "../ComponentsManager.h"
#include "Logging.h"


MeshOpenCLRenderer::MeshOpenCLRenderer(Object3D *parent, std::vector<Triangle*> &triangles)
:
    clQueue(ComponentsManager::get()->getComponentRender()->getClCommandQueue()),
    clDeviceId(ComponentsManager::get()->getComponentRender()->getClDeviceId()),
    clRet(0),
    context(ComponentsManager::get()->getComponentRender()->getClContext()),
    triangles(triangles),
    object(parent),
    loaded(false),
    numTrianglesClipped(0)
{
    //createBuffers();
}

void MeshOpenCLRenderer::createBuffers()
{
    loaded = true;
    clBufferTriangles = clCreateBuffer(context, CL_MEM_WRITE_ONLY, MAX_OPENCL_TRIANGLES * sizeof(OCTriangle), nullptr, nullptr);
    clBufferMeshContext = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(OCLMeshContext), nullptr, nullptr );
    clBufferStencil = clCreateBuffer(context, CL_MEM_READ_WRITE, EngineBuffers::get()->sizeBuffers * sizeof(bool), nullptr, nullptr );
}

void MeshOpenCLRenderer::onUpdate(Image *texture)
{
    if (!object->isEnabled() || !isLoaded()) return;

    auto componentCamera = ComponentsManager::get()->getComponentCamera();
    auto p = object->getPosition();
    auto r = object->getRotation();
    meshContext = OCLMeshContext(
        ObjectData(
            OCVertex3D(p.x, p.y, p.z),
            OCVertex3D(r.getPitch(), r.getYaw(), r.getRoll()),
            object->getScale(),
            object->isEnableLights()
        ),
        componentCamera->getCameraData(),
        componentCamera->getFrustumData()
    );

    const int numTriangles = (int) oclTriangles.size();
    if (numTriangles <= 0) return;

    //makeOCLTriangles();
    //clEnqueueWriteBuffer(clQueue, clBufferTriangles, CL_TRUE, 0, numTriangles * sizeof(OCTriangle), oclTriangles.data(), 0, nullptr, nullptr);
    clEnqueueWriteBuffer(clQueue, clBufferMeshContext, CL_TRUE, 0, sizeof(OCLMeshContext), &meshContext, 0, nullptr, nullptr);

    renderTriangles(texture);
    renderClippedTriangles(texture);
    //renderFragments(texture);
}

MeshOpenCLRenderer::~MeshOpenCLRenderer()
{
    if (isLoaded()) {
        clReleaseMemObject(clBufferTriangles);
        clReleaseMemObject(clBufferMeshContext);
        clReleaseMemObject(clBufferStencil);
    }
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
    createBuffers();
    oclTriangles.clear();

    for (auto t : this->triangles) {
        oclTriangles.emplace_back(OCVertex3D(t->A), OCVertex3D(t->B), OCVertex3D(t->C));
    }

    clEnqueueWriteBuffer(clQueue, clBufferTriangles, CL_TRUE, 0, triangles.size() * sizeof(OCTriangle), oclTriangles.data(), 0, nullptr, nullptr);

    indicesClipping = new int[this->triangles.size()];
    clBufferIndicesClipping = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(int) * this->triangles.size(), indicesClipping, nullptr);
}


cl_mem *MeshOpenCLRenderer::getClBufferTriangles(){
    return &clBufferTriangles;
}

cl_mem *MeshOpenCLRenderer::getClBufferMeshContext(){
    return &clBufferMeshContext;
}

const std::vector<OCTriangle> &MeshOpenCLRenderer::getOclTriangles() const {
    return oclTriangles;
}

bool MeshOpenCLRenderer::isLoaded() const {
    return loaded;
}

cl_mem *MeshOpenCLRenderer::getClBufferStencil() {
    return &clBufferStencil;
}


void MeshOpenCLRenderer::renderTriangles(Image *texture)
{
    const int numTriangles = (int) oclTriangles.size();

    const bool useStencil = object->isStencilBufferEnabled();
    const auto componentRender = ComponentsManager::get()->getComponentRender();
    auto kernel = componentRender->getRendererKernel();
    auto numLights = ComponentsManager::get()->getComponentRender()->getLightPoints().size();

    //if (useStencil) {
    cl_int pattern = 0;
    clEnqueueFillBuffer(clQueue, clBufferStencil, &pattern, sizeof(cl_bool), 0, EngineBuffers::get()->sizeBuffers, 0, nullptr, nullptr);
    //}

    numTrianglesClipped = 0;
    clBufferNumTrianglesClipped = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, sizeof(int), &numTrianglesClipped, nullptr);

    int useClipping = 1;
    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->depthBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&clBufferMeshContext);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&clBufferTriangles);
    clSetKernelArg(kernel, 6, sizeof(int), &numTriangles);
    clSetKernelArg(kernel, 7, sizeof(cl_mem), (void *)texture->getOpenClTexture());
    clSetKernelArg(kernel, 8, sizeof(int), &texture->getSurface()->w);
    clSetKernelArg(kernel, 9, sizeof(int), &texture->getSurface()->h);
    clSetKernelArg(kernel, 10, sizeof(cl_mem), (void *)&clBufferStencil);
    clSetKernelArg(kernel, 11, sizeof(int), &useStencil);
    clSetKernelArg(kernel, 12, sizeof(cl_mem), (void *)componentRender->getClBufferLights());
    clSetKernelArg(kernel, 13, sizeof(int), &numLights);
    clSetKernelArg(kernel, 14, sizeof(cl_mem), (void *)&clBufferIndicesClipping);
    clSetKernelArg(kernel, 15, sizeof(cl_mem), (void *)&clBufferNumTrianglesClipped);
    clSetKernelArg(kernel, 16, sizeof(int), &useClipping);

    size_t global_item_size[2] = {640, 16};
    size_t local_item_size[2] = {1, 1};

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);
    debugKernel();
}

void MeshOpenCLRenderer::renderClippedTriangles(Image *texture)
{
    const bool useStencil = object->isStencilBufferEnabled();
    const auto componentRender = ComponentsManager::get()->getComponentRender();
    auto kernel = componentRender->getRendererKernel();
    auto numLights = ComponentsManager::get()->getComponentRender()->getLightPoints().size();

    auto cam = ComponentsManager::get()->getComponentCamera()->getCamera();
    auto SETUP = EngineSetup::get();
    std::vector<Triangle *> clippedTriangles;

    clEnqueueReadBuffer(clQueue, clBufferNumTrianglesClipped, CL_TRUE, 0, sizeof(int), &numTrianglesClipped, 0, NULL, NULL);

    if (numTrianglesClipped > 0){
        clEnqueueReadBuffer(clQueue, clBufferIndicesClipping, CL_TRUE, 0, sizeof(int) * this->triangles.size(), indicesClipping, 0, NULL, NULL);

        for (int h = 0; h < numTrianglesClipped; h++) {
            int i = indicesClipping[h];

            auto t = triangles[i];
            t->updateObjectSpace();
            t->updateNormal();
            t->updateCameraSpace(cam);
            t->updatePerspectiveNDCSpace(cam->getFrustum());
            t->updateScreenSpace();
            t->updateBoundingBox();
            t->updateFullArea();
            t->updateUVCache();
            t->clipping(
                cam->getFrustum(),
                cam->getFrustum()->planes,
                SETUP->LEFT_PLANE,
                SETUP->BOTTOM_PLANE,
                triangles[h]->parent,
                clippedTriangles
            );
        }

        std::vector<OCTriangle> oclClippedTriangles;
        for (auto t : clippedTriangles) {
            oclClippedTriangles.emplace_back(OCVertex3D(t->A), OCVertex3D(t->B), OCVertex3D(t->C));
        }

        cl_mem clBufferClippedTriangles = clCreateBuffer(context, CL_MEM_READ_WRITE, clippedTriangles.size() * sizeof(OCTriangle), nullptr, nullptr);
        clEnqueueWriteBuffer(clQueue, clBufferClippedTriangles, CL_TRUE, 0, clippedTriangles.size() * sizeof(OCTriangle), oclClippedTriangles.data(), 0, nullptr, nullptr);

        int numClippedTriangles = (int) clippedTriangles.size();
        int useClipping = 0;
        clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
        clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
        clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->depthBufferOCL);
        clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&clBufferMeshContext);
        clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&clBufferClippedTriangles);
        clSetKernelArg(kernel, 6, sizeof(int), &numClippedTriangles);
        clSetKernelArg(kernel, 7, sizeof(cl_mem), (void *)texture->getOpenClTexture());
        clSetKernelArg(kernel, 8, sizeof(int), &texture->getSurface()->w);
        clSetKernelArg(kernel, 9, sizeof(int), &texture->getSurface()->h);
        clSetKernelArg(kernel, 10, sizeof(cl_mem), (void *)&clBufferStencil);
        clSetKernelArg(kernel, 11, sizeof(int), &useStencil);
        clSetKernelArg(kernel, 12, sizeof(cl_mem), (void *)componentRender->getClBufferLights());
        clSetKernelArg(kernel, 13, sizeof(int), &numLights);
        clSetKernelArg(kernel, 14, sizeof(cl_mem), (void *)&clBufferIndicesClipping);
        clSetKernelArg(kernel, 15, sizeof(cl_mem), (void *)&clBufferNumTrianglesClipped);
        clSetKernelArg(kernel, 16, sizeof(int), &useClipping);

        size_t global_item_size[2] = {1024, 16};
        size_t local_item_size[2] = {16, 16};

        clRet = clEnqueueNDRangeKernel(clQueue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);
        debugKernel();
    }
}

void MeshOpenCLRenderer::renderFragments(Image *texture)
{
    const auto componentRender = ComponentsManager::get()->getComponentRender();
    auto kernel = componentRender->getFragmentsKernel();
    const bool useStencil = object->isStencilBufferEnabled();
    auto numLights = ComponentsManager::get()->getComponentRender()->getLightPoints().size();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->depthBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&this->clBufferMeshContext);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)texture->getOpenClTexture());
    clSetKernelArg(kernel, 6, sizeof(int), &texture->getSurface()->w);
    clSetKernelArg(kernel, 7, sizeof(int), &texture->getSurface()->h);
    clSetKernelArg(kernel, 8, sizeof(cl_mem), (void *)&clBufferStencil);
    clSetKernelArg(kernel, 9, sizeof(int), &useStencil);
    clSetKernelArg(kernel, 10, sizeof(cl_mem), (void *)componentRender->getClBufferLights());
    clSetKernelArg(kernel, 11, sizeof(int), &numLights);
    clSetKernelArg(kernel, 12, sizeof(cl_mem), (void *)&componentRender->clBufferFragments);

    size_t global_item_size2[2] = {(size_t) EngineSetup::get()->screenWidth, (size_t) EngineSetup::get()->screenHeight};
    size_t local_item_size2[2] = {32, 32};

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 2, NULL, global_item_size2, local_item_size2, 0, NULL, NULL);
    debugKernel();
}

