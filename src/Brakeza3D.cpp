
#include "../headers/Brakeza3D.h"

Brakeza3D *Brakeza3D::instance = nullptr;

Brakeza3D::Brakeza3D() {
    componentsManager = ComponentsManager::get();
    componentsManager->setSceneObjects(&sceneObjects);

    componentCamera = nullptr;
    componentInput = nullptr;
    componentWeapons = nullptr;
    componentCollisions = nullptr;
    componentWindow = nullptr;
    componentBSP = nullptr;
    componentSound = nullptr;
    componentRender = nullptr;
    componentMenu = nullptr;
    componentHUD = nullptr;
    componentGUI = nullptr;
    componentGame = nullptr;
    componentGameInput = nullptr;
}

Brakeza3D *Brakeza3D::get() {
    if (instance == nullptr) {
        instance = new Brakeza3D();
    }

    return instance;
}

void Brakeza3D::start() {
    componentCamera = new ComponentCamera();
    componentInput = new ComponentInput();
    componentWeapons = new ComponentWeapons();
    componentCollisions = new ComponentCollisions();
    componentWindow = new ComponentWindow();
    componentBSP = new ComponentBSP();
    componentSound = new ComponentSound();
    componentRender = new ComponentRender();
    componentMenu = new ComponentMenu();
    componentHUD = new ComponentHUD();
    componentGUI = new ComponentGUI(finish);
    componentGame = new ComponentGame();
    componentGameInput = new ComponentGameInput(componentGame->getPlayer());

    componentsManager->registerComponent(componentWindow, "ComponentWindow");
    componentsManager->registerComponent(componentCamera, "ComponentCamera");
    componentsManager->registerComponent(componentCollisions, "ComponentCollisions");
    componentsManager->registerComponent(componentInput, "ComponentInput");
    componentsManager->registerComponent(componentBSP, "ComponentBSP");
    componentsManager->registerComponent(componentSound, "ComponentSound");
    componentsManager->registerComponent(componentRender, "ComponentRender");
    componentsManager->registerComponent(componentGUI, "ComponentGUI");
    componentsManager->registerComponent(componentMenu, "ComponentMenu");
    componentsManager->registerComponent(componentWeapons, "ComponentWeapons");
    componentsManager->registerComponent(componentHUD, "ComponentHUD");
    componentsManager->registerComponent(componentGame, "ComponentGame");
    componentsManager->registerComponent(componentGameInput, "ComponentGameInput");

    ComponentsManager::get()->configureComponents();

    mainLoop();
}

void Brakeza3D::mainLoop() {
    ImGuiIO &io = ImGui::GetIO();
    SDL_Event e;

    engineTimer.start();

    onStartComponents();

    while (!finish) {
        this->updateTimer();

        preUpdateComponents();

        while (SDL_PollEvent(&e))
            onUpdateSDLPollEventComponents(&e, finish);

        onUpdateComponents();
        postUpdateComponents();
    }

    onEndComponents();
}

std::vector<Object3D *> &Brakeza3D::getSceneObjects() {
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
