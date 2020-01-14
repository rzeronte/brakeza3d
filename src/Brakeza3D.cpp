
#include "../headers/Brakeza3D.h"
#include "../headers/Components/ComponentRender.h"
#include "../headers/Components/ComponentInput.h"
#include "../headers/Components/ComponentSound.h"
#include <thread>

Brakeza3D* Brakeza3D::instance = 0;

Brakeza3D::Brakeza3D()
{
    componentsManager = ComponentsManager::get();
}

Brakeza3D* Brakeza3D::get()
{
    if (instance == 0) {
        instance = new Brakeza3D();
    }

    return instance;
}

void Brakeza3D::start()
{
    componentCamera     = new ComponentCamera();
    componentInput      = new ComponentInput();
    componentWeapons    = new ComponentWeapons();
    componentCollisions = new ComponentCollisions();
    componentWindow     = new ComponentWindow();
    componentBSP        = new ComponentBSP();
    componentSound      = new ComponentSound();
    componentRender     = new ComponentRender();
    componentMenu       = new ComponentMenu();
    componentHUD        = new ComponentHUD();
    componentGUI        = new ComponentGUI( finish );
    componentGame       = new ComponentGame();
    componentGameInput  = new ComponentGameInput( componentGame->getPlayer());


    componentsManager->registerComponent( componentWindow , &getSceneObjects() );
    componentsManager->registerComponent( componentCamera, &getSceneObjects() );
    componentsManager->registerComponent( componentCollisions, &getSceneObjects() );
    componentsManager->registerComponent( componentInput, &getSceneObjects() );
    componentsManager->registerComponent( componentBSP, &getSceneObjects() );
    componentsManager->registerComponent( componentSound, &getSceneObjects() );
    componentsManager->registerComponent( componentRender, &getSceneObjects() );
    componentsManager->registerComponent( componentGUI, &getSceneObjects() );
    componentsManager->registerComponent( componentHUD, &getSceneObjects() );
    componentsManager->registerComponent( componentMenu, &getSceneObjects() );
    componentsManager->registerComponent( componentWeapons, &getSceneObjects() );
    componentsManager->registerComponent( componentGame, &getSceneObjects() );
    componentsManager->registerComponent( componentGameInput, &getSceneObjects() );

    ComponentsManager::get()->configureComponents();

    mainLoop();
}

void Brakeza3D::mainLoop()
{
    ImGuiIO& io = ImGui::GetIO();
    SDL_Event e;

    engineTimer.start();

    onStartComponents();

    while ( !finish ) {
        this->updateTimer();

        preUpdateComponents();

        while ( SDL_PollEvent(&e) )
            onUpdateSDLPollEventComponents(&e, finish);

        onUpdateComponents();
        postUpdateComponents();
    }

    onEndComponents();
}

std::vector<Object3D*> &Brakeza3D::getSceneObjects()
{
    return sceneObjects;
}

void Brakeza3D::addObject3D(Object3D *obj, std::string label)
{
    Logging::getInstance()->Log("Adding Object3D: '" + label + "'", "INFO");
    obj->setLabel(label);
    sceneObjects.emplace_back(obj);
}

Object3D* Brakeza3D::getObjectByLabel(std::string label)
{
    for (int i = 0; i < this->sceneObjects.size(); i++ ) {
        if (!sceneObjects[i]->getLabel().compare(label)) {
            return sceneObjects[i];
        }
    }
}

Timer* Brakeza3D::getTimer()
{
    return &this->engineTimer;
}

void Brakeza3D::updateTimer()
{
    current_ticks = engineTimer.getTicks();
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;

    frameTime += deltaTime;
    executionTime += deltaTime / 1000.f;

    updateFPS();
}

float Brakeza3D::getDeltaTime()
{
    return this->deltaTime/1000;
}

void Brakeza3D::updateFPS()
{
    if (!EngineSetup::getInstance()->DRAW_FPS) return;

    ++fpsFrameCounter;
    if (frameTime > 1000) {
        fps = fpsFrameCounter;
        frameTime = 0;
        fpsFrameCounter = 0;
    }

    Tools::writeTextCenterHorizontal(getComponentsManager()->getComponentWindow()->renderer, getComponentsManager()->getComponentWindow()->fontSmall, Color::yellow(), std::to_string(fps) +"fps", 20);
}

void Brakeza3D::onStartComponents() {
    std::vector<Component *>::iterator it;
    for ( it = componentsManager->components.begin(); it != componentsManager->components.end(); it++) {
        Component *component = *(it);
        component->onStart();
    }
}

void Brakeza3D::preUpdateComponents() {
    std::vector<Component *>::iterator it;
    for ( it = componentsManager->components.begin(); it != componentsManager->components.end(); it++) {
        Component *component = *(it);
        component->preUpdate();
    }
}

void Brakeza3D::onUpdateComponents() {
    std::vector<Component *>::iterator it;
    for ( it = componentsManager->components.begin(); it != componentsManager->components.end(); it++) {
        Component *component = *(it);
        component->onUpdate();
    }
}

void Brakeza3D::postUpdateComponents()
{
    std::vector<Component *>::iterator it;
    for ( it = componentMenu->components->begin(); it != componentMenu->components->end(); it++) {
        Component *component = *(it);
        component->postUpdate();
    }
}

void Brakeza3D::onEndComponents() {
    std::vector<Component *>::iterator it;
    for ( it = componentMenu->components->begin(); it != componentMenu->components->end(); it++) {
        Component *component = *(it);
        component->onEnd();
    }
}

void Brakeza3D::onUpdateSDLPollEventComponents(SDL_Event *event, bool &finish)
{
    std::vector<Component *>::iterator it;
    for ( it = componentMenu->components->begin(); it != componentMenu->components->end(); it++) {
        Component *component = *(it);
        component->onSDLPollEvent( event, finish );
    }
}

ComponentsManager *Brakeza3D::getComponentsManager() const {
    return componentsManager;
}
