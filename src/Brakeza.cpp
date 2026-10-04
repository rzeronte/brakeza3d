#define GL_GLEXT_PROTOTYPES

#include <algorithm>
#include <atomic>
#include <thread>
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

namespace {
    // Workers del pool de cómputo (carga de modelos/animaciones con Assimp). Antes fijo a 4 en una
    // CPU de 12 hilos: la carga de la partida esperaba en cola con núcleos libres (ver
    // .claude/memory/loading-profile-report.md). Se dejan 2 hilos al principal/driver, y tope 8:
    // cada parseo de un FBX de personaje (~32 MB) ocupa bastante memoria mientras dura.
    size_t computeWorkerCount()
    {
        const unsigned hw = std::thread::hardware_concurrency();
        if (hw == 0) return 4;
        return std::clamp<size_t>(hw > 2 ? hw - 2 : 1, 4, 8);
    }
}

Brakeza::Brakeza()
:
    pool(computeWorkerCount()),
    poolImages(4)
{
    componentsManager =
        Components::get();

    pool.setMaxCallbacksPerFrame(8);
    pool.setMaxConcurrentTasks(computeWorkerCount());

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

    // EngineObserver::init() ANTES de AutoLoadProjectOrContinue(): con autoload, esa llamada ya
    // dispara ProjectLoader::LoadProject + PlayLUAScripts() (onStart de TODOS los scripts) -- si
    // init() corría después (como antes de este cambio), un crash durante esa carga ocurría con
    // eventsFile todavía sin abrir y brakeza_events.jsonl quedaba vacío, inútil para diagnosticar.
    EngineObserver::init(Config::get()->ROOT_FOLDER);
    AutoLoadProjectOrContinue();     // Parse CLI options

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
        if (cliOptions.exitAfterSeconds > 0.0f && executionTime >= cliOptions.exitAfterSeconds) {
            LOG_MESSAGE("[Brakeza] --exit-after %.1fs reached, shutting down", cliOptions.exitAfterSeconds);
            requestExit(2); // 2 = watchdog timeout, distinguishable from a project-driven requestExit()
            break;
        }

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
    // Deliberately NOT deleting `objects` here. This function only runs once, right after
    // the main loop exits on Config::EXIT -- i.e. exclusively on a full app quit (Shutdown()
    // is the only thing that sets EXIT), never on in-scene transitions (those go through
    // removeAllObjects(), a completely separate deferred-removal path). exit(0) below tears
    // down the whole process anyway, so the OS reclaims GPU/audio/heap memory regardless of
    // whether we free it ourselves first. Deleting every live Object3D individually used to
    // do a per-object Bullet removeCollisionObject() (O(n) linear search each, O(n^2) total)
    // plus unbatched glDeleteBuffers/glDeleteTextures/logging per mesh -- with a busy RTS
    // scene (units/buildings/civilians/traffic/emitters) that made "Quit to desktop" take
    // several seconds. Component::onEnd() below still runs (cheap: closes the audio device
    // and destroys the window/renderer) so the app closes visibly clean.
    for (Component*& component : componentsManager->getComponents())
        component->onEnd();

    delete componentsManager;

    SDL_Quit();
    std::cout << "Exiting... good bye! ;)" << std::endl;
    exit(exitCode);
}

void Brakeza::AutoLoadProjectOrContinue() const
{
    if (cliOptions.autoload) {
        printf("[Brakeza] ProjectLoader::LoadProject START\n"); fflush(stdout);
        ProjectLoader::LoadProject(Config::get()->PROJECTS_FOLDER + cliOptions.project);
        printf("[Brakeza] ProjectLoader::LoadProject DONE\n"); fflush(stdout);
        Config::get()->ENABLE_IMGUI = false;
        // El backend SDL2 de ImGui desactiva la captura automática del ratón de SDL al inicializarse
        // (imgui_impl_sdl2.cpp: SDL_HINT_MOUSE_AUTO_CAPTURE = "0") porque la hace él por frame con
        // SDL_CaptureMouse() en ImGui_ImplSDL2_NewFrame(). Con ImGui apagado ese NewFrame ya no corre,
        // así que nadie capturaba: soltar el botón fuera de la ventana no llegaba y el arrastre /
        // la selección por recuadro se quedaban "pegados" al volver. SDL relee este hint en caliente.
        SDL_SetHint(SDL_HINT_MOUSE_AUTO_CAPTURE, "1");
        // Autoload runs have no ImGui/in-app console to look at, so mirror LOG_MESSAGE/Lua
        // print() (routed through Logging::Message) to stdout instead of the default
        // interactive-mode behaviour (silent after startup, GUI console only) -- otherwise a
        // headless/CI run (see tools/run_scenario.ps1) produces no visible output at all.
        Config::get()->ENABLE_LOGGING_STD = true;
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
    // atomic: AnimationData::cloneInto/ModelData::cloneInto llaman a esto desde
    // ThreadJobLoadMesh3DAnimation::fnProcess() (worker thread, hasta 4 en paralelo,
    // ver Brakeza::pool.setMaxConcurrentTasks(4)) cuando varios civiles spawnean a la vez
    // y comparten el mismo FBX de animacion en cache (cache-HIT -> clone). Un
    // 'static unsigned int' + '++' sin atomic es una data race real (UB) que puede hacer
    // que dos objetos distintos acaben con el mismo id.
    static std::atomic<unsigned int> counter{0};
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
        ("set", "Generic key=value CLI param, repeatable. Meaning is defined entirely by the "
                "loaded project's own 'cli_params' declaration -- the engine never interprets it.",
                cxxopts::value<std::vector<std::string>>())
        ("exit-after", "Force-quit N seconds after startup (safety watchdog for headless/automated runs)",
                cxxopts::value<float>())
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

    cliOptions.exitAfterSeconds = result.count("exit-after") ? result["exit-after"].as<float>() : 0.0f;

    cliOptions.rawParams.clear();
    if (result.count("set")) {
        for (auto &kv : result["set"].as<std::vector<std::string>>()) {
            auto pos = kv.find('=');
            if (pos == std::string::npos) {
                LOG_WARNING("[Brakeza] --set ignored, expected key=value: '%s'", kv.c_str());
                continue;
            }
            cliOptions.rawParams[kv.substr(0, pos)] = kv.substr(pos + 1);
            LOG_MESSAGE("[Brakeza] --set %s=%s", kv.substr(0, pos).c_str(), kv.substr(pos + 1).c_str());
        }
    }

    return false;
}

Brakeza::~Brakeza()
{
    ImGui::DestroyContext();

    for (const auto o : objects)
        delete o;
}
