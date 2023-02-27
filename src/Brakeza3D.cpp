
#include "../include/Brakeza3D.h"
#include "../imgui/backends/imgui_impl_sdl.h"
#include "../imgui/backends/imgui_impl_sdlrenderer.h"

Brakeza3D *Brakeza3D::instance = nullptr;

Brakeza3D::Brakeza3D()
{
    componentsManager = ComponentsManager::get();
    componentsManager->setSceneObjects(&sceneObjects);
}

Brakeza3D *Brakeza3D::get() {
    if (instance == nullptr) {
        instance = new Brakeza3D();
    }

    return instance;
}

void Brakeza3D::start()
{
    componentCamera = new ComponentCamera();
    componentInput = new ComponentInput();
    componentCollisions = new ComponentCollisions();
    componentWindow = new ComponentWindow();
    componentSound = new ComponentSound();
    componentRender = new ComponentRender();
    componentMenu = new ComponentMenu();
    componentHUD = new ComponentHUD();
    componentGame = new ComponentGame();
    componentGameInput = new ComponentGameInput(componentGame->getPlayer());

    componentsManager->registerComponent(componentWindow, "ComponentWindow");
    componentsManager->registerComponent(componentCamera, "ComponentCamera");
    componentsManager->registerComponent(componentCollisions, "ComponentCollisions");
    componentsManager->registerComponent(componentInput, "ComponentInput");
    componentsManager->registerComponent(componentSound, "ComponentSound");
    componentsManager->registerComponent(componentRender, "ComponentRender");
    componentsManager->registerComponent(componentMenu, "ComponentMenu");
    componentsManager->registerComponent(componentGame, "ComponentGame");
    componentsManager->registerComponent(componentHUD, "ComponentHUD");
    componentsManager->registerComponent(componentGameInput, "ComponentGameInput");

    ComponentsManager::get()->configureComponents();

    mainLoop();
}

void Brakeza3D::mainLoop() {
    SDL_Event e;

    engineTimer.start();

    onStartComponents();

    ImGuiInitialize();

    //AxisPlaneInitialize();

    while (!finish) {
        controlFrameRate();

        this->updateTimer();

        preUpdateComponents();

        ImGuiOnUpdate();

        while (SDL_PollEvent(&e)) {
            onUpdateSDLPollEventComponents(&e, finish);
            ImGui_ImplSDL2_ProcessEvent(&e);
        }

        onUpdateComponents();
        postUpdateComponents();
    }

    onEndComponents();

    delete componentsManager;
}

void Brakeza3D::controlFrameRate() const
{
    if (!EngineSetup::get()->LIMIT_FRAMERATE) return;

    float frameDelay = 1000.0f / (float) EngineSetup::get()->FRAMERATE;

    if (deltaTime < frameDelay) {
        SDL_Delay(floor(frameDelay - deltaTime));
    }
}

const std::vector<Object3D *> &Brakeza3D::getSceneObjects() const {
    return sceneObjects;
}

void Brakeza3D::addObject3D(Object3D *obj, const std::string &label) {
    Logging::Log("Adding Object3D: '" + label + "'", "INFO");
    obj->setLabel(label);
    sceneObjects.push_back(obj);
}

Object3D *Brakeza3D::getObjectByLabel(const std::string &label) {
    for (int i = 0; i < this->sceneObjects.size(); i++) {
        if (sceneObjects[i]->getLabel() == label) {
            return sceneObjects[i];
        }
    }

    return nullptr;
}

Timer *Brakeza3D::getTimer() {
    return &this->engineTimer;
}

void Brakeza3D::updateTimer() {
    current_ticks = (float) engineTimer.getTicks();
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;

    executionTime += deltaTime / 1000.f;
}

float Brakeza3D::getDeltaTime() const {
    return this->deltaTime / 1000;
}

void Brakeza3D::onStartComponents() {
    std::vector<Component *>::iterator it;
    for (it = this->componentsManager->components.begin(); it != componentsManager->components.end(); it++) {
        Component *component = *(it);
        component->onStart();
    }
}

void Brakeza3D::preUpdateComponents() {
    std::vector<Component *>::iterator it;
    for (it = this->componentsManager->components.begin(); it != componentsManager->components.end(); it++) {
        Component *component = *(it);
        component->preUpdate();
    }
}

void Brakeza3D::onUpdateComponents() {
    std::vector<Component *>::iterator it;
    for (it = this->componentsManager->components.begin(); it != componentsManager->components.end(); it++) {
        Component *component = *(it);
        component->onUpdate();
    }
}

void Brakeza3D::postUpdateComponents() {
    std::vector<Component *>::iterator it;
    for (it = this->componentsManager->components.begin(); it != componentsManager->components.end(); it++) {
        Component *component = *(it);
        component->postUpdate();
    }
}

void Brakeza3D::onEndComponents() {
    std::vector<Component *>::iterator it;
    for (it = this->componentsManager->components.begin(); it != componentsManager->components.end(); it++) {
        Component *component = *(it);
        component->onEnd();
    }
}

void Brakeza3D::onUpdateSDLPollEventComponents(SDL_Event *event, bool &finish) {
    std::vector<Component *>::iterator it;
    for (it = this->componentsManager->components.begin(); it != componentsManager->components.end(); it++) {
        Component *component = *(it);
        component->onSDLPollEvent(event, finish);
    }
}

ComponentsManager *Brakeza3D::getComponentsManager() const {
    return componentsManager;
}

void Brakeza3D::AxisPlaneInitialize()
{
    auto axisPlanes = new Mesh3DBody();
    axisPlanes->setRotation(0, 0, 0);
    axisPlanes->initializeStencilBuffer();
    axisPlanes->setStencilBufferEnabled(true);
    axisPlanes->setEnabled(false);
    axisPlanes->setLabel("test");
    axisPlanes->setCollisionsEnabled(true);
    axisPlanes->setScale(1);
    axisPlanes->setFlatTextureColor(false);
    axisPlanes->setRotationFrameEnabled(false);
    axisPlanes->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "axisPlanes.fbx"));
    axisPlanes->makeRigidBodyFromTriangleMesh(0, ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld(), EngineSetup::collisionGroups::AllFilter, EngineSetup::collisionGroups::AllFilter);
    addObject3D(axisPlanes, "AxisPlanes");
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
    for (auto o : sceneObjects) {
        delete o;
    }
}

float &Brakeza3D::getExecutionTime() {
    return executionTime;
}
