
#include "../headers/Brakeza3D.h"
#include "../headers/Components/ComponentRender.h"
#include "../headers/Components/ComponentInput.h"
#include "../headers/Components/ComponentSound.h"
#include "../headers/Objects/Mesh3DAnimated.h"
#include <thread>

Brakeza3D* Brakeza3D::instance = 0;

Brakeza3D::Brakeza3D()
{
    componentsManager = ComponentsManager::get();
    componentsManager->setSceneObjects( &sceneObjects );
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
    componentGameInput  = new ComponentGameInput( componentGame->getPlayer() );

    componentsManager->registerComponent( componentWindow, "ComponentWindow" );
    componentsManager->registerComponent( componentCamera, "ComponentCamera" );
    componentsManager->registerComponent( componentCollisions, "ComponentCollisions" );
    componentsManager->registerComponent( componentInput, "ComponentInput" );
    componentsManager->registerComponent( componentBSP, "ComponentBSP" );
    componentsManager->registerComponent( componentSound, "ComponentSound" );
    componentsManager->registerComponent( componentRender, "ComponentRender" );
    componentsManager->registerComponent( componentGUI, "ComponentGUI" );
    componentsManager->registerComponent( componentMenu, "ComponentMenu" );
    componentsManager->registerComponent( componentWeapons,  "ComponentWeapons" );
    componentsManager->registerComponent( componentHUD,  "ComponentHUD" );
    componentsManager->registerComponent( componentGame,  "ComponentGame" );
    componentsManager->registerComponent( componentGameInput,  "ComponentGameInput" );

    ComponentsManager::get()->configureComponents();

    Mesh3DAnimated* mesh = new Mesh3DAnimated();
    this->addObject3D(mesh, "collada");

    if ( mesh->AssimpLoad( EngineSetup::getInstance()->MODELS_FOLDER + "bob_lamp_update.dae") ) {
        mesh->setScale(1);
        Vertex3D p = *componentCamera->getCamera()->getPosition();
        p.y-= -1;
        p.z-= -10;
        M3 r = M3::getMatrixRotationForEulerAngles(90, 0, 0);
        mesh->setRotation( r );
        mesh->setPosition( p );
    }

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

    std::cout << "FPS: " <<fps << " fps" << std::endl;
    //Tools::writeTextCenterHorizontal(getComponentsManager()->getComponentWindow()->renderer, getComponentsManager()->getComponentWindow()->fontSmall, Color::yellow(), std::to_string(fps) +"fps", 20);
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
