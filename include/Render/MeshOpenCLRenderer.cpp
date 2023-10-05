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
    loaded(false)
{
    //createBuffers();
}

void MeshOpenCLRenderer::createBuffers()
{
    loaded = true;
    clBufferTriangles = clCreateBuffer(context, CL_MEM_READ_WRITE, MAX_OPENCL_TRIANGLES * sizeof(OCTriangle), nullptr, nullptr);
    clBufferMeshContext = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(OCLMeshContext), nullptr, nullptr );
    clBufferStencil = clCreateBuffer(context, CL_MEM_READ_WRITE, EngineBuffers::get()->sizeBuffers * sizeof(bool), nullptr, nullptr );
}

void MeshOpenCLRenderer::onUpdate(Image *texture)
{
    if (!object->isEnabled() || !isLoaded()) return;

    auto componentCamera = ComponentsManager::get()->getComponentCamera();
    meshContext = OCLMeshContext(
        ObjectData(
            OCVertex3D(object->getPosition().x, object->getPosition().y, object->getPosition().z),
            OCVertex3D(object->getRotation().getPitch(), object->getRotation().getYaw(), object->getRotation().getRoll()),
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

    const bool useStencil = object->isStencilBufferEnabled();
    const auto componentRender = ComponentsManager::get()->getComponentRender();
    auto kernel = componentRender->getRendererKernel();

    if (useStencil) {
        cl_int pattern = 0;
        clEnqueueFillBuffer(clQueue, clBufferStencil, &pattern, sizeof(cl_bool), 0, EngineBuffers::get()->sizeBuffers, 0, nullptr, nullptr);
    }

    auto numLights = ComponentsManager::get()->getComponentRender()->getLightPoints().size();

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

    size_t global_item_size[2] = {640, 16};
    size_t local_item_size[2] = {16, 16};

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);

    debugKernel();
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
