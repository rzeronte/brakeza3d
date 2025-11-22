#define GL_GLEXT_PROTOTYPES

#include "../include/Brakeza3D.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../include/cxxxopts.h"

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

void Brakeza3D::start(int argc, char *argv[])
{
    cxxopts::Options options("Brakeza3D", "Thanks for using Brakeza3D!. Here you can see argument's options:");

    options.add_options()
        ("p,project", "Project file", cxxopts::value<std::string>())
        ("h,help", "Help")
    ;

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return;
    }

    bool projectAutoload = false;
    std::string project;

    if (result.count("p")) {
        projectAutoload = true;
        project = result["p"].as<std::string>();
        std::cout << "Autoload Project: " << result["p"].as<std::string>() << std::endl;
    }

    componentsManager->registerComponent(new ComponentWindow(), "ComponentWindow");
    componentsManager->registerComponent(new ComponentScripting(), "ComponentScripting");
    componentsManager->registerComponent(new ComponentCamera(), "ComponentCamera");
    componentsManager->registerComponent(new ComponentCollisions(), "ComponentCollisions");
    componentsManager->registerComponent(new ComponentInput(), "ComponentInput");
    componentsManager->registerComponent(new ComponentSound(), "ComponentSound");
    componentsManager->registerComponent(new ComponentRender(), "ComponentRender");

    mainLoop(projectAutoload, project);
}

void Brakeza3D::welcomeMessage()
{
    Logging::Message("Brakeza3D - Open source game toolkit for old school lovers");
    Logging::Message("By Eduardo Rodriguez (eduardo@brakeza.com)");
    Logging::Message("https://brakeza.com");
    Logging::Message("Let's start!");

    Logging::Message("Running %s", EngineSetup::get()->ENGINE_TITLE.c_str());
}

void Brakeza3D::CaptureInputEvents(SDL_Event &e)
{
    while (SDL_PollEvent(&e)) {
        checkForResizeOpenGLWindow(e);
        onUpdateSDLPollEventComponents(&e, finish);
        ImGui_ImplSDL2_ProcessEvent(&e);
    }
}

void Brakeza3D::mainLoop(bool autostart, const std::string& project)
{
    SDL_Event event;

    engineTimer.start();
    managerGUI = new GUIManager(sceneObjects);

    auto window = componentsManager->getComponentWindow();
    auto render = componentsManager->getComponentRender();
    auto scripting = componentsManager->getComponentScripting();
    auto collisions = componentsManager->getComponentCollisions();

    scripting->initLUATypes();
    collisions->initBulletSystem();
    onStartComponents();

    window->ImGuiInitialize(EngineSetup::get()->CONFIG_FOLDER + "ImGuiDefault.ini");
    welcomeMessage();

    handleAutoStartProject(autostart, project);

    while (!finish) {
        ControlFrameRate();
        UpdateTimer();
        PreUpdateComponents();

        render->RunShadersOpenGLPreUpdate();

        CaptureInputEvents(event);
        window->ClearOGLFrameBuffers();

        onUpdateComponents();

        render->RenderLayersToGlobalFramebuffer();
        render->RunShadersOpenGLPostUpdate();
        postUpdateComponents();

        window->RenderLayersToMain();
    }

    onEndComponents();
}

void Brakeza3D::checkForResizeOpenGLWindow(const SDL_Event &e)
{
    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        auto window = ComponentsManager::get()->getComponentWindow();
        window->updateWindowSize();
        glViewport(0,0, window->getWidth(), window->getHeight());
        window->resetFramebuffer();
    }
}

void Brakeza3D::ControlFrameRate() const
{
    if (!EngineSetup::get()->LIMIT_FRAMERATE) return;

    const float frameDelay = 1000.0f / static_cast<float>(EngineSetup::get()->FRAMERATE);

    if (deltaTime < frameDelay) {
        SDL_Delay(floor(frameDelay - deltaTime));
    }
}

std::vector<Object3D *> &Brakeza3D::getSceneObjects()
{
    return sceneObjects;
}

void Brakeza3D::addObject3D(Object3D *obj, const std::string &label)
{
    Logging::Message("Adding Object3D to scene: %s", label.c_str());
    obj->setLabel(label);
    sceneObjects.push_back(obj);
}

Timer *Brakeza3D::getTimer()
{
    return &this->engineTimer;
}

void Brakeza3D::UpdateTimer()
{
    current_ticks = static_cast<float>(engineTimer.getTicks());
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;
    executionTime += deltaTime / 1000.f;
}

float Brakeza3D::getDeltaTime() const
{
    return this->deltaTime / 1000;
}

float Brakeza3D::getDeltaTimeMicro() const
{
    return this->deltaTime;
}

void Brakeza3D::onStartComponents() const
{
    for (Component*& component : componentsManager->Components()) {
        component->onStart();
    }
}

void Brakeza3D::PreUpdateComponents() const
{
    for (Component*& component : componentsManager->Components()) {
        component->preUpdate();
    }
}

void Brakeza3D::onUpdateComponents() const
{
    for (Component*& component : componentsManager->Components()) {
        component->onUpdate();
    }
}

void Brakeza3D::postUpdateComponents() const
{
    for (Component*& component : componentsManager->Components()) {
        component->postUpdate();
    }
}

void Brakeza3D::onEndComponents() const
{
    for (const auto o : sceneObjects) {
        delete o;
    }

    for (Component*& component : componentsManager->Components()) {
        component->onEnd();
    }

    delete componentsManager;

    SDL_Quit();
    std::cout << "Exiting... good bye! ;)" << std::endl;
    exit(0);
}

void Brakeza3D::handleAutoStartProject(bool autostart, const std::string &project) const
{
    auto render = componentsManager->getComponentRender();

    if (autostart) {
        render->getProjectLoader().loadProject(EngineSetup::get()->PROJECTS_FOLDER + project);
        EngineSetup::get()->ENABLE_IMGUI = false;
        componentsManager->getComponentScripting()->playLUAScripts();
        return;
    }

    render->getSceneLoader().loadScene(EngineSetup::get()->CONFIG_FOLDER + EngineSetup::get()->DEFAULT_SCENE);
}

void Brakeza3D::onUpdateSDLPollEventComponents(SDL_Event *event, bool &finish) const
{
    for (Component* &component : componentsManager->Components()) {
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

    for (const auto o : sceneObjects) {
        delete o;
    }
}

int Brakeza3D::getNextObjectID() const
{
    const int id = (static_cast<int>(sceneObjects.size()) + 1) * 10;
    Logging::Message("Next Object ID: %d", id);

    return id;
}

float &Brakeza3D::getExecutionTime()
{
    return executionTime;
}

std::string Brakeza3D::uniqueObjectLabel(const char *prefix)
{
    return prefix + std::string("_") + std::to_string(Tools::random(0, 100));
}

GUIManager *Brakeza3D::getManagerGui() const
{
    return managerGUI;
}

Object3D *Brakeza3D::getSceneObjectByLabel(const std::string &label) const
{
    for (const auto sceneObject : sceneObjects) {
        if (sceneObject->getLabel() == label) {
            return sceneObject;
        }
    }

    return nullptr;
}

Object3D *Brakeza3D::getSceneObjectById(const int id) const
{
    for (const auto sceneObject : sceneObjects) {
        if (sceneObject->getId() == id) {
            return sceneObject;
        }
    }

    return nullptr;
}

void Brakeza3D::shutdown()
{
    finish = true;
}