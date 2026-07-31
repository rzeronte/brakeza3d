#define GL_GLEXT_PROTOTYPES

#include "../imgui/imgui.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
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
    componentsManager =
        Components::get();

    pool.setMaxCallbacksPerFrame(8);
    pool.setMaxConcurrentTasks(4);

    poolImages.setMaxCallbacksPerFrame(8);
    poolImages.setMaxConcurrentTasks(4);
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
    auto objects = Brakeza::get()->copySceneObjects();

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
        if (Config::get()->OBSERVER_AI_ENABLED) EngineObserver::frameCount++;

        Profiler::get()->ResetTotalFrameTime();                              // Reset profiler measures

        ControlFrameRate();                                                  // Control framerate based on SDL_Delay
        UpdateTimer();                                                       // Refresh main timer
        PoolImages().processMainThreadCallbacks();                           // Main Thread pool images
        PoolCompute().processMainThreadCallbacks();                          // Main Thread pool compute

        if (Config::get()->OBSERVER_AI_ENABLED) EngineObserver::setPipelineStep("PreUpdate");
        PreUpdateComponents();                                               // PreUpdate for componentes
        CaptureInputEvents(event);                                        // Capture keyboard/mouse status

        if (Config::get()->ENABLE_IMGUI) {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            ImGui::NewFrame();
            Components::get()->Render()->DrawSelectionRectFill();
            Brakeza::get()->GUI()->DrawGUI();
        }

        Components::get()->Window()->ClearOGLFrameBuffers();                 // Clean video framebuffers

        if (Config::get()->OBSERVER_AI_ENABLED) EngineObserver::setPipelineStep("OnUpdate+ObjectShaders");
        OnUpdateComponents();                                                // OnUpdate for componentes

        if (Config::get()->OBSERVER_AI_ENABLED) EngineObserver::setPipelineStep("ShadowPass");
        Components::get()->Render()->RunShadowPass();                        // Centralised shadow pass (all casters × all lights)

        if (Config::get()->OBSERVER_AI_ENABLED) EngineObserver::setPipelineStep("LightPass");
        Components::get()->Render()->LightPass();                            // Deferred opaque objects light pass

        if (Config::get()->OBSERVER_AI_ENABLED) EngineObserver::setPipelineStep("PostUpdate");
        PostUpdateComponents();                                              // PostUpdate for componentes (transparent mainly)

        if (Config::get()->OBSERVER_AI_ENABLED) EngineObserver::setPipelineStep("FlipBuffersToGlobal");
        Components::get()->Render()->FlipBuffersToGlobal();                  // Buffers compositing

        if (Config::get()->OBSERVER_AI_ENABLED) EngineObserver::setPipelineStep("PostProcessingChain");
        ComponentRender::PostProcessingShadersChain();                       // Post-pass running for shaders

        if (Config::get()->OBSERVER_AI_ENABLED) EngineObserver::setPipelineStep("FlipToWindow");
        Profiler::get()->EndTotalFrameTime();                                // End frame time measures
        Components::get()->Window()->FlipGlobalToWindow();                   // Flip to screen

        if (Config::get()->OBSERVER_AI_ENABLED) EngineObserver::setPipelineStep("idle");

        if (Config::get()->OBSERVER_AI_ENABLED && EngineObserver::frameCount % 60 == 0)
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
    {
        std::unique_lock lock(objectsMutex);
        objects.push_back(obj);
        objectsByName[label] = obj;
        objectsById[obj->getId()] = obj;
    }

    obj->ReloadScriptsEnvironment();
    if (componentsManager->Scripting()->isExecuting()) {
        obj->RunStartScripts();
    }
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
        printf("[Brakeza] ProjectLoader::LoadProject START\n"); fflush(stdout);
        ProjectLoader::LoadProject(Config::get()->PROJECTS_FOLDER + cliOptions.project);
        printf("[Brakeza] ProjectLoader::LoadProject DONE\n"); fflush(stdout);
        Config::get()->ENABLE_IMGUI = false;
        printf("[Brakeza] PlayLUAScripts START\n"); fflush(stdout);
        componentsManager->Scripting()->PlayLUAScripts();
        printf("[Brakeza] PlayLUAScripts DONE\n"); fflush(stdout);
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
    std::shared_lock lock(objectsMutex);
    auto it = objectsByName.find(label);
    if (it != objectsByName.end()) return it->second;
    return nullptr;
}

void Brakeza::removeObjectFromIndex(Object3D *obj)
{
    auto it = objectsByName.find(obj->getName());
    if (it != objectsByName.end() && it->second == obj)
        objectsByName.erase(it);
    objectsById.erase(obj->getId());
}

Object3D *Brakeza::getObjectById(const unsigned int id) const
{
    std::shared_lock lock(objectsMutex);
    auto it = objectsById.find(id);
    return it != objectsById.end() ? it->second : nullptr;
}

Object3D *Brakeza::getObjectByIndex(int index) const
{
    std::shared_lock lock(objectsMutex);
    return objects[index];
}

Object3D *Brakeza::getObjectAtScreen(int rawX, int rawY) const
{
    auto *window = Components::get()->Window();
    int renderX = (int)((float)rawX / (float)window->getWidth()  * (float)window->getWidthRender());
    int renderY = (int)((float)rawY / (float)window->getHeight() * (float)window->getHeightRender());
    unsigned int id = window->getObjectIDByPickingColorFramebuffer(renderX, renderY);
    Object3D *obj = getObjectById(id);
    if (obj == nullptr) {
        obj = Components::get()->Render()->hitTestAvatar(rawX, rawY);
    }
    return obj;
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
