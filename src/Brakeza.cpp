#define GL_GLEXT_PROTOTYPES

#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../cxxxopts/cxxxopts.h"
#include "../include/Brakeza.h"
#include "../include/Components/Components.h"
#include "../include/Render/Profiler.h"

Brakeza *Brakeza::instance = nullptr;

Brakeza::Brakeza()
:
    pool(4),
    poolImages(4)
{
    componentsManager = Components::get();

    pool.setMaxCallbacksPerFrame(5);
    pool.setMaxConcurrentTasks(2);

    poolImages.setMaxCallbacksPerFrame(5);
    poolImages.setMaxConcurrentTasks(2);
}

Brakeza *Brakeza::get()
{
    if (instance == nullptr) {
        instance = new Brakeza();
    }

    return instance;
}

void Brakeza::Start(int argc, char *argv[])
{
    if (ReadArgs(argc, argv)) return;
    RegisterComponents();
    PreMainLoop();
    MainLoop();
}

void Brakeza::RegisterComponents() const
{
    componentsManager->RegisterComponent(new ComponentWindow(), "Window");
    componentsManager->RegisterComponent(new ComponentScripting(), "Scripting");
    componentsManager->RegisterComponent(new ComponentCamera(), "Camera");
    componentsManager->RegisterComponent(new ComponentCollisions(), "Collisions");
    componentsManager->RegisterComponent(new ComponentInput(), "Input");
    componentsManager->RegisterComponent(new ComponentSound(), "Sound");
    componentsManager->RegisterComponent(new ComponentRender(), "Render");
}

void Brakeza::PreMainLoop()
{
    timer.start();
    GUI()->OnStart();

    GUI::WelcomeMessage();
    GUI::ShowLoadTime("Time until components initialization", timer);

    OnStartComponents();             // Starting componentes
    AutoLoadProjectOrContinue();     // Parse CLI options

    // Profiler tags
    Profiler::InitMeasure(Profiler::get()->getComponentMeasures(), "RenderLayersToGlobal");
    Profiler::InitMeasure(Profiler::get()->getComponentMeasures(), "RunShadersOpenGLPreUpdate");
    Profiler::InitMeasure(Profiler::get()->getComponentMeasures(), "RunShadersOpenGLPostUpdate");
    Profiler::InitMeasure(Profiler::get()->getComponentMeasures(), "FlipBuffersToGlobal");
}

void Brakeza::MainLoop()
{
    SDL_Event event;

    GUI::ShowLoadTime("Time until main loop starts", timer);

    while (!Config::get()->EXIT) {
        Profiler::get()->ResetTotalFrameTime();                             // Reset profiler measures
        ControlFrameRate();                                                 // Control framerate based on SDL_Delay
        UpdateTimer();                                                      // Refresh main timer
        PoolImages().processMainThreadCallbacks();                          // Main Thread pool images
        PoolCompute().processMainThreadCallbacks();                         // Main Thread pool compute
        PreUpdateComponents();                                              // PreUpdate for componentes
        CaptureInputEvents(event);                                       // Capture keyboard/mouse status
        Components::get()->Window()->ClearOGLFrameBuffers();                // Clean video framebuffers
        OnUpdateComponents();                                               // OnUpdate for componentes
        Components::get()->Render()->FlipBuffersToGlobal();                 // Buffers compositing
        ComponentRender::RunSceneShadersPostUpdate();                       // Post-pass running for shaders
        PostUpdateComponents();                                             // PostUpdate for componentes
        Profiler::get()->EndTotalFrameTime();                               // End frame time measures
        Components::get()->Window()->FlipGlobalToWindow();                  // Flip to screen
    }

    onEndComponents();
}

void Brakeza::CaptureInputEvents(SDL_Event &e) const
{

    while (SDL_PollEvent(&e)) {
        Components::get()->Window()->CheckForResizeOpenGLWindow(e);
        onUpdateSDLPollEventComponents(&e);
        ImGui_ImplSDL2_ProcessEvent(&e);
    }
}

void Brakeza::ControlFrameRate() const
{
    if (!Config::get()->LIMIT_FRAMERATE) return;

    const float frameDelay = 1000.0f / static_cast<float>(Config::get()->FRAMERATE);
    if (deltaTime < frameDelay) {
        SDL_Delay(floor(frameDelay - deltaTime));
    }
}

void Brakeza::AddObject3D(Object3D *obj, const std::string &label)
{
    LOG_MESSAGE("[AddObject] Adding object '%s' to scene...", label.c_str());
    obj->setName(label);
    objects.push_back(obj);
}

void Brakeza::UpdateTimer()
{
    current_ticks = static_cast<float>(timer.getTicks());
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;
    executionTime += deltaTime / 1000.f;
}

void Brakeza::OnStartComponents() const
{
    for (auto &c : componentsManager->getComponents())
        c->onStart();

    Config::get()->ENABLE_LOGGING_STD = false;
    GUI::ShowLoadTime("Time until the components get ready", timer);
}

void Brakeza::PreUpdateComponents() const
{
    for (auto &c : componentsManager->getComponents()) {
        Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), c->getLabel() + ProfilerConstants::SUFFIX_PRE);
        c->preUpdate();
        Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), c->getLabel() + ProfilerConstants::SUFFIX_PRE);
    }
}

void Brakeza::OnUpdateComponents() const
{
    for (auto &c : componentsManager->getComponents()) {
        Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), c->getLabel() + ProfilerConstants::SUFFIX_UPDATE);
        c->onUpdate();
        Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), c->getLabel() + ProfilerConstants::SUFFIX_UPDATE);
    }
}

void Brakeza::PostUpdateComponents() const
{
    for (auto &c: componentsManager->getComponents()) {
        Profiler::StartMeasure(Profiler::get()->getComponentMeasures(), c->getLabel() + ProfilerConstants::SUFFIX_POST);
        c->postUpdate();
        Profiler::EndMeasure(Profiler::get()->getComponentMeasures(), c->getLabel() + ProfilerConstants::SUFFIX_POST);
    }
}

void Brakeza::onEndComponents() const
{
    for (const auto &o : objects)
        delete o;

    for (Component*& component : componentsManager->getComponents())
        component->onEnd();

    delete componentsManager;

    SDL_Quit();
    std::cout << "Exiting... good bye! ;)" << std::endl;
    exit(0);
}

void Brakeza::AutoLoadProjectOrContinue() const
{
    if (cliOptions.autoload) {
        ProjectLoader::LoadProject(Config::get()->PROJECTS_FOLDER + cliOptions.project);
        Config::get()->ENABLE_IMGUI = false;
        componentsManager->Scripting()->PlayLUAScripts();
        return;
    }

    SceneLoader::LoadScene(Config::get()->CONFIG_FOLDER + Config::get()->DEFAULT_SCENE);
}

void Brakeza::onUpdateSDLPollEventComponents(SDL_Event *event) const
{
    for (Component* &component : componentsManager->getComponents())
        component->onSDLPollEvent(event, Config::get()->EXIT);
}

unsigned int Brakeza::getNextUniqueObjectId()
{
    static unsigned int counter = 0;
    return ++counter;
}

std::string Brakeza::UniqueObjectLabel(const char *prefix)
{
    return prefix + std::string("_") + std::to_string(get()->getTimer()->getTicks());
}

Object3D *Brakeza::getObjectByName(const std::string &label) const
{
    for (const auto &o : objects) {
        if (o->getName() == label) {
            return o;
        }
    }

    return nullptr;
}

Object3D *Brakeza::getObjectById(const unsigned int id) const
{
    for (const auto &o : objects) {
        if (o->getId() == id) {
            return o;
        }
    }

    return nullptr;
}

bool Brakeza::ReadArgs(int argc, char **argv)
{
    cxxopts::Options options(
        "Brakeza3D",
        "Thanks for using Brakeza3D!. Here you can see argument's options:"
    );

    options.add_options()
        ("p,project", "Project file", cxxopts::value<std::string>())
        ("h,help", "Help")
    ;

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return true;
    }

    cliOptions.autoload = false;

    if (result.count("p")) {
        cliOptions.autoload = true;
        cliOptions.project = result["p"].as<std::string>();
        LOG_MESSAGE("[Brakeza] Autoload project: %s", cliOptions.project.c_str());
    }

    return false;
}

Brakeza::~Brakeza()
{
    ImGui::DestroyContext();

    for (const auto o : objects)
        delete o;
}