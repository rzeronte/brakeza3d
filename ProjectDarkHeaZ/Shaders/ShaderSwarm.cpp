
#include "ShaderSwarm.h"
#include "../../include/Brakeza3D.h"

ShaderSwarm::ShaderSwarm(bool active, Vertex3D center, int numElements, OCSwarmContext swarmContext)
:
    ShaderOpenCL(active, "swarm.cl"),
    center(center),
    numberElements(numElements)
{
    initBoids();
    openCLBufferBoids = clCreateBuffer(context, CL_MEM_READ_WRITE, MAX_SWARM_OBJETS * sizeof(OCBoid), this->boids.data(), nullptr);
    openCLBufferSwarmContext = clCreateBuffer(context, CL_MEM_READ_WRITE, 1 * sizeof(OCSwarmContext), nullptr, nullptr);
}

void ShaderSwarm::update()
{
    Shader::update();
    executeKernelOpenCL();
}

void ShaderSwarm::executeKernelOpenCL()
{
    auto swarmContext = ShaderSwarm::openCLSwarmContext();
    clEnqueueWriteBuffer(clQueue, openCLBufferSwarmContext, CL_TRUE, 0, 1 * sizeof(OCSwarmContext), &swarmContext, 0, nullptr, nullptr);

    float dt = Brakeza3D::get()->getDeltaTime();

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &dt);
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&openCLBufferSwarmContext);
    clSetKernelArg(kernel, 5, sizeof(cl_mem), (void *)&openCLBufferBoids);
    clSetKernelArg(kernel, 6, sizeof(int), &numberElements);

    size_t global_item_size = MAX_SWARM_OBJETS;
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr);

    debugKernel("ShaderSwarm");
}


OCSwarmContext ShaderSwarm::openCLSwarmContext()
{
    auto cam = ComponentsManager::get()->getComponentCamera()->getCamera();
    auto rp = cam->getRotation();

    auto frustum = cam->getFrustum();

    std::vector<OCLPlane> planesOCL;
    for (int i = EngineSetup::get()->NEAR_PLANE ; i <= EngineSetup::get()->BOTTOM_PLANE ; i++) {
        OCVertex3D A( frustum->planes[i].A.x, frustum->planes[i].A.y, frustum->planes[i].A.z );
        OCVertex3D B( frustum->planes[i].B.x, frustum->planes[i].B.y, frustum->planes[i].B.z );
        OCVertex3D C( frustum->planes[i].C.x, frustum->planes[i].C.y, frustum->planes[i].C.z );
        OCVertex3D normal( frustum->planes[i].normal.x, frustum->planes[i].normal.y, frustum->planes[i].normal.z );

        planesOCL.emplace_back(A, B, C, normal);
    }

    return OCSwarmContext(
        1, 1 , 1,
        CameraData(
            OCVertex3D(cam->getPosition().x, cam->getPosition().y, cam->getPosition().z),
            OCVertex3D(rp.getPitch(), rp.getYaw(), rp.getRoll())
        ),
        FrustumData(
            OCVertex3D(frustum->vNLs),
            OCVertex3D(frustum->vNRs),
            OCVertex3D(frustum->vNTs),
            OCVertex3D(frustum->vNBs),
            planesOCL
        )
    );
}

void ShaderSwarm::initBoids()
{
    for (int i = 0; i < this->numberElements; i++) {
        auto velocity = Tools::randomVertex().getNormalize();
        auto position = center + Tools::randomVertex().getNormalize().getScaled(100);

        this->boids.emplace_back(
            Tools::vertexOCL(position),
            Tools::vertexOCL(velocity),
            OCVertex3D(255, 0, 0)
        );
    }
}
