#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL_image.h>
#include "imgui_internal.h"
#include "../../include/Components/ComponentWindow.h"
#include "../../include/Misc/Logging.h"
#include "../../include/OpenGL/ShaderOGLImage.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../../include/Brakeza.h"

ComponentWindow::ComponentWindow()
:
    applicationIcon(IMG_Load(std::string(BrakezaSetup::get()->ICONS_FOLDER + BrakezaSetup::get()->iconApplication).c_str()))
{
    initWindow();
    initFontsTTF();
}

void ComponentWindow::onStart()
{
    Component::onStart();

    ImGuiInitialize(BrakezaSetup::get()->CONFIG_FOLDER + "ImGuiDefault.ini");

    createFramebuffer();
    createGBuffer();
    createPickingColorBuffer();
}

void ComponentWindow::preUpdate()
{
    Component::preUpdate();

    UpdateWindowSize();
    glViewport(0,0, widthWindow, heightWindow);
}

void ComponentWindow::onUpdate()
{
    Component::onUpdate();
}

void ComponentWindow::postUpdate()
{
    Component::postUpdate();
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

void ComponentWindow::onSDLPollEvent(SDL_Event *event, bool &finish)
{
    ComponentsManager::get()->getComponentRender()->updateSelectedObject3D();
}

void ComponentWindow::initWindow()
{
    Logging::Message("[ComponentWindow] Init window...");

    Logging::Message("[ComponentWindow] Render drivers:");

    for( int i = 0; i < SDL_GetNumRenderDrivers(); ++i ){
        SDL_RendererInfo rendererInfo = {};
        SDL_GetRenderDriverInfo( i, &rendererInfo );
        Logging::Message(" > Driver rendering: %s", rendererInfo.name);
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("[ComponentWindow] ERROR: SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    }

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

    Logging::Message("[ComponentWindow] Current video driver: %s", SDL_GetCurrentVideoDriver());
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
    SDL_GetRendererOutputSize(renderer, &widthRender, &heightRender);

    initOpenGL();
    glewInit();
    SDL_GL_SetSwapInterval(1);
    SDL_SetWindowIcon(window, applicationIcon);
}

void ComponentWindow::initFontsTTF()
{
    Logging::Message("[ComponentWindow] Init TrueTypeFonts...");

    if (TTF_Init() < 0) {
        Logging::Message(TTF_GetError());
        exit(-1);
    }

    std::string pathFont = SETUP->FONTS_FOLDER + "TroubleFont.ttf";
    Logging::Message("[ComponentWindow] Loading default TTF: %s", pathFont.c_str());

    fontDefault = TTF_OpenFont(pathFont.c_str(), 35);

    if (!fontDefault) {
        Logging::Message(TTF_GetError());
        exit(-1);
    }
}

SDL_Window *ComponentWindow::getWindow() const
{
    return window;
}

SDL_Renderer *ComponentWindow::getRenderer() const
{
    return renderer;
}

TTF_Font *ComponentWindow::getFontDefault() const
{
    return fontDefault;
}

GLuint ComponentWindow::getSceneFramebuffer() const
{
    return openGLBuffers.sceneFBO;
}

OpenGLPickingBuffer &ComponentWindow::getPickingColorFramebuffer()
{
    return pickingColorBuffer;
}

GLuint ComponentWindow::getBackgroundFramebuffer() const {
    return openGLBuffers.backgroundFBO;
}

GLuint ComponentWindow::getUIFramebuffer() const {
    return openGLBuffers.uiFBO;
}

GLuint ComponentWindow::getForegroundFramebuffer() const {
    return openGLBuffers.foregroundFBO;
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
    UpdateWindowSize();

    glGenFramebuffers(1, &openGLBuffers.globalFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, openGLBuffers.globalFBO);

    glGenTextures(1, &openGLBuffers.globalTexture);
    glBindTexture(GL_TEXTURE_2D, openGLBuffers.globalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthRender, heightRender, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLBuffers.globalTexture, 0);

    GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // --
    glGenFramebuffers(1, &openGLBuffers.postProcessingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, openGLBuffers.postProcessingFBO);

    glGenTextures(1, &openGLBuffers.postProcessingTexture);
    glBindTexture(GL_TEXTURE_2D, openGLBuffers.postProcessingTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthRender, heightRender, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLBuffers.postProcessingTexture, 0);

    // ----

    glGenFramebuffers(1, &openGLBuffers.sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, openGLBuffers.sceneFBO);

    glGenTextures(1, &openGLBuffers.sceneTexture);
    glBindTexture(GL_TEXTURE_2D, openGLBuffers.sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthRender, heightRender, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLBuffers.sceneTexture, 0);

    glGenTextures(1, &openGLBuffers.sceneDepthTexture);
    glBindTexture(GL_TEXTURE_2D, openGLBuffers.sceneDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, widthRender, heightRender, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, openGLBuffers.sceneDepthTexture, 0);

    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // ----

    glGenFramebuffers(1, &openGLBuffers.backgroundFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, openGLBuffers.backgroundFBO);

    glGenTextures(1, &openGLBuffers.backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, openGLBuffers.backgroundTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthRender, heightRender, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLBuffers.backgroundTexture, 0);

    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // ----

    glGenFramebuffers(1, &openGLBuffers.foregroundFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, openGLBuffers.foregroundFBO);

    glGenTextures(1, &openGLBuffers.foregroundTexture);
    glBindTexture(GL_TEXTURE_2D, openGLBuffers.foregroundTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthRender, heightRender, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLBuffers.foregroundTexture, 0);

    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ----
    glGenFramebuffers(1, &openGLBuffers.uiFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, openGLBuffers.uiFBO);

    glGenTextures(1, &openGLBuffers.uiTexture);
    glBindTexture(GL_TEXTURE_2D, openGLBuffers.uiTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthRender, heightRender, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLBuffers.uiTexture, 0);
    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentWindow::resetFramebuffer()
{
    glDeleteFramebuffers(1, &openGLBuffers.globalFBO);
    glDeleteTextures(1, &openGLBuffers.globalTexture);

    glDeleteFramebuffers(1, &openGLBuffers.sceneFBO);
    glDeleteTextures(1, &openGLBuffers.sceneTexture);
    glDeleteTextures(1, &openGLBuffers.sceneDepthTexture);

    glDeleteFramebuffers(1, &openGLBuffers.backgroundFBO);
    glDeleteTextures(1, &openGLBuffers.backgroundTexture);

    glDeleteFramebuffers(1, &openGLBuffers.foregroundFBO);
    glDeleteTextures(1, &openGLBuffers.foregroundTexture);

    glDeleteFramebuffers(1, &openGLBuffers.uiFBO);
    glDeleteTextures(1, &openGLBuffers.uiTexture);

    createFramebuffer();

    ComponentsManager::get()->getComponentRender()->resizeShadersFramebuffers();

    resizeGBuffer();
}

void ComponentWindow::RenderLayersToMain()
{
   if (BrakezaSetup::get()->ENABLE_IMGUI) {
       ImGuiOnUpdate();
   }

    auto shaderOGLImage = ComponentsManager::get()->getComponentRender()->getShaderOGLImage();
    shaderOGLImage->renderTexture(openGLBuffers.foregroundTexture, 0, 0, widthWindow, heightWindow, 1, true, openGLBuffers.globalFBO);
    shaderOGLImage->renderTexture(openGLBuffers.globalTexture, 0, 0, widthWindow, heightWindow, 1, true, 0);
    shaderOGLImage->renderTexture(openGLBuffers.uiTexture, 0, 0, widthWindow, heightWindow, 1, true, 0);

    SDL_GL_SwapWindow(window);
}

void ComponentWindow::ClearOGLFrameBuffers() const
{
    const GLuint framebuffers[] = {
        openGLBuffers.postProcessingFBO,
        openGLBuffers.foregroundFBO,
        openGLBuffers.uiFBO,
        openGLBuffers.backgroundFBO,
        openGLBuffers.globalFBO,
        openGLBuffers.sceneFBO,
        gBuffer.FBO,
        pickingColorBuffer.FBO
    };

    for (const GLuint fbo : framebuffers) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    if (BrakezaSetup::get()->ENABLE_SHADOW_MAPPING) {
        ComponentsManager::get()->getComponentRender()->getShaderOGLShadowPass()->clearDirectionalLightDepthTexture();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

int ComponentWindow::getWidth() const
{
    return widthWindow;
}

int ComponentWindow::getHeight() const
{
    return heightWindow;
}

int ComponentWindow::getWidthRender() const
{
    return widthRender;
}

int ComponentWindow::getHeightRender() const
{
    return heightRender;
}

GLuint ComponentWindow::getGlobalFramebuffer() const {
    return openGLBuffers.globalFBO;
}

GLuint ComponentWindow::getPostProcessingFramebuffer() const {
    return openGLBuffers.postProcessingFBO;
}

void ComponentWindow::saveImGuiCurrentLayout() const
{
    switch(ImGuiConfigChanged) {
        case BrakezaSetup::ImGUIConfigs::DEFAULT: {
            ImGui::SaveIniSettingsToDisk(std::string(BrakezaSetup::get()->CONFIG_FOLDER + "ImGuiDefault.ini").c_str());
            Logging::Message("Saving to ImGUIDefault.ini");
            break;
        }
        case BrakezaSetup::ImGUIConfigs::CODING: {
            ImGui::SaveIniSettingsToDisk(std::string(BrakezaSetup::get()->CONFIG_FOLDER + "ImGuiCoding.ini").c_str());
            Logging::Message("Saving to ImGuiCoding.ini");
            break;
        }
        case BrakezaSetup::ImGUIConfigs::DESIGN: {
            ImGui::SaveIniSettingsToDisk(std::string(BrakezaSetup::get()->CONFIG_FOLDER + "ImGuiDesign.ini").c_str());
            Logging::Message("Saving to ImGuiDesign.ini");
            break;
        }
    }
}

void ComponentWindow::ImGuiInitialize(const std::string& configFile)
{
    Logging::Message("[ComponentWindow] Initializing ImGui...");

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
    colors[ImGuiCol_WindowBg]               = ImVec4(0.11f, 0.11f, 0.11f, 0.90f);
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
    ImGuiConfig = BrakezaSetup::ImGUIConfigs::DEFAULT;
}

void ComponentWindow::ImGuiOnUpdate()
{
    if (ImGuiConfig != ImGuiConfigChanged) {
        switch(ImGuiConfigChanged) {
            case BrakezaSetup::ImGUIConfigs::DEFAULT: {
                ImGui::ClearIniSettings();
                Logging::Message("Loading layout ImGUIDefault.ini");
                ImGui::LoadIniSettingsFromDisk(std::string(BrakezaSetup::get()->CONFIG_FOLDER + "ImGuiDefault.ini").c_str());
                break;
            }
            case BrakezaSetup::ImGUIConfigs::CODING: {
                ImGui::ClearIniSettings();
                Logging::Message("Loading layout ImGuiCoding.ini");
                ImGui::LoadIniSettingsFromDisk(std::string(BrakezaSetup::get()->CONFIG_FOLDER + "ImGuiCoding.ini").c_str());
                break;
            }
            case BrakezaSetup::ImGUIConfigs::DESIGN: {
                ImGui::ClearIniSettings();
                Logging::Message("Loading layout ImGuiDesign.ini");
                ImGui::LoadIniSettingsFromDisk(std::string(BrakezaSetup::get()->CONFIG_FOLDER + "ImGuiDesign.ini").c_str());
                break;
            }
        }
        ImGuiConfig = ImGuiConfigChanged;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, getUIFramebuffer());

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();

    ImGui::NewFrame();
    Brakeza::get()->GUI()->DrawGUI();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool ComponentWindow::isWindowMaximized() const
{
    Uint32 flags = SDL_GetWindowFlags(window);

    return (flags & SDL_WINDOW_MAXIMIZED) != 0;
}

void ComponentWindow::setWindowTitle(const char *title) const
{
    SDL_SetWindowTitle(window, title);
}

void ComponentWindow::toggleFullScreen() const
{
    if (BrakezaSetup::get()->FULLSCREEN) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window, 0);
    }
}

ImGuizmo::OPERATION ComponentWindow::getGuiZmoOperation() const
{
    return ImGuiOperationGuizmo;
}

void ComponentWindow::setGuiZmoOperation(ImGuizmo::OPERATION operation)
{
    ImGuiOperationGuizmo = operation;
}

GLuint ComponentWindow::getDepthTexture() const
{
    return openGLBuffers.sceneDepthTexture;
}

GLuint ComponentWindow::getSceneTexture() const
{
    return openGLBuffers.sceneTexture;
}

GLuint ComponentWindow::getGlobalTexture() const
{
    return openGLBuffers.globalTexture;
}

OpenGLGBuffer& ComponentWindow::getGBuffer()
{
    return gBuffer;
}

OpenGLGlobalFramebuffers& ComponentWindow::getGlobalBuffers()
{
    return openGLBuffers;
}

void ComponentWindow::createPickingColorBuffer()
{
    if (pickingColorBuffer.FBO != 0) {
        glDeleteFramebuffers(1, &pickingColorBuffer.FBO);
        glDeleteTextures(1, &pickingColorBuffer.rbgTexture);
        glDeleteRenderbuffers(1, &pickingColorBuffer.depthTexture);
    }

    glGenFramebuffers(1, &pickingColorBuffer.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, pickingColorBuffer.FBO);

    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glGenTextures(1, &pickingColorBuffer.rbgTexture);
    glBindTexture(GL_TEXTURE_2D, pickingColorBuffer.rbgTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, widthRender, heightRender, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingColorBuffer.rbgTexture, 0);

    glGenRenderbuffers(1, &pickingColorBuffer.depthTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, pickingColorBuffer.depthTexture);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, widthRender, heightRender);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pickingColorBuffer.depthTexture);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[ERROR] PickingColor: Framebuffer no está completo!" << std::endl;
        exit(-1);
    }

    Logging::Message("[ComponentWindow] PickingColor-Buffer created successful (%d, %d)", widthRender,  heightRender);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentWindow::createGBuffer()
{
    if (gBuffer.FBO != 0) {
        glDeleteFramebuffers(1, &gBuffer.FBO);
        glDeleteTextures(1, &gBuffer.albedo);
        glDeleteTextures(1, &gBuffer.normals);
        glDeleteTextures(1, &gBuffer.positions);
        glDeleteRenderbuffers(1, &gBuffer.depth);
    }

    glGenFramebuffers(1, &gBuffer.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FBO);

    glGenTextures(1, &gBuffer.positions);
    glBindTexture(GL_TEXTURE_2D, gBuffer.positions);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, widthRender, heightRender, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBuffer.positions, 0);

    glGenTextures(1, &gBuffer.normals);
    glBindTexture(GL_TEXTURE_2D, gBuffer.normals);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, widthRender, heightRender, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gBuffer.normals, 0);

    glGenTextures(1, &gBuffer.albedo);
    glBindTexture(GL_TEXTURE_2D, gBuffer.albedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthRender, heightRender, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gBuffer.albedo, 0);

    GLuint attachments[3] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2
    };
    glDrawBuffers(3, attachments);

    glGenTextures(1, &gBuffer.depth);
    glBindTexture(GL_TEXTURE_2D, gBuffer.depth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, widthRender, heightRender, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gBuffer.depth, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "[ERROR] G-Buffer: Framebuffer no está completo!" << std::endl;
        exit(-1);
    }

    Logging::Message("[ComponentWindow] G-Buffer created successful (%d, %d)", widthRender, heightRender);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentWindow::resizeGBuffer()
{
    createGBuffer();
    createPickingColorBuffer();
}

void ComponentWindow::UpdateWindowSize()
{
    SDL_GetWindowSize(window, &widthWindow, &heightWindow);
    SDL_GetRendererOutputSize(renderer, &widthRender, &heightRender);
}

int ComponentWindow::getObjectIDByPickingColorFramebuffer(const int x, const int y) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, pickingColorBuffer.FBO);

    unsigned char pixel[4];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    const int flippedY = getHeight() - y - 1;
    glReadPixels(x, flippedY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

    const auto c = Color(
        static_cast<float>(pixel[0])/255.0f,
        static_cast<float>(pixel[1])/255.0f,
        static_cast<float>(pixel[2])/255.0f
    );

    return Color::colorToId(c);
}

BrakezaSetup::ImGUIConfigs ComponentWindow::getImGuiConfig() const
{
    return ImGuiConfig;
}

void ComponentWindow::setImGuiConfig(BrakezaSetup::ImGUIConfigs c)
{
    ImGuiConfigChanged = c;
}

void ComponentWindow::CheckForResizeOpenGLWindow(const SDL_Event &e)
{
    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        UpdateWindowSize();
        glViewport(0,0, getWidth(), getHeight());
        resetFramebuffer();
    }
}
