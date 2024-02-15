#define GL_GLEXT_PROTOTYPES

#include "../include/Brakeza3D.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "imgui_internal.h"

Brakeza3D *Brakeza3D::instance = nullptr;

Brakeza3D::Brakeza3D() : managerGUI(nullptr)
{
    componentsManager = ComponentsManager::get();
}

Brakeza3D *Brakeza3D::get()
{
    if (instance == nullptr) {
        instance = new Brakeza3D();
    }

    return instance;
}

void Brakeza3D::start()
{
    componentsManager->registerComponent(new ComponentWindow(), "ComponentWindow");
    componentsManager->registerComponent(new ComponentCamera(), "ComponentCamera");
    componentsManager->registerComponent(new ComponentCollisions(), "ComponentCollisions");
    componentsManager->registerComponent(new ComponentInput(), "ComponentInput");
    componentsManager->registerComponent(new ComponentSound(), "ComponentSound");
    componentsManager->registerComponent(new ComponentRender(), "ComponentRender");

    // Custom components here
    componentsManager->registerComponent(new ComponentMenu(), "ComponentMenu");
    componentsManager->registerComponent(new ComponentGame(), "ComponentGame");
    componentsManager->registerComponent(new ComponentHUD(), "ComponentHUD");
    componentsManager->registerComponent(new ComponentGameInput(), "ComponentGameInput");

    mainLoop();
}

void Brakeza3D::welcomeMessage() {
    Logging::Message("Brakeza3D - Open source game toolkit for old school lovers");
    Logging::Message("By Eduardo Rodriguez (eduardo@brakeza.com)");
    Logging::Message("https://brakeza.com");
    Logging::Message("Let's start!");

    Logging::Message("Running %s", EngineSetup::get()->ENGINE_TITLE.c_str());
}

void Brakeza3D::mainLoop()
{
    SDL_Event e;
    engineTimer.start();
    managerGUI = new GUIManager(sceneObjects);
    LUAManager::get()->initLUATypes();
    componentsManager->getComponentCollisions()->initBulletSystem();
    componentsManager->getComponentCamera()->setFreeLook(true);
    onStartComponents();
    //LoadDemo();
    componentsManager->getComponentWindow()->ImGuiInitialize(EngineSetup::get()->CONFIG_FOLDER + "ImGuiDefault.ini");
    welcomeMessage();

    while (!finish) {
        controlFrameRate();
        updateTimer();
        preUpdateComponents();
        componentsManager->getComponentRender()->runShadersOpenCLPreUpdate();
        while (SDL_PollEvent(&e)) {
            checkForResizeOpenGLWindow(e);
            onUpdateSDLPollEventComponents(&e, finish);
            ImGui_ImplSDL2_ProcessEvent(&e);
        }
        onUpdateComponents();
        componentsManager->getComponentWindow()->RenderLayersToGlobalFramebuffer();
        postUpdateComponents();
        componentsManager->getComponentRender()->runShadersOpenCLPostUpdate();
        if (EngineSetup::get()->IMGUI_ENABLED) componentsManager->getComponentWindow()->ImGuiOnUpdate();
        componentsManager->getComponentWindow()->RenderLayersToMain();
    }
    onEndComponents();
    delete componentsManager;
}

void Brakeza3D::checkForResizeOpenGLWindow(SDL_Event &e) {
    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        auto window = ComponentsManager::get()->getComponentWindow();
        SDL_GetWindowSize(window->getWindow(), &window->width, &window->height);
        glViewport(0,0,(GLsizei)window->width,(GLsizei)window->height);
        ComponentsManager::get()->getComponentWindow()->resetFramebuffer();
    }
}

void Brakeza3D::controlFrameRate() const
{
    if (!EngineSetup::get()->LIMIT_FRAMERATE) return;

    const float frameDelay = 1000.0f / (float) EngineSetup::get()->FRAMERATE;

    if (deltaTime < frameDelay) {
        SDL_Delay(floor(frameDelay - deltaTime));
    }
}

std::vector<Object3D *> &Brakeza3D::getSceneObjects(){
    return sceneObjects;
}

void Brakeza3D::addObject3D(Object3D *obj, const std::string &label)
{
    Logging::Log("Adding Object3D: %s", label.c_str());
    obj->setLabel(label);
    sceneObjects.push_back(obj);
}

Timer *Brakeza3D::getTimer()
{
    return &this->engineTimer;
}

void Brakeza3D::updateTimer()
{
    current_ticks = (float) engineTimer.getTicks();
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;
    executionTime += deltaTime / 1000.f;
}

float Brakeza3D::getDeltaTime() const {
    return this->deltaTime / 1000;
}

float Brakeza3D::getDeltaTimeMicro() const {
    return this->deltaTime;
}

void Brakeza3D::onStartComponents()
{
    for (Component*& component : componentsManager->components) {
        component->onStart();
    }
}

void Brakeza3D::preUpdateComponents()
{
    for (Component*& component : componentsManager->components) {
        component->preUpdate();
    }
}

void Brakeza3D::onUpdateComponents()
{
    for (Component*& component : componentsManager->components) {
        component->onUpdate();
    }
}

void Brakeza3D::postUpdateComponents()
{
    for (Component*& component : componentsManager->components) {
        component->postUpdate();
    }
}

void Brakeza3D::onEndComponents()
{
    for (Component*& component : componentsManager->components) {
        component->onEnd();
    }

    Logging::head("Exiting... good bye! ;)");
}

void Brakeza3D::onUpdateSDLPollEventComponents(SDL_Event *event, bool &finish)
{
    for (Component* &component : this->componentsManager->components) {
        component->onSDLPollEvent(event, finish);
    }
}

ComponentsManager *Brakeza3D::getComponentsManager() const
{
    return componentsManager;
}


Brakeza3D::~Brakeza3D()
{
    ImGui::DestroyContext();

    for (auto o : sceneObjects) {
        delete o;
    }
}

float &Brakeza3D::getExecutionTime()
{
    return executionTime;
}

std::string Brakeza3D::uniqueObjectLabel(const char *prefix)
{
    return prefix + std::string("_") + std::to_string(Tools::random(0, 100));
}

void Brakeza3D::LoadDemo()
{
    auto *newObject = new Mesh3D();
    newObject->setEnabled(true);
    newObject->setBelongToScene(true);
    newObject->setStencilBufferEnabled(true);
    newObject->setRotationFrameEnabled(true);
    newObject->setRotationFrame(Vertex3D(1, 0, 0));
    newObject->setPosition(Vertex3D(0, 0, 6000));
    newObject->setScale(0.5);
    newObject->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "eye.fbx"));

    Brakeza3D::get()->addObject3D(newObject, "DemoObject3D");
}

GUIManager *Brakeza3D::getManagerGui()
{
    return managerGUI;
}

Object3D &Brakeza3D::getSceneObjectByLabel(const std::string &label)
{
    for (unsigned int i = 0; i < this->sceneObjects.size(); i++) {
        if (sceneObjects[i]->getLabel() == label) {
            return *sceneObjects[i];
        }
    }

    throw std::runtime_error("Object not found");
}

Object3D *Brakeza3D::getSceneObjectById(int i)
{
    return sceneObjects[i];
}