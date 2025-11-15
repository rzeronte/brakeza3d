#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "../../include/Components/ComponentWindow.h"
#include "../../include/Render/Logging.h"
#include "../../include/OpenGL/ShaderOpenGLImage.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "imgui_internal.h"
#include "../../include/Brakeza3D.h"

ComponentWindow::ComponentWindow()
:
    window(nullptr),
    renderer(nullptr),
    screenSurface(nullptr),
    screenTexture(nullptr),
    fontDefault(nullptr),
    guizmoOperation(ImGuizmo::TRANSLATE),
    ImGuiConfig(ImGUIConfigs::DEFAULT),
    applicationIcon(IMG_Load(std::string(EngineSetup::get()->ICONS_FOLDER + EngineSetup::get()->iconApplication).c_str())),
    ImGuiConfigChanged(ImGUIConfigs::DEFAULT)

{
    initWindow();
    initFontsTTF();
}

void ComponentWindow::onStart()
{
    createFramebuffer();
    createGBuffer();
}

void ComponentWindow::preUpdate()
{
    SDL_GetWindowSize(window, &width, &height);
    glViewport(0,0, width, height);

    clearShadowMaps();
}

void ComponentWindow::onUpdate()
{
}

void ComponentWindow::postUpdate()
{
}

void ComponentWindow::onEnd()
{
    TTF_CloseFont(fontDefault);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(screenTexture);
    SDL_FreeSurface(screenSurface);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

void ComponentWindow::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

void ComponentWindow::initWindow()
{
    Logging::Message("Initializating ComponentWindow...");

    Logging::Message("Available video drivers:");

    for( int i = 0; i < SDL_GetNumRenderDrivers(); ++i ){
        SDL_RendererInfo rendererInfo = {};
        SDL_GetRenderDriverInfo( i, &rendererInfo );
        Logging::Message("Driver rendering: %s", rendererInfo.name);
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    } else {
        window = SDL_CreateWindow(
            SETUP->ENGINE_TITLE.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            SETUP->screenWidth,
            SETUP->screenHeight,
            SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_RESIZABLE
        );

        context = SDL_GL_CreateContext(window);
        SDL_GL_MakeCurrent(window, context);

        Logging::Message("Current video driver: %s", SDL_GetCurrentVideoDriver());
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1 );
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8 );
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8 );
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8 );
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8 );
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);

        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );

        if (window == nullptr) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            exit(-1);
        }

        screenSurface = SDL_CreateRGBSurface(0, SETUP->screenWidth, SETUP->screenHeight, 32, 0, 0, 0, 0);

        SDL_SetSurfaceBlendMode(screenSurface, SDL_BLENDMODE_MOD);

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED );

        initOpenGL();
        glewInit();
        SDL_GL_SetSwapInterval(1);
        SDL_SetWindowIcon(this->window, applicationIcon);
    }
}

void ComponentWindow::initFontsTTF()
{
    Logging::Message("Initializing TTF...");

    if (TTF_Init() < 0) {
        Logging::Message(TTF_GetError());
        exit(-1);
    }

    std::string pathFont = SETUP->FONTS_FOLDER + "TroubleFont.ttf";
    Logging::Message("Loading default TTF: %s", pathFont.c_str());

    fontDefault = TTF_OpenFont(pathFont.c_str(), 35);

    if (!fontDefault)  {
        Logging::Message(TTF_GetError());
        exit(-1);
    }
}

SDL_Window *ComponentWindow::getWindow() const {
    return window;
}

SDL_Renderer *ComponentWindow::getRenderer() const {
    return renderer;
}

TTF_Font *ComponentWindow::getFontDefault() const {
    return fontDefault;
}

GLuint ComponentWindow::getShadowMapArrayTex() const {
    return shadowMapArrayTex;
}

GLuint ComponentWindow::getSceneFramebuffer() const {
    return sceneFramebuffer;
}

GLuint ComponentWindow::getBackgroundFramebuffer() const {
    return backgroundFramebuffer;
}

GLuint ComponentWindow::getUIFramebuffer() const {
    return uiFramebuffer;
}

GLuint ComponentWindow::getForegroundFramebuffer() const {
    return foregroundFramebuffer;
}

void ComponentWindow::initOpenGL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void ComponentWindow::createFramebuffer()
{
    int w, h;
    SDL_GetRendererOutputSize(renderer, &w, &h);

    glGenFramebuffers(1, &globalFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, globalFramebuffer);

    glGenTextures(1, &globalTexture);
    glBindTexture(GL_TEXTURE_2D, globalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, globalTexture, 0);

    GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // --
    glGenFramebuffers(1, &postProcessingFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFramebuffer);

    glGenTextures(1, &postProcessingTexture);
    glBindTexture(GL_TEXTURE_2D, postProcessingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcessingTexture, 0);

    // ----

    glGenFramebuffers(1, &sceneFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);

    glGenTextures(1, &sceneTexture);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture, 0);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // ----

    glGenFramebuffers(1, &backgroundFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, backgroundFramebuffer);

    glGenTextures(1, &backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backgroundTexture, 0);

    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // ----

    glGenFramebuffers(1, &foregroundFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, foregroundFramebuffer);

    glGenTextures(1, &foregroundTexture);
    glBindTexture(GL_TEXTURE_2D, foregroundTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, foregroundTexture, 0);

    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ----
    glGenFramebuffers(1, &uiFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, uiFramebuffer);

    glGenTextures(1, &uiTexture);
    glBindTexture(GL_TEXTURE_2D, uiTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uiTexture, 0);
    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentWindow::resetFramebuffer()
{
    glDeleteFramebuffers(1, &globalFramebuffer);
    glDeleteTextures(1, &globalTexture);

    glDeleteFramebuffers(1, &sceneFramebuffer);
    glDeleteTextures(1, &sceneTexture);
    glDeleteTextures(1, &depthTexture);

    glDeleteFramebuffers(1, &backgroundFramebuffer);
    glDeleteTextures(1, &backgroundTexture);

    glDeleteFramebuffers(1, &foregroundFramebuffer);
    glDeleteTextures(1, &foregroundTexture);

    glDeleteFramebuffers(1, &uiFramebuffer);
    glDeleteTextures(1, &uiTexture);

    createFramebuffer();

    ComponentsManager::get()->getComponentRender()->resizeFramebuffers();

    resizeGBuffer();
}

void ComponentWindow::RenderLayersToGlobalFramebuffer()
{
    auto render = ComponentsManager::get()->getComponentRender();
    auto window  = ComponentsManager::get()->getComponentWindow();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (isUseDeferredRendering()) {
        auto shaderRender = render->getShaderOGLRender();

        render->getShaderOGLDeferredLighting()->render(
            gBuffer.getPositions(),
            gBuffer.getNormal(),
            gBuffer.getAlbedo(),
            shaderRender->getDirectionalLight(),
            shaderRender->getNumLightPoints(),
            shaderRender->getNumSpotLights(),
            getShadowMapArrayTex(),
            static_cast<int>(shaderRender->getShadowMappingLightPoints().size()),
            sceneFramebuffer
        );
    }

    //auto shaderShadowPassDebugLight = render->getShaderOGLShadowPassDebugLight();
    //shaderShadowPassDebugLight->render(getShadowMapArrayTex(), 0, getSceneFramebuffer());
    //shaderShadowPassDebugLight->renderInternal(getShadowMapArrayTex(), 0);

    auto shaderOGLImage = render->getShaderOGLImage();
    shaderOGLImage->renderTexture(backgroundTexture, 0, 0, width, height, 1, true, globalFramebuffer);
    shaderOGLImage->renderTexture(sceneTexture, 0, 0, width, height, 1, true, globalFramebuffer);
    shaderOGLImage->renderTexture(postProcessingTexture, 0, 0, width, height, 1, true, globalFramebuffer);

    if (EngineSetup::get()->ENABLE_FOG) {
        auto shaderOGLFOG = render->getShaderOGLFOG();
        shaderOGLFOG->render(globalTexture, depthTexture);
        shaderOGLImage->renderTexture(shaderOGLFOG->getTextureResult(), 0, 0, width, height, 1, false, globalFramebuffer);
    }

    if (EngineSetup::get()->ENABLE_DEPTH_OF_FIELD) {
        auto shaderOGLDOF = render->getShaderOGLDOF();
        shaderOGLDOF->render(globalTexture, depthTexture);
        shaderOGLImage->renderTexture(shaderOGLDOF->getTextureResult(), 0, 0, width, height, 1, false, globalFramebuffer);
    }

    if (EngineSetup::get()->SHOW_DEPTH_OF_FIELD) {
        auto shaderOGLDepthMap = render->getShaderOGLDepthMap();
        shaderOGLDepthMap->render(depthTexture, globalFramebuffer);
    }
}

void ComponentWindow::RenderLayersToMain() const
{
    auto render = ComponentsManager::get()->getComponentRender();
    auto shaderOGLImage = render->getShaderOGLImage();

    shaderOGLImage->renderTexture(foregroundTexture, 0, 0, width, height, 1, true, globalFramebuffer);
    shaderOGLImage->renderTexture(globalTexture, 0, 0, width, height, 1, true, 0);
    shaderOGLImage->renderTexture(uiTexture, 0, 0, width, height, 1, true, 0);

    SDL_GL_SwapWindow(window);
    cleanFrameBuffers();
}

void ComponentWindow::cleanFrameBuffers() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, postProcessingFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, foregroundFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, uiFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, backgroundFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, globalFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int ComponentWindow::getWidth() const {
    return width;
}

int ComponentWindow::getHeight() const {
    return height;
}

GLuint ComponentWindow::getGlobalFramebuffer() const {
    return globalFramebuffer;
}

GLuint ComponentWindow::getPostProcessingFramebuffer() const {
    return postProcessingFramebuffer;
}

void ComponentWindow::saveImGuiCurrentLayout() const
{
    switch(ImGuiConfigChanged) {
        case ImGUIConfigs::DEFAULT: {
            ImGui::SaveIniSettingsToDisk(std::string(EngineSetup::get()->CONFIG_FOLDER + "ImGuiDefault.ini").c_str());
            Logging::Message("Saving to ImGUIDefault.ini");
            break;
        }
        case ImGUIConfigs::CODING: {
            ImGui::SaveIniSettingsToDisk(std::string(EngineSetup::get()->CONFIG_FOLDER + "ImGuiCoding.ini").c_str());
            Logging::Message("Saving to ImGuiCoding.ini");
            break;
        }
        case ImGUIConfigs::DESIGN: {
            ImGui::SaveIniSettingsToDisk(std::string(EngineSetup::get()->CONFIG_FOLDER + "ImGuiDesign.ini").c_str());
            Logging::Message("Saving to ImGuiDesign.ini");
            break;
        }
    }
}

void ComponentWindow::ImGuiInitialize(const std::string& configFile) {
    Logging::Message("ImGui initialization...");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    const char* glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImGuiIO &io = ImGui::GetIO();
    io.WantCaptureMouse = false;
    io.WantCaptureKeyboard = false;
    io.WantSaveIniSettings = true;
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

    ImGui::ClearIniSettings();
    ImGui::LoadIniSettingsFromDisk(std::string(configFile).c_str());
    ImGui::GetStyle().Alpha = 1.0f;
    ImGuiConfig = ImGUIConfigs::DEFAULT;
}

void ComponentWindow::ImGuiOnUpdate()
{
    if (ImGuiConfig != ImGuiConfigChanged) {
        switch(ImGuiConfigChanged) {
            case ImGUIConfigs::DEFAULT: {
                ImGui::ClearIniSettings();
                Logging::Message("Loading layout ImGUIDefault.ini");
                ImGui::LoadIniSettingsFromDisk(std::string(EngineSetup::get()->CONFIG_FOLDER + "ImGuiDefault.ini").c_str());
                break;
            }
            case ImGUIConfigs::CODING: {
                ImGui::ClearIniSettings();
                Logging::Message("Loading layout ImGuiCoding.ini");
                ImGui::LoadIniSettingsFromDisk(std::string(EngineSetup::get()->CONFIG_FOLDER + "ImGuiCoding.ini").c_str());
                break;
            }
            case ImGUIConfigs::DESIGN: {
                ImGui::ClearIniSettings();
                Logging::Message("Loading layout ImGuiDesign.ini");
                ImGui::LoadIniSettingsFromDisk(std::string(EngineSetup::get()->CONFIG_FOLDER + "ImGuiDesign.ini").c_str());
                break;
            }
        }
        ImGuiConfig = ImGuiConfigChanged;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, getUIFramebuffer());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
    Brakeza3D::get()->getManagerGui()->draw(Brakeza3D::get()->getDeltaTime(),Brakeza3D::get()->finish);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool ComponentWindow::isWindowMaximized() const {
    Uint32 flags = SDL_GetWindowFlags(window);

    // Verifica si el flag SDL_WINDOW_MAXIMIZED está activo
    return (flags & SDL_WINDOW_MAXIMIZED) != 0;
}

void ComponentWindow::setWindowTitle(const char *title) const {
    SDL_SetWindowTitle(window, title);
}

void ComponentWindow::toggleFullScreen() const {
    if (EngineSetup::get()->FULLSCREEN) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window, 0);
    }
}

ImGuizmo::OPERATION ComponentWindow::getGuizmoOperation() const
{
    return guizmoOperation;
}

void ComponentWindow::setGuizmoOperation(ImGuizmo::OPERATION guizmoOperation)
{
    ComponentWindow::guizmoOperation = guizmoOperation;
}

GLuint ComponentWindow::getDepthTexture() const
{
    return depthTexture;
}

GLuint ComponentWindow::getSceneTexture() const
{
    return sceneTexture;
}

GLuint ComponentWindow::getGlobalTexture() const
{
    return globalTexture;
}

GBuffer& ComponentWindow::getGBuffer()
{
    return this->gBuffer;
}

void ComponentWindow::createGBuffer()
{
    auto window = ComponentsManager::get()->getComponentWindow();

    int width = window->getWidth();
    int height = window->getHeight();

    SDL_GetRendererOutputSize(window->getRenderer(), &width, &height);

    // Crear el framebuffer
    glGenFramebuffers(1, &gBuffer.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FBO);

    // Crear texturas del G-Buffer
    // --- Posición ---
    glGenTextures(1, &gBuffer.gPosition);
    glBindTexture(GL_TEXTURE_2D, gBuffer.gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer.gPosition, 0);

    // --- Normal ---
    glGenTextures(1, &gBuffer.gNormal);
    glBindTexture(GL_TEXTURE_2D, gBuffer.gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBuffer.gNormal, 0);

    // --- Albedo + Specular ---
    glGenTextures(1, &gBuffer.gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gBuffer.gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBuffer.gAlbedoSpec, 0);

    // Indicar qué buffers se van a escribir
    GLuint attachments[3] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
    };
    glDrawBuffers(3, attachments);

    // Buffer de profundidad (Renderbuffer)
    glGenRenderbuffers(1, &gBuffer.rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, gBuffer.rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gBuffer.rboDepth);

    // Comprobar si está completo
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[ERROR] G-Buffer: Framebuffer no está completo!" << std::endl;
    } else {
        std::cout << "G-Buffer creado correctamente (" << width << "x" << height << ")" << std::endl;
    }

    // Desvincular
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentWindow::createShadowMapBuffers(int numLights)
{
    auto window = ComponentsManager::get()->getComponentWindow();

    const int w = window->getWidth();
    const int h = window->getHeight();

    glGenTextures(1, &shadowMapArrayTex);
    glBindTexture(GL_TEXTURE_2D_ARRAY, shadowMapArrayTex);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32, w, h, numLights,0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Configuración
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    float borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, borderColor);
}

void ComponentWindow::resizeGBuffer()
{
    glDeleteFramebuffers(1, &gBuffer.FBO);
    glDeleteTextures(1, &gBuffer.gAlbedoSpec);
    glDeleteTextures(1, &gBuffer.gNormal);
    glDeleteTextures(1, &gBuffer.gPosition);
    glDeleteRenderbuffers(1, &gBuffer.rboDepth);

    createGBuffer();
}

bool ComponentWindow::isUseDeferredRendering() const {
    return !EngineSetup::get()->FORWARD_RENDER;
}

void ComponentWindow::setUseDeferredRendering(const bool use) {
    EngineSetup::get()->FORWARD_RENDER = !use;
    Logging::Message("Deferred Rendering: %s", use ? "ENABLED" : "DISABLED");
}

void ComponentWindow::clearShadowMaps()
{
  auto render = ComponentsManager::get()->getComponentRender();
   auto shaderRender = render->getShaderOGLRender();

    auto numLights = (int) shaderRender->getShadowMappingLightPoints().size();
    if (numLights > 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, render->getShaderOGLShadowPass()->getShadowFBO());
        glClear(GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < numLights; i++) {
            glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, getShadowMapArrayTex(), 0, i);
            glClear(GL_DEPTH_BUFFER_BIT);
        }
    }

}