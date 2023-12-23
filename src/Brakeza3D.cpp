#define GL_GLEXT_PROTOTYPES

#include "../include/Brakeza3D.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include <SDL2/SDL_opengl.h>

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

void Brakeza3D::start()
{
    componentsManager->registerComponent(new ComponentWindow(), "ComponentWindow");
    componentsManager->registerComponent(new ComponentCamera(), "ComponentCamera");
    componentsManager->registerComponent(new ComponentCollisions(), "ComponentCollisions");
    componentsManager->registerComponent(new ComponentInput(), "ComponentInput");
    componentsManager->registerComponent(new ComponentSound(), "ComponentSound");
    componentsManager->registerComponent(new ComponentRender(), "ComponentRender");

    // Custom components here
    componentsManager->registerComponent(new ComponentMenu(), "ComponentMenu");
    componentsManager->registerComponent(new ComponentGame(), "ComponentGame");
    componentsManager->registerComponent(new ComponentHUD(), "ComponentHUD");
    componentsManager->registerComponent(new ComponentGameInput(), "ComponentGameInput");

    mainLoop();
}

void Brakeza3D::welcomeMessage() const {
    Logging::Message("Brakeza3D - Open source game toolkit for old school lovers");
    Logging::Message("By Eduardo Rodriguez (eduardo@brakeza.com)");
    Logging::Message("https://brakeza.com");
    Logging::Message("Let's start!");

    Logging::Message("Running %s", EngineSetup::get()->ENGINE_TITLE.c_str());
}

void Brakeza3D::mainLoop()
{
    SDL_Event e;

    engineTimer.start();

    managerGUI = new GUIManager(sceneObjects);
    ComponentsManager::get()->getComponentCollisions()->initBulletSystem();

    ComponentsManager::get()->getComponentCamera()->setFreeLook(true);

    onStartComponents();

    //LoadDemo();

    SceneLoader::createParticleEmitterInScene();

    ImGuiInitialize();

    welcomeMessage();

    EngineBuffers::get()->initLUATypes();

    while (!finish) {

        controlFrameRate();

        updateTimer();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, componentsManager->getComponentWindow()->getForegroundFramebuffer());
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, componentsManager->getComponentWindow()->getUIFramebuffer());
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, componentsManager->getComponentWindow()->getBackgroundFramebuffer());
        glClear(GL_COLOR_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, componentsManager->getComponentWindow()->getSceneFramebuffer());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        preUpdateComponents();

        while (SDL_PollEvent(&e)) {
            checkForResizeOpenGLWindow(e);
            onUpdateSDLPollEventComponents(&e, finish);
            ImGui_ImplSDL2_ProcessEvent(&e);
        }

        onUpdateComponents();

        componentsManager->getComponentRender()->onUpdateSceneObjectsSecondPass();

        componentsManager->getComponentRender()->drawObjetsInHostBuffer();

        postUpdateComponents();

        if (EngineSetup::get()->IMGUI_ENABLED) ImGuiOnUpdate();

        componentsManager->getComponentWindow()->renderToWindow();
    }

    onEndComponents();

    delete componentsManager;
}

void Brakeza3D::checkForResizeOpenGLWindow(SDL_Event &e) {
    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        int width = e.window.data1;
        int height = e.window.data2;
        Logging::Message("%f, %f", width, height);
        glViewport(0,0,(GLsizei)width,(GLsizei)height);
    }
}

void Brakeza3D::controlFrameRate() const
{
    if (!EngineSetup::get()->LIMIT_FRAMERATE) return;

    const float frameDelay = 1000.0f / (float) EngineSetup::get()->FRAMERATE;

    if (deltaTime < frameDelay) {
        SDL_Delay(floor(frameDelay - deltaTime));
    }
}

std::vector<Object3D *> &Brakeza3D::getSceneObjects(){
    return sceneObjects;
}

void Brakeza3D::addObject3D(Object3D *obj, const std::string &label)
{
    Logging::Log("Adding Object3D: %s", label.c_str());
    obj->setLabel(label);
    sceneObjects.push_back(obj);
}

Timer *Brakeza3D::getTimer()
{
    return &this->engineTimer;
}

void Brakeza3D::updateTimer()
{
    current_ticks = (float) engineTimer.getTicks();
    deltaTime = current_ticks - last_ticks;
    last_ticks = current_ticks;
    executionTime += deltaTime / 1000.f;
}

float Brakeza3D::getDeltaTime() const {
    return this->deltaTime / 1000;
}

float Brakeza3D::getDeltaTimeMicro() const {
    return this->deltaTime;
}

void Brakeza3D::onStartComponents()
{
    for (Component*& component : componentsManager->components) {
        component->onStart();
    }
}

void Brakeza3D::preUpdateComponents()
{
    for (Component*& component : componentsManager->components) {
        component->preUpdate();
    }
}

void Brakeza3D::onUpdateComponents()
{
    for (Component*& component : componentsManager->components) {
        component->onUpdate();
    }
}

void Brakeza3D::postUpdateComponents()
{
    for (Component*& component : componentsManager->components) {
        component->postUpdate();
    }
}

void Brakeza3D::onEndComponents()
{
    for (Component*& component : componentsManager->components) {
        component->onEnd();
    }

    Logging::head("Exiting... good bye! ;)");
}

void Brakeza3D::onUpdateSDLPollEventComponents(SDL_Event *event, bool &finish)
{
    for (Component* &component : this->componentsManager->components) {
        component->onSDLPollEvent(event, finish);
    }
}

ComponentsManager *Brakeza3D::getComponentsManager() const
{
    return componentsManager;
}

void Brakeza3D::ImGuiOnUpdate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, ComponentsManager::get()->getComponentWindow()->getUIFramebuffer());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
    managerGUI->draw(
        Brakeza3D::get()->getDeltaTime(),
        finish
    );

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Brakeza3D::ImGuiInitialize() const
{
    Logging::Message("ImGui initialization...");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplSDL2_InitForSDLRenderer(
        getComponentsManager()->getComponentWindow()->getWindow(),
        getComponentsManager()->getComponentWindow()->getRenderer()
    );

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(getComponentsManager()->getComponentWindow()->getWindow(), getComponentsManager()->getComponentWindow()->context);
    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImGuiIO &io = ImGui::GetIO();
    io.WantCaptureMouse = false;
    io.WantCaptureKeyboard = false;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable docking

    ImGui::StyleColorsDark();

    ImGuiStyle * style = &ImGui::GetStyle();

    style->WindowPadding            = ImVec2(15, 15);
    style->WindowRounding           = 5.0f;
    style->FramePadding             = ImVec2(5, 5);
    style->FrameRounding            = 4.0f;
    style->ItemSpacing              = ImVec2(12, 8);
    style->ItemInnerSpacing         = ImVec2(8, 6);
    style->IndentSpacing            = 25.0f;
    style->ScrollbarSize            = 15.0f;
    style->ScrollbarRounding        = 9.0f;
    style->GrabMinSize              = 5.0f;
    style->GrabRounding             = 3.0f;
    style->Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.60f);

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_WindowBg]               = ImVec4(0.11f, 0.11f, 0.11f, 0.60f);
    colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
    colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_FrameBg]                = ImVec4(0.24f, 0.48f, 0.16f, 0.54f);
    colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    colors[ImGuiCol_FrameBgActive]          = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_TitleBg]                = ImVec4(0.27f, 0.38f, 0.49f, 1.00f);
    colors[ImGuiCol_TitleBgActive]          = ImVec4(0.16f, 0.29f, 0.48f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    colors[ImGuiCol_MenuBarBg]              = ImVec4(0.29f, 0.34f, 0.40f, 1.00f);
    colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    colors[ImGuiCol_CheckMark]              = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_SliderGrab]             = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
    colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Button]                 = ImVec4(0.36f, 0.48f, 0.82f, 0.40f);
    colors[ImGuiCol_ButtonHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_ButtonActive]           = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    colors[ImGuiCol_Header]                 = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
    colors[ImGuiCol_HeaderHovered]          = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    colors[ImGuiCol_HeaderActive]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
    colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
    colors[ImGuiCol_ResizeGrip]             = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
    colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    colors[ImGuiCol_Tab]                    = ImVec4(0.18f, 0.39f, 0.58f, 0.86f);
    colors[ImGuiCol_TabHovered]             = ImVec4(0.30f, 0.76f, 0.21f, 0.80f);
    colors[ImGuiCol_TabActive]              = ImVec4(0.34f, 0.68f, 0.20f, 1.00f);
    colors[ImGuiCol_TabUnfocused]           = ImVec4(0.21f, 0.36f, 0.68f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.18f, 0.24f, 0.40f, 1.00f);
    colors[ImGuiCol_DockingPreview]         = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    colors[ImGuiCol_DockingEmptyBg]         = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight]       = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
    colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
}

Brakeza3D::~Brakeza3D()
{
    ImGui::DestroyContext();

    for (auto o : sceneObjects) {
        delete o;
    }
}

float &Brakeza3D::getExecutionTime()
{
    return executionTime;
}

std::string Brakeza3D::uniqueObjectLabel(const char *prefix)
{
    return prefix + std::string("_") + std::to_string(Tools::random(0, 100));
}

void Brakeza3D::LoadDemo()
{
    auto *newObject = new Mesh3D();
    newObject->setEnabled(true);
    newObject->setBelongToScene(true);
    newObject->setStencilBufferEnabled(true);
    newObject->setRotationFrameEnabled(true);
    newObject->setRotationFrame(Vertex3D(1, 0, 0));
    newObject->setPosition(Vertex3D(0, 0, 6000));
    newObject->setScale(0.5);
    newObject->AssimpLoadGeometryFromFile(std::string(EngineSetup::get()->MODELS_FOLDER + "eye.fbx"));

    Brakeza3D::get()->addObject3D(newObject, "DemoObject3D");
}

GUIManager *Brakeza3D::getManagerGui()
{
    return managerGUI;
}

Object3D &Brakeza3D::getSceneObjectByLabel(const std::string &label)
{
    for (unsigned int i = 0; i < this->sceneObjects.size(); i++) {
        if (sceneObjects[i]->getLabel() == label) {
            return *sceneObjects[i];
        }
    }

    throw std::runtime_error("Object not found");
}

Object3D *Brakeza3D::getSceneObjectById(int i)
{
    return sceneObjects[i];
}
