#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL_image.h>
#include "imgui_internal.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../../include/Components/ComponentWindow.h"
#include "../../include/Misc/Logging.h"
#include "../../include/OpenGL/ShaderOGLImage.h"
#include "../../include/Brakeza.h"
#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/IconsGUI.h"
#include "../../include/GUI/GUI.h"

ComponentWindow::ComponentWindow()
:
    applicationIcon(IMG_Load(std::string(Config::get()->ICONS_FOLDER + Config::get()->iconApplication).c_str()))
{
    InitWindow();
}

void ComponentWindow::onStart()
{
    Component::onStart();
    InitFontsTTF();
    postProcessingManager = new PostProcessingManager();
    postProcessingManager->initialize(widthRender, heightRender);

    ImGuiInitialize(Config::get()->CONFIG_FOLDER + "ImGuiDefault.ini");

    CreateFramebuffer();
    CreateGBuffer();
    CreatePickingColorBuffer();
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
    Components::get()->Render()->UpdateSelectedObject3D();
}

void ComponentWindow::InitWindow()
{
    Logging::Message("[Window] Init window...");

    std::string drivers;
    for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i) {
        SDL_RendererInfo rendererInfo = {};
        SDL_GetRenderDriverInfo(i, &rendererInfo);
        if (i > 0) drivers += ", ";
        drivers += rendererInfo.name;
    }
    Logging::Message("[Window] Drivers for rendering: %s", drivers.c_str());

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        Logging::Error("[Window] SDL could not initialize! SDL_Error: %s", SDL_GetError());
        exit(-1);
    }

    window = SDL_CreateWindow(
        SETUP->ENGINE_TITLE.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SETUP->screenWidth,
        SETUP->screenHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
    );

    context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, context);

    Logging::Message("[Window] Current video driver: %s", SDL_GetCurrentVideoDriver());
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
        Logging::Error("Window could not be created! SDL_Error: %s", SDL_GetError());
        exit(-1);
    }

    screenSurface = SDL_CreateRGBSurface(0, SETUP->screenWidth, SETUP->screenHeight, 32, 0, 0, 0, 0);

    SDL_SetSurfaceBlendMode(screenSurface, SDL_BLENDMODE_MOD);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED );
    SDL_GetRendererOutputSize(renderer, &widthRender, &heightRender);

    InitOpenGL();
    glewInit();
    SDL_GL_SetSwapInterval(1);
    SDL_SetWindowIcon(window, applicationIcon);
}

void ComponentWindow::InitFontsTTF()
{
    Logging::Message("[Window] Init TrueTypeFonts...");

    if (TTF_Init() < 0) {
        Logging::Message(TTF_GetError());
        exit(-1);
    }

    std::string pathFont = SETUP->FONTS_FOLDER + "TroubleFont.ttf";
    Logging::Message("[Window] Loading default TTF: %s", pathFont.c_str());

    fontDefault = TTF_OpenFont(pathFont.c_str(), 35);

    if (!fontDefault) {
        Logging::Message(TTF_GetError());
        exit(-1);
    }
}

void ComponentWindow::InitOpenGL()
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

void ComponentWindow::CreateFramebuffer()
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

void ComponentWindow::ResetFramebuffer()
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

    CreateFramebuffer();

    Components::get()->Render()->resizeShadersFramebuffers();

    ResizeGBuffer();
}

void ComponentWindow::FlipGlobalToWindow()
{
   if (Config::get()->ENABLE_IMGUI) {
       ImGuiOnUpdate();
   }

    auto shaderOGLImage = Components::get()->Render()->getShaders()->shaderOGLImage;
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

    if (Config::get()->ENABLE_SHADOW_MAPPING) {
        Components::get()->Render()->getShaders()->shaderShadowPass->clearDirectionalLightDepthTexture();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ComponentWindow::SaveImGuiCurrentLayout() const
{
    switch(ImGuiConfigChanged) {
        case Config::ImGUIConfigs::DEFAULT: {
            ImGui::SaveIniSettingsToDisk(std::string(Config::get()->CONFIG_FOLDER + "ImGuiDefault.ini").c_str());
            Logging::Message("Saving to ImGUIDefault.ini");
            break;
        }
        case Config::ImGUIConfigs::CODING: {
            ImGui::SaveIniSettingsToDisk(std::string(Config::get()->CONFIG_FOLDER + "ImGuiCoding.ini").c_str());
            Logging::Message("Saving to ImGuiCoding.ini");
            break;
        }
        case Config::ImGUIConfigs::DESIGN: {
            ImGui::SaveIniSettingsToDisk(std::string(Config::get()->CONFIG_FOLDER + "ImGuiDesign.ini").c_str());
            Logging::Message("Saving to ImGuiDesign.ini");
            break;
        }
    }
}

void ComponentWindow::ImGuiInitialize(const std::string& configFile)
{
    Logging::Message("[Window] Initializing ImGui...");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    auto glsl_version = "#version 130";
    ImGui_ImplOpenGL3_Init(glsl_version);

    ImGuiIO &io = ImGui::GetIO();
    io.WantCaptureMouse = false;
    io.WantCaptureKeyboard = false;
    io.WantSaveIniSettings = true;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable docking

    ImGui::StyleColorsDark();

    GUI::ImGuiSetColors();

    ImGui::ClearIniSettings();
    ImGui::LoadIniSettingsFromDisk(std::string(configFile).c_str());
    ImGui::GetStyle().Alpha = 1.0f;
    ImGuiConfig = Config::ImGUIConfigs::DEFAULT;
}

void ComponentWindow::ImGuiOnUpdate()
{
    if (ImGuiConfig != ImGuiConfigChanged) {
        switch(ImGuiConfigChanged) {
            case Config::ImGUIConfigs::DEFAULT: {
                ImGui::ClearIniSettings();
                Logging::Message("[Window] Loading layout ImGUIDefault.ini");
                ImGui::LoadIniSettingsFromDisk(std::string(Config::get()->CONFIG_FOLDER + "ImGuiDefault.ini").c_str());
                break;
            }
            case Config::ImGUIConfigs::CODING: {
                ImGui::ClearIniSettings();
                Logging::Message("[Window] Loading layout ImGuiCoding.ini");
                ImGui::LoadIniSettingsFromDisk(std::string(Config::get()->CONFIG_FOLDER + "ImGuiCoding.ini").c_str());
                break;
            }
            case Config::ImGUIConfigs::DESIGN: {
                ImGui::ClearIniSettings();
                Logging::Message("[Window] Loading layout ImGuiDesign.ini");
                ImGui::LoadIniSettingsFromDisk(std::string(Config::get()->CONFIG_FOLDER + "ImGuiDesign.ini").c_str());
                break;
            }
        }
        ImGuiConfig = ImGuiConfigChanged;
        Brakeza::get()->GUI()->setLayoutToDefault(getImGuiConfig());
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
    if (Config::get()->FULLSCREEN) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window, 0);
    }
}


void ComponentWindow::setGuiZmoOperation(ImGuizmo::OPERATION operation)
{
    ImGuiOperationGuizmo = operation;
}

void ComponentWindow::CreatePickingColorBuffer()
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
        Logging::Error("[Window] PickingColor: Framebuffer no está completo!");
        exit(-1);
    }

    Logging::Message("[Window] PickingColor-Buffer created successful (%d, %d)", widthRender,  heightRender);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentWindow::CreateGBuffer()
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
        Logging::Error("[Window] G-Buffer: Framebuffer no está completo!");
        exit(-1);
    }

    Logging::Message("[ComponentWindow] G-Buffer created successful (%d, %d)", widthRender, heightRender);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ComponentWindow::ResizeGBuffer()
{
    CreateGBuffer();
    CreatePickingColorBuffer();
}

void ComponentWindow::UpdateWindowSize()
{
    SDL_GetWindowSize(window, &widthWindow, &heightWindow);
    SDL_GL_GetDrawableSize(window, &widthRender, &heightRender);
}

unsigned int ComponentWindow::getObjectIDByPickingColorFramebuffer(const int x, const int y) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, pickingColorBuffer.FBO);

    unsigned char pixel[4];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    const int flippedY = getHeight() - y - 1;
    glReadPixels(x, flippedY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

    const auto c = Color(
        (float) pixel[0] / 255.0f,
        (float) pixel[1] / 255.0f,
        (float) pixel[2] / 255.0f
    );

    return Color::colorToId(c);
}

void ComponentWindow::setImGuiConfig(Config::ImGUIConfigs c)
{
    ImGuiConfigChanged = c;
}

void ComponentWindow::CheckForResizeOpenGLWindow(const SDL_Event &e)
{
    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        UpdateWindowSize();
        glViewport(0,0, getWidth(), getHeight());
        ResetFramebuffer();
    }
}
