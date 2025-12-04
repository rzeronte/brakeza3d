#define GL_GLEXT_PROTOTYPES

#include "../include/Brakeza.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../cxxxopts/cxxxopts.h"

Brakeza *Brakeza::instance = nullptr;

Brakeza::Brakeza() : managerGUI(nullptr)
{
    componentsManager = ComponentsManager::get();
}

Brakeza *Brakeza::get()
{
    if (instance == nullptr) {
        instance = new Brakeza();
    }

    return instance;
}

void Brakeza::start(int argc, char *argv[])
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

    componentsManager->RegisterComponent(new ComponentWindow(), "ComponentWindow");
    componentsManager->RegisterComponent(new ComponentScripting(), "ComponentScripting");
    componentsManager->RegisterComponent(new ComponentCamera(), "ComponentCamera");
    componentsManager->RegisterComponent(new ComponentCollisions(), "ComponentCollisions");
    componentsManager->RegisterComponent(new ComponentInput(), "ComponentInput");
    componentsManager->RegisterComponent(new ComponentSound(), "ComponentSound");
    componentsManager->RegisterComponent(new ComponentRender(), "ComponentRender");

    mainLoop(projectAutoload, project);
}

void Brakeza::WelcomeMessage()
{
    Logging::Message("############################################################");
    Logging::Message("");
    Logging::Message("************************");
    Logging::Message("*  B R A K E Z A 3 D   *");
    Logging::Message("************************");
    Logging::Message("");
    Logging::Message("Open source game engine for developers");
    Logging::Message("############################################################");
    Logging::Message("");
    Logging::Message(BrakezaSetup::get()->ENGINE_WEBSITE.c_str());
    Logging::Message(BrakezaSetup::get()->ENGINE_SOURCE_WEBSITE.c_str());
    Logging::Message("");
    Logging::Message("%s", BrakezaSetup::get()->ENGINE_TITLE.c_str());
    Logging::Message("");
    Logging::Message("############################################################");
    Logging::Message("");
}

void Brakeza::CaptureInputEvents(SDL_Event &e) const {

    while (SDL_PollEvent(&e)) {
        checkForResizeOpenGLWindow(e);
        onUpdateSDLPollEventComponents(&e);
        ImGui_ImplSDL2_ProcessEvent(&e);
    }
}

void Brakeza::mainLoop(bool autostart, const std::string& project)
{
    SDL_Event event;

    engineTimer.start();
    managerGUI = new GUIManager(sceneObjects);

    auto window = componentsManager->getComponentWindow();
    auto render = componentsManager->getComponentRender();

    componentsManager->getComponentScripting()->InitLUATypes();
    componentsManager->getComponentCollisions()->InitBulletSystem();

    OnStartComponents();

    WelcomeMessage();
    BrakezaSetup::get()->ENABLE_LOGGING_STD = false;

    //HandleAutoStartProject(autostart, project);

    // Profiler tags
    Profiler::ResetMeasure(Profiler::get()->getComponentMeasures(), "RenderLayersToGlobal");
    Profiler::ResetMeasure(Profiler::get()->getComponentMeasures(), "RunShadersOpenGLPreUpdate");
    Profiler::ResetMeasure(Profiler::get()->getComponentMeasures(), "RunShadersOpenGLPostUpdate");
    Profiler::ResetMeasure(Profiler::get()->getComponentMeasures(), "RenderLayersToGlobalFramebuffer");

    while (!BrakezaSetup::get()->EXIT) {
        Profiler::get()->ResetTotalFrameTime();     // Reset profiler measures
        ControlFrameRate();                         // Control framerate based on SDL_Delay
        UpdateTimer();                              // Refresh main timer.
        PreUpdateComponents();                      // PreUpdate for componentes
        render->RunSceneShadersPreUpdate();         // Pre-pass running for shaders
        CaptureInputEvents(event);               // Capture keyboard/mouse status
        window->ClearOGLFrameBuffers();             // Clean video framebuffers
        OnUpdateComponents();                       // OnUpdate for componentes
        render->RenderLayersToGlobalFramebuffer();  // Buffers compositing
        render->RunSceneShadersPostUpdate();        // Post-pass running for shaders
        PostUpdateComponents();                     // PostUpdate for componentes
        Profiler::get()->EndTotalFrameTime();       // End frame time measure
        window->RenderLayersToMain();               // Flip to screen
    }

    onEndComponents();
}

void Brakeza::checkForResizeOpenGLWindow(const SDL_Event &e)
{
    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        auto window = ComponentsManager::get()->getComponentWindow();
        window->UpdateWindowSize();
        glViewport(0,0, window->getWidth(), window->getHeight());
        window->resetFramebuffer();
    }
}

void Brakeza::ControlFrameRate() const
{
    if (!BrakezaSetup::get()->LIMIT_FRAMERATE) return;

    const float frameDelay = 1000.0f / static_cast<float>(BrakezaSetup::get()->FRAMERATE);
    if (deltaTime < frameDelay) {
        SDL_Delay(floor(frameDelay - deltaTime));
    }
}

std::vector<Object3D *> &Brakeza::getSceneObjects()
{
    return sceneObjects;
}

void Brakeza::addObject3D(Object3D *obj, const std::string &label)
{
    Logging::Message("[Brakeza3D addObject3D] Adding Object3D to scene: %s", label.c_str());
    obj->setLabel(label);
    sceneObjects.push_back(obj);
}

Timer *Brakeza::getTimer()
{
    return &this->engineTimer;
}

void Brakeza::UpdateTimer()
{
    current_ticks = static_cast<float>(engineTimer.getTicks());
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;
    executionTime += deltaTime / 1000.f;
}

float Brakeza::getDeltaTime() const
{
    return this->deltaTime / 1000;
}

float Brakeza::getDeltaTimeMicro() const
{
    return this->deltaTime;
}

void Brakeza::OnStartComponents() const
{
    for (auto c : componentsManager->Components())
        c->onStart();

    Logging::Message("[Brakeza] Time to wake up...");
    Logging::Message("");
}

void Brakeza::PreUpdateComponents() const
{
    for (auto c : componentsManager->Components()) {
        Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), c->getLabel() + "_pre");
        c->preUpdate();
        Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), c->getLabel() + "_pre");
    }
}

void Brakeza::OnUpdateComponents() const
{
    for (auto c : componentsManager->Components()) {
        Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), c->getLabel() + "_update");
        c->onUpdate();
        Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), c->getLabel() + "_update");
    }
}

void Brakeza::PostUpdateComponents() const
{
    for (auto c: componentsManager->Components()) {
        Profiler::get()->StartMeasure(Profiler::get()->getComponentMeasures(), c->getLabel() + "_post");
        c->postUpdate();
        Profiler::get()->EndMeasure(Profiler::get()->getComponentMeasures(), c->getLabel() + "_post");
    }
}

void Brakeza::onEndComponents() const
{
    for (const auto o : sceneObjects)
        delete o;

    for (Component*& component : componentsManager->Components())
        component->onEnd();

    delete componentsManager;

    SDL_Quit();
    std::cout << "Exiting... good bye! ;)" << std::endl;
    exit(0);
}

void Brakeza::HandleAutoStartProject(bool autostart, const std::string &project) const
{
    auto render = componentsManager->getComponentRender();

    if (autostart) {
        render->getProjectLoader().LoadProject(BrakezaSetup::get()->PROJECTS_FOLDER + project);
        BrakezaSetup::get()->ENABLE_IMGUI = false;
        componentsManager->getComponentScripting()->PlayLUAScripts();
        return;
    }

    render->getSceneLoader().LoadScene(BrakezaSetup::get()->CONFIG_FOLDER + BrakezaSetup::get()->DEFAULT_SCENE);
}

void Brakeza::onUpdateSDLPollEventComponents(SDL_Event *event) const
{
    for (Component* &component : componentsManager->Components()) {
        component->onSDLPollEvent(event, BrakezaSetup::get()->EXIT);
    }
}

ComponentsManager *Brakeza::getComponentsManager() const
{
    return componentsManager;
}

Brakeza::~Brakeza()
{
    ImGui::DestroyContext();

    for (const auto o : sceneObjects)
        delete o;
}

int Brakeza::getNextObjectID() const
{
    const int id = (static_cast<int>(sceneObjects.size()) + 1) * 10;
    Logging::Message("[Brakeza] Next Object ID: %d", id);
    return id;
}

float &Brakeza::getExecutionTime()
{
    return executionTime;
}

std::string Brakeza::uniqueObjectLabel(const char *prefix)
{
    return prefix + std::string("_") + std::to_string(Tools::random(0, 100));
}

GUIManager *Brakeza::GUI() const
{
    return managerGUI;
}

Object3D *Brakeza::getSceneObjectByLabel(const std::string &label) const
{
    for (const auto sceneObject : sceneObjects) {
        if (sceneObject->getLabel() == label) {
            return sceneObject;
        }
    }

    return nullptr;
}

float Brakeza::getEngineTotalTime() const
{
    return last_ticks / 1000.f;
}

Object3D *Brakeza::getSceneObjectById(const int id) const
{
    for (const auto sceneObject : sceneObjects) {
        if (sceneObject->getId() == id) {
            return sceneObject;
        }
    }

    return nullptr;
}

void Brakeza::shutdown()
{
    BrakezaSetup::get()->EXIT = true;
}