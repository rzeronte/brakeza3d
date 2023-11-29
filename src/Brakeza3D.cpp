
#include "../include/Brakeza3D.h"
#include "../imgui/backends/imgui_impl_sdl.h"
#include "../imgui/backends/imgui_impl_sdlrenderer.h"
#include <thread>

Brakeza3D *Brakeza3D::instance = nullptr;

Brakeza3D::Brakeza3D()
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
    Logging::Message("Brakeza3D - Open source game toolkit for old school lovers");
    Logging::Message("By Eduardo Rodriguez (eduardo@brakeza.com)");
    Logging::Message("https://brakeza.com");
    Logging::Message("Let's start!");

    Logging::Message("Running %s", EngineSetup::get()->ENGINE_TITLE.c_str());

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

    startLoop();
}

void Brakeza3D::renderLoop()
{
    while (!finish) {
        controlFrameRate();

        this->updateTimer();

        componentsManager->getComponentWindow()->clearVideoBuffers();

        componentsManager->getComponentRender()->writeOCLBuffersFromHost();

        preUpdateComponents();

        onUpdateComponents();

        ComponentRender::onUpdateSceneObjectsSecondPass();

        if (EngineSetup::get()->IMGUI_ENABLED) ImGuiOnUpdate();

        componentsManager->getComponentRender()->writeOCLBufferIntoHost();

        componentsManager->getComponentRender()->drawObjetsInHostBuffer();

        postUpdateComponents();

        componentsManager->getComponentWindow()->renderToWindow();
    }
}

void Brakeza3D::mainLoop()
{
    while (!finish) {
        while (SDL_PollEvent(&e)) {
            onUpdateSDLPollEventComponents(&e, finish);
            ImGui_ImplSDL2_ProcessEvent(&e);
        }
    }
}

void Brakeza3D::startLoop()
{

    engineTimer.start();

    ComponentsManager::get()->getComponentCollisions()->initBulletSystem();
    ComponentsManager::get()->getComponentCamera()->setFreeLook(true);

    onStartComponents();

    //LoadDemo();
    std::thread renderThread(&Brakeza3D::renderLoop, this);

    ImGuiInitialize();

    mainLoop();

    onEndComponents();

    delete componentsManager;
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
        //if (!component->isCore()) continue;
        component->preUpdate();
    }
}

void Brakeza3D::onUpdateComponents()
{
    for (Component*& component : componentsManager->components) {
        //if (!component->isCore()) continue;
        component->onUpdate();
    }
}

void Brakeza3D::postUpdateComponents()
{
    for (Component*& component : componentsManager->components) {
        //if (!component->isCore()) continue;
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


void Brakeza3D::ImGuiOnUpdate()
{
    ImGui_ImplSDLRenderer_NewFrame();
    ImGui_ImplSDL2_NewFrame(getComponentsManager()->getComponentWindow()->getWindow());

    ImGui::NewFrame();
    this->managerGUI.draw(
        Brakeza3D::get()->getDeltaTime(),
        finish,
        Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera(),
        Brakeza3D::get()->getSceneObjects(),
        Brakeza3D::get()->getComponentsManager()->getComponentRender()->getLightPoints(),
        Brakeza3D::get()->getComponentsManager()->getComponentRender()->getTiles(),
        Brakeza3D::get()->getComponentsManager()->getComponentRender()->getTilesWidth()
    );

    //ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
}

void Brakeza3D::ImGuiInitialize() const
{
    std::cout << "ImGuiInitialize" << std::endl;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplSDL2_InitForSDLRenderer(
        getComponentsManager()->getComponentWindow()->getWindow(),
        getComponentsManager()->getComponentWindow()->getRenderer()
    );

    ImGui_ImplSDLRenderer_Init(getComponentsManager()->getComponentWindow()->getRenderer());

    ImGuiIO &io = ImGui::GetIO();
    io.WantCaptureMouse = false;
    io.WantCaptureKeyboard = false;

    // Setup style
    ImGui::StyleColorsDark();
    ImGuiStyle &style = ImGui::GetStyle();
    style.FrameBorderSize = 1.0f;
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
    return prefix + std::string("_") + std::to_string(Brakeza3D::get()->getSceneObjects().size() + 1);
}

void Brakeza3D::LoadDemo()
{
    auto *newObject = new Mesh3D();
    newObject->setEnabled(true);
    newObject->setStencilBufferEnabled(true);
    newObject->setRotationFrameEnabled(true);
    newObject->setRotationFrame(Vertex3D(1, 0, 0));
    newObject->setPosition(Vertex3D(0, 0, 6000));
    newObject->setScale(1);
    newObject->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "eye.fbx"));

    Brakeza3D::get()->addObject3D(newObject, Brakeza3D::uniqueObjectLabel("eye"));
}