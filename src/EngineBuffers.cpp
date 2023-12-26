
#include <SDL2/SDL_surface.h>
#include "../include/EngineBuffers.h"
#include "../include/ComponentsManager.h"
#include "../include/Shaders/ShaderExplosion.h"
#include "LUAIntegration.h"

inline void my_panic(sol::optional<std::string> maybe_msg) {
    const std::string& msg = maybe_msg.value();
    throw std::runtime_error(   msg);
}

EngineBuffers *EngineBuffers::instance = nullptr;

EngineBuffers *EngineBuffers::get()
{
    if (instance == nullptr) {
        instance = new EngineBuffers();
    }

    return instance;
}

EngineBuffers::EngineBuffers()
{
    EngineSetup *setup = EngineSetup::get();

    sizeBuffers = setup->RESOLUTION;
    depthBuffer = new float[sizeBuffers];
    videoBuffer = new Uint32[sizeBuffers];

    loadParticlesEmptyBuffer();

    sharedLuaContext = new SharedLUAContext();
}

void EngineBuffers::clearDepthBuffer() const {
    std::fill(depthBuffer, depthBuffer + sizeBuffers, 50000);
}

float EngineBuffers::getDepthBuffer(int x, int y) const {
    return depthBuffer[y * this->widthVideoBuffer + x];
}

float EngineBuffers::getDepthBuffer(int i) const {
    return depthBuffer[i];
}

void EngineBuffers::setDepthBuffer(int x, int y, float value) const {
    depthBuffer[y * this->widthVideoBuffer + x] = value;
}

void EngineBuffers::setDepthBuffer(int i, const float value) const {
    depthBuffer[i] = value;
}

void EngineBuffers::setVideoBuffer(int x, int y, Uint32 value) const {
    videoBuffer[y * this->widthVideoBuffer + x] = value;
}

void EngineBuffers::setVideoBuffer(int i, Uint32 value) const {
    videoBuffer[i] = value;
}

uint32_t EngineBuffers::getVideoBuffer(int x, int y) const {
    return videoBuffer[y * this->widthVideoBuffer + x];
}

uint32_t EngineBuffers::getVideoBuffer(int bufferIndex) const
{
    return videoBuffer[bufferIndex];
}

void EngineBuffers::clearVideoBuffer() const {

}

void EngineBuffers::flipVideoBufferToSurface(SDL_Surface *surface) {
    // buffer -> surface
    memcpy(&surface->pixels, &videoBuffer, sizeof(surface->pixels));
}

void EngineBuffers::createOpenCLBuffers(_cl_context *context, cl_command_queue &queue)
{
    videoBufferOCL = clCreateBuffer(
        context,
        CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
        sizeBuffers * sizeof(Uint32),
        videoBuffer,
        nullptr
    );

    depthBufferOCL = clCreateBuffer(
        context,
        CL_MEM_READ_WRITE,
        sizeBuffers * sizeof(unsigned int),
        nullptr,
        nullptr
    );

    tileManager = new TileManager(40, 30);

}

void EngineBuffers::loadParticlesEmptyBuffer()
{

}

const std::vector<OCParticle> &EngineBuffers::getParticles() const {
    return particles;
}

sol::state &EngineBuffers::getLua()
{
    return lua;
}

void EngineBuffers::initLUATypes()
{
    lua.open_libraries(sol::lib::base);

    LUAIntegration(lua);

    lua["brakeza"] = Brakeza3D::get();
    lua["componentsManager"] = ComponentsManager::get();
    lua["context"] = sharedLuaContext;
    lua.set_function("print", &Logging::Message);
}

Object3D &EngineBuffers::getSceneObjectById(int i)
{
    auto sceneObjects = Brakeza3D::get()->getSceneObjects();

    return *sceneObjects[i];
}


