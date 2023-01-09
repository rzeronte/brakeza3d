#include "ShaderLaser.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"


ShaderLaser::ShaderLaser() : ShaderOpenCL("laser.opencl")
{
    setIntensity(0.0);
    setReach(50000);
}

void ShaderLaser::update()
{
    Shader::update();

    if (!isEnabled()) return;

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

    auto player = ComponentsManager::get()->getComponentGame()->getPlayer();
    Vertex3D start = player->getPosition() + player->AxisDown().getScaled(1000);

    Point2D screenPoint = Transforms::WorldToPoint(start, ComponentsManager::get()->getComponentCamera()->getCamera());

    Vertex3D end = player->getPosition() + player->AxisDown().getScaled(getReach()).getScaled(getIntensity());
    Point2D middlePoint = Transforms::WorldToPoint(end, ComponentsManager::get()->getComponentCamera()->getCamera());

    // Crea una instancia de btCollisionWorld::ClosestRayResultCallback
    btCollisionWorld::ClosestRayResultCallback rayCallback(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z)
    );

    // Realiza la prueba de colisión utilizando la función rayTest
    ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->rayTest(
        btVector3(start.x, start.y, start.z),
        btVector3(end.x, end.y, end.z),
        rayCallback
    );

    // Si se produjo una colisión, imprime la información sobre la misma
    if (rayCallback.hasHit()) {
        auto *brkObjectA = (Collisionable *) rayCallback.m_collisionObject->getUserPointer();

        auto *enemy = dynamic_cast<EnemyGhost*> (brkObjectA);

        if (enemy != nullptr) {
            btVector3 rayHitPosition = rayCallback.m_hitPointWorld;
            auto hitPosition = Vertex3D(rayHitPosition.x(), rayHitPosition.y(), rayHitPosition.z());
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
    clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoOutput);
    clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoInput);
    clSetKernelArg(kernel, 5, sizeof(int), (void *)&screenPoint.x);
    clSetKernelArg(kernel, 6, sizeof(int), (void *)&screenPoint.y);
    clSetKernelArg(kernel, 7, sizeof(int), (void *)&middlePoint.x);
    clSetKernelArg(kernel, 8, sizeof(int), (void *)&middlePoint.y);

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
            openClBufferMappedWithVideoOutput,
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

void ShaderLaser::setReach(float reach) {
    ShaderLaser::reach = reach;
}

float ShaderLaser::getDamage() const {
    return damage;
}

void ShaderLaser::setDamage(float damage) {
    ShaderLaser::damage = damage;
}

