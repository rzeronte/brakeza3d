#include "ShaderLightRay.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"

ShaderLightRay::ShaderLightRay(Object3D *parent, float speed, float damage, Color c, int filterGroup, int filterMask):
    ShaderOpenCL("laser.opencl"),
    intensity(0),
    reach(0),
    speed(speed),
    damage(damage),
    filterGroup(filterGroup),
    filterMask(filterMask),
    color(c),
    target(parent),
    parent(parent),
    image(Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png"))
{
    opencl_buffer_pixels_image = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        this->bufferSize * sizeof(Uint32),
        this->image.pixels(),
        nullptr
    );

    clEnqueueWriteBuffer(
        clCommandQueue,
        opencl_buffer_pixels_image,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        image.pixels(),
        0,
        nullptr,
        nullptr
    );
}

void ShaderLightRay::update()
{
    Shader::update();

    if (!isEnabled()) return;

    executeKernelOpenCL();
}

void ShaderLightRay::executeKernelOpenCL()
{
    clEnqueueWriteBuffer(
        clCommandQueue,
        openClBufferMappedWithVideoInput,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        EngineBuffers::getInstance()->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    Vertex3D start = target->getPosition() + target->AxisDown().getScaled(1000);
    Point2D screenPoint = Transforms::WorldToPoint(start, ComponentsManager::get()->getComponentCamera()->getCamera());

    Vertex3D end = target->getPosition() + target->AxisDown().getScaled(getReach());
    Point2D middlePoint = Transforms::WorldToPoint(end, ComponentsManager::get()->getComponentCamera()->getCamera());

    btCollisionWorld::ClosestRayResultCallback rayCallback(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z)
    );

    rayCallback.m_collisionFilterGroup = this->filterGroup;
    rayCallback.m_collisionFilterMask = this->filterMask;

    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->rayTest(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z),
        rayCallback
    );

    bool increase = true;

    if (rayCallback.hasHit()) {
        auto *brkObjectA = (Collisionable *) rayCallback.m_collisionObject->getUserPointer();

        auto *object = dynamic_cast<Object3D*> (brkObjectA);
        auto *enemy = dynamic_cast<EnemyGhost*> (brkObjectA);
        auto *player = dynamic_cast<Player*> (brkObjectA);

        if (object != this->parent) {
            if (player != nullptr) {
                btVector3 rayHitPosition = rayCallback.m_hitPointWorld;
                auto hitPosition = Vertex3D(rayHitPosition.x(), rayHitPosition.y(), rayHitPosition.z());
                middlePoint = Transforms::WorldToPoint(hitPosition, ComponentsManager::get()->getComponentCamera()->getCamera());

                player->takeDamage(getDamage());
                increase = false;
            }

            if (enemy != nullptr) {
                btVector3 rayHitPosition = rayCallback.m_hitPointWorld;
                auto hitPosition = Vertex3D(rayHitPosition.x(), rayHitPosition.y(), rayHitPosition.z());
                middlePoint = Transforms::WorldToPoint(hitPosition, ComponentsManager::get()->getComponentCamera()->getCamera());

                enemy->takeDamage(getDamage());
                increase = false;
            }
        }
    }

    if (increase){
        increaseReach();
    }

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->getExecutionTime());
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoInput);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&opencl_buffer_pixels_image);
    clSetKernelArg(kernel, 5, sizeof(int), (void *)&screenPoint.x);
    clSetKernelArg(kernel, 6, sizeof(int), (void *)&screenPoint.y);
    clSetKernelArg(kernel, 7, sizeof(int), (void *)&middlePoint.x);
    clSetKernelArg(kernel, 8, sizeof(int), (void *)&middlePoint.y);
    clSetKernelArg(kernel, 9, sizeof(float), (void *)&this->intensity);
    clSetKernelArg(kernel, 10, sizeof(float), (void *)&this->color.r);
    clSetKernelArg(kernel, 11, sizeof(float), (void *)&this->color.g);
    clSetKernelArg(kernel, 12, sizeof(float), (void *)&this->color.b);

    // Process the entire lists
    size_t global_item_size = this->bufferSize;
    // Divide work items into groups of 64
    size_t local_item_size = 64;

    clRet = clEnqueueNDRangeKernel(
        clCommandQueue,
        kernel,
        1,
        nullptr,
        &global_item_size,
        &local_item_size,
        0,
        nullptr,
        nullptr
    );

    clEnqueueReadBuffer(
        clCommandQueue,
        openClBufferMappedWithVideoInput,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        EngineBuffers::getInstance()->videoBuffer,
        0,
        nullptr,
        nullptr
    );

    this->debugKernel();
}

void ShaderLightRay::setIntensity(float value) {
    ShaderLightRay::intensity = value;
}

float ShaderLightRay::getReach() const {
    return reach;
}

void ShaderLightRay::resetReach() {
    this->reach = 0;
}

float ShaderLightRay::getDamage() const {
    return damage;
}

void ShaderLightRay::setDamage(float value) {
    ShaderLightRay::damage = value;
}

void ShaderLightRay::increaseReach() {
    reach = reach + 1 * speed;
}

float ShaderLightRay::getSpeed() const {
    return speed;
}

void ShaderLightRay::setSpeed(float value) {
    ShaderLightRay::speed = value;
}

Object3D *ShaderLightRay::getTarget() const {
    return target;
}

void ShaderLightRay::setTarget(Object3D *object) {
    ShaderLightRay::target = object;
}

const Color &ShaderLightRay::getColor() const {
    return color;
}

void ShaderLightRay::setColor(const Color &c) {
    ShaderLightRay::color = c;
}

ShaderLightRay::~ShaderLightRay()
{
    clReleaseMemObject(opencl_buffer_pixels_image);
}

