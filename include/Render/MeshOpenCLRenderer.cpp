//
// Created by eduardo on 20/04/23.
//

#include "MeshOpenCLRenderer.h"
#include "../ComponentsManager.h"


MeshOpenCLRenderer::MeshOpenCLRenderer(const std::string& kernelFilename, std::vector<Triangle*> &triangles)
:
        kernelFilename(kernelFilename),
        clQueue(ComponentsManager::get()->getComponentRender()->getClCommandQueue()),
        clDeviceId(ComponentsManager::get()->getComponentRender()->getClDeviceId()),
        context(ComponentsManager::get()->getComponentRender()->getClContext()),
        triangles(triangles)
{
    initOpenCLProgram();

    Logging::Message("Loading '%s' kernel", kernelFilename.c_str());
}

void MeshOpenCLRenderer::initOpenCLProgram()
{
    size_t source_size;
    char * source_str = Tools::readFile(
        EngineSetup::get()->ROOT_FOLDER + this->kernelFilename,
        source_size
    );

    program = clCreateProgramWithSource(
        context,
        1,
        (const char **)&source_str,
        (const size_t *)&source_size,
        &clRet
    );

    clBuildProgram(program, 1, &this->clDeviceId, nullptr, nullptr, nullptr);

    kernel = clCreateKernel(program, "onUpdate", &clRet);

    clBufferTriangles = clCreateBuffer(
        this->context,
        CL_MEM_READ_WRITE,
        MAX_OPENCL_TRIANGLES * sizeof(OCTriangle),
        nullptr,
        nullptr
    );

    clBufferMeshContext = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        1 * sizeof(OCLMeshContext),
        nullptr,
        nullptr
    );
    free(source_str);
}

void MeshOpenCLRenderer::onUpdate(OCLMeshContext *context)
{
    auto oclTriangles = openCLTriangles();

    clEnqueueWriteBuffer(
        clQueue,
        EngineBuffers::getInstance()->openClVideoBuffer,
        CL_FALSE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        EngineBuffers::getInstance()->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    clEnqueueWriteBuffer(
        clQueue,
        EngineBuffers::getInstance()->openClDepthBuffer,
        CL_FALSE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(float),
        EngineBuffers::getInstance()->depthBuffer,
        0,
        nullptr,
        nullptr
    );

    int numTriangles = (int) oclTriangles.size();

    clEnqueueWriteBuffer(clQueue, clBufferTriangles, CL_TRUE, 0, numTriangles * sizeof(OCTriangle), oclTriangles.data(), 0, nullptr, nullptr );
    clEnqueueWriteBuffer(clQueue, clBufferMeshContext, CL_TRUE, 0, sizeof(OCLMeshContext), context, 0, nullptr, nullptr );

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&EngineBuffers::getInstance()->openClVideoBuffer);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::getInstance()->openClDepthBuffer);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&clBufferMeshContext);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&clBufferTriangles);
    clSetKernelArg(kernel, 6, sizeof(int), &numTriangles);

    // Process the entire lists
    size_t global_item_size = MAX_OPENCL_TRIANGLES;
    // Divide work items into groups of 64
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel( clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr );

    clEnqueueReadBuffer(
        clQueue,
        EngineBuffers::getInstance()->openClVideoBuffer,
        CL_FALSE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
        EngineBuffers::getInstance()->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    clEnqueueReadBuffer(
        clQueue,
        EngineBuffers::getInstance()->openClDepthBuffer,
        CL_FALSE,
        0,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(float),
        EngineBuffers::getInstance()->depthBuffer,
        0,
        nullptr,
        nullptr
    );
}

MeshOpenCLRenderer::~MeshOpenCLRenderer()
{
    clReleaseMemObject(clBufferTriangles);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
}


void MeshOpenCLRenderer::debugKernel() const
{
    if (!EngineSetup::get()->LOGGING) return;

    if (clRet != CL_SUCCESS) {
        Logging::Log( "Error OpenCL kernel: %d",  clRet);

        char buffer[2048];
        clGetProgramBuildInfo(
            program,
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
        t->updateObjectSpace();
        t->updateNormal();
        t->updateCameraSpace(cam);
        t->updatePerspectiveNDCSpace(cam->getFrustum());
        t->updateScreenSpace();
        t->updateBoundingBox();
        t->updateFullArea();
        t->updateUVCache();
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