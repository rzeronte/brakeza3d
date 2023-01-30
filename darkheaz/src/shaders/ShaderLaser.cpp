#include "ShaderLaser.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"

ShaderLaser::ShaderLaser() : ShaderOpenCL("laser.opencl")
{
    setIntensity(0.0);
    setReach(0);
    setColor(Color::fuchsia());

    this->image = new Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png");

    opencl_buffer_pixels_image = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        this->bufferSize * sizeof(Uint32),
        this->image->pixels(),
        &clRet
    );

    clEnqueueWriteBuffer(
        clCommandQueue,
        opencl_buffer_pixels_image,
        CL_TRUE,
        0,
        this->bufferSize * sizeof(Uint32),
        image->pixels(),
        0,
        nullptr,
        nullptr
    );
}

void ShaderLaser::update()
{
    Shader::update();

    if (!isEnabled()) return;

    increaseReach();

    executeKernelOpenCL();
}

void ShaderLaser::executeKernelOpenCL()
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

    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->rayTest(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z),
        rayCallback
    );

    if (rayCallback.hasHit()) {
        auto *brkObjectA = (Collisionable *) rayCallback.m_collisionObject->getUserPointer();

        auto *enemy = dynamic_cast<EnemyGhost*> (brkObjectA);
        auto *player = dynamic_cast<Player*> (brkObjectA);

        btVector3 rayHitPosition = rayCallback.m_hitPointWorld;
        auto hitPosition = Vertex3D(rayHitPosition.x(), rayHitPosition.y(), rayHitPosition.z());

        if (player != nullptr) {
            middlePoint = Transforms::WorldToPoint(hitPosition, ComponentsManager::get()->getComponentCamera()->getCamera());
            player->takeDamage(getDamage());
            Brakeza3D::get()->addObject3D(
                new ParticleEmissorFireworks(hitPosition, Vertex3D(0, 4, 5), true, 520, 3, 0.02, Color::green(), 2, 2),
                "fireworks" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel()
            );
        }

        if (enemy != nullptr) {
            middlePoint = Transforms::WorldToPoint(hitPosition, ComponentsManager::get()->getComponentCamera()->getCamera());
            enemy->takeDamage(getDamage());
            Brakeza3D::get()->addObject3D(
                new ParticleEmissorFireworks(hitPosition, Vertex3D(0, 4, 5), true, 520, 3, 0.02, Color::green(), 2, 2),
                "fireworks" + ComponentsManager::get()->getComponentRender()->getUniqueGameObjectLabel()
            );
        }
    }

    clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
    clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
    clSetKernelArg(kernel, 2, sizeof(float), &Brakeza3D::get()->executionTime);
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

float ShaderLaser::getIntensity() const {
    return intensity;
}

void ShaderLaser::setIntensity(float value) {
    ShaderLaser::intensity = value;
}

float ShaderLaser::getReach() const {
    return reach;
}

void ShaderLaser::setReach(float value) {
    ShaderLaser::reach = value;
}

float ShaderLaser::getDamage() const {
    return damage;
}

void ShaderLaser::setDamage(float value) {
    ShaderLaser::damage = value;
}

void ShaderLaser::increaseReach() {
    reach = reach + 1 * speed;
}

float ShaderLaser::getSpeed() const {
    return speed;
}

void ShaderLaser::setSpeed(float value) {
    ShaderLaser::speed = value;
}

Object3D *ShaderLaser::getTarget() const {
    return target;
}

void ShaderLaser::setTarget(Object3D *target) {
    ShaderLaser::target = target;
}

const Color &ShaderLaser::getColor() const {
    return color;
}

void ShaderLaser::setColor(const Color &color) {
    ShaderLaser::color = color;
}

