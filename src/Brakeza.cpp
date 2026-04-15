#define GL_GLEXT_PROTOTYPES

#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../cxxxopts/cxxxopts.h"
#include "../include/Brakeza.h"
#include "../include/Components/Components.h"
#include "../include/GUI/Objects/FileSystemGUI.h"
#include "../include/Render/Profiler.h"
#include "../include/Render/EngineObserver.h"
#include "../include/Loaders/SceneLoader.h"
#include "../include/SceneObjectTypes.h"
#include "../include/Misc/cJSON.h"

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

    GUI::CLIWelcomeMessage();
    GUI::ShowLoadTime("Time until components initialization", timer);

    OnStartComponents();             // Starting componentes
    AutoLoadProjectOrContinue();     // Parse CLI options
    EngineObserver::init(Config::get()->ROOT_FOLDER);

    // Profiler tags
    Profiler::InitMeasure(Profiler::get()->getComponentMeasures(), "LightPass");
    Profiler::InitMeasure(Profiler::get()->getComponentMeasures(), "FlipBuffersToGlobal");
    Profiler::InitMeasure(Profiler::get()->getComponentMeasures(), "PostProcessingShadersChain");
}

static void DumpObserverState()
{
    auto *render  = Components::get()->Render();
    auto &objects = Brakeza::get()->getSceneObjects();

    int lights = 0;
    for (auto *o : objects) {
        if (o->getTypeObject() == ObjectType::LightPoint ||
            o->getTypeObject() == ObjectType::LightSpot)
            lights++;
    }

    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "frame",         EngineObserver::frameCount);
    cJSON_AddStringToObject(root, "scene",         EngineObserver::currentScene.c_str());
    cJSON_AddStringToObject(root, "pipeline_step", EngineObserver::pipelineStep.c_str());
    cJSON_AddNumberToObject(root, "fps",           render->getFps());
    cJSON_AddNumberToObject(root, "objects",       (int)objects.size());
    cJSON_AddNumberToObject(root, "lights",        lights);
    cJSON_AddBoolToObject(root,   "scene_loading", SceneLoader::isLoading);
    cJSON_AddBoolToObject(root,   "physics_on",    !Config::get()->BULLET_DEBUG_MODE);

    cJSON *errs = cJSON_AddArrayToObject(root, "recent_errors");
    for (const auto &e : EngineObserver::recentErrors)
        cJSON_AddItemToArray(errs, cJSON_CreateString(e.c_str()));

    char *s = cJSON_Print(root);
    std::ofstream f(EngineObserver::statePath);
    if (f.is_open()) { f << s; }
    free(s);
    cJSON_Delete(root);
}

void Brakeza::MainLoop()
{
    SDL_Event event;

    GUI::ShowLoadTime("Time until main loop starts", timer);

    while (!Config::get()->EXIT) {
        EngineObserver::frameCount++;

        Profiler::get()->ResetTotalFrameTime();                              // Reset profiler measures
        ControlFrameRate();                                                  // Control framerate based on SDL_Delay
        UpdateTimer();                                                       // Refresh main timer
        PoolImages().processMainThreadCallbacks();                           // Main Thread pool images
        PoolCompute().processMainThreadCallbacks();                          // Main Thread pool compute

        EngineObserver::setPipelineStep("PreUpdate");
        PreUpdateComponents();                                               // PreUpdate for componentes
        CaptureInputEvents(event);                                           // Capture keyboard/mouse status
        Components::get()->Window()->ClearOGLFrameBuffers();                 // Clean video framebuffers

        EngineObserver::setPipelineStep("OnUpdate+ObjectShaders");
        OnUpdateComponents();                                                // OnUpdate for componentes

        EngineObserver::setPipelineStep("LightPass");
        Components::get()->Render()->LightPass();                            // Deferred opaque objects light pass

        EngineObserver::setPipelineStep("PostUpdate");
        PostUpdateComponents();                                              // PostUpdate for componentes (transparent mainly)

        EngineObserver::setPipelineStep("FlipBuffersToGlobal");
        Components::get()->Render()->FlipBuffersToGlobal();                  // Buffers compositing

        EngineObserver::setPipelineStep("PostProcessingChain");
        ComponentRender::PostProcessingShadersChain();                       // Post-pass running for shaders

        EngineObserver::setPipelineStep("FlipToWindow");
        Profiler::get()->EndTotalFrameTime();                                // End frame time measures
        Components::get()->Window()->FlipGlobalToWindow();                   // Flip to screen

        EngineObserver::setPipelineStep("idle");

        if (EngineObserver::frameCount % 60 == 0)
            DumpObserverState();
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
    FileSystemGUI::autoExpandScene = false;
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
