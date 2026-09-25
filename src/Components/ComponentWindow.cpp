#define GL_GLEXT_PROTOTYPES

#include <complex>
#include <set>
#include <vector>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <SDL_image.h>
#include "imgui_internal.h"
#include "../imgui/backends/imgui_impl_opengl3.h"
#include "../imgui/backends/imgui_impl_sdl2.h"
#include "../../include/Components/ComponentWindow.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Misc/Tools.h"
#include "../../include/OpenGL/ShaderOGLImage.h"
#include "../../include/Brakeza.h"
#include "../../include/2D/Image2D.h"
#include "../../include/Components/Components.h"
#include "../../include/GUI/Objects/IconsGUI.h"
#include "../../include/GUI/GUI.h"
#include "../../include/Render/Profiler.h"

ComponentWindow::ComponentWindow()
:
    applicationIcon(Tools::SafeIMGLoad(Config::get()->ICONS_FOLDER + Config::get()->iconApplication))
{
    InitWindow();
}

void ComponentWindow::onStart()
{
    setEnabled(true);
    InitFontsTTF();
    postProcessingManager = new PostProcessingManager();
    postProcessingManager->Initialize(widthRender, heightRender);

    ImGuiInitialize(Config::get()->CONFIG_FOLDER + "ImGuiDefault.ini");

    CreateGBuffer();
    CreateFramebuffers();
    CreatePickingColorBuffer();
}

void ComponentWindow::preUpdate()
{
    Component::preUpdate();

    //UpdateWindowSize();
    //glViewport(0,0, widthWindow, heightWindow);
}

void ComponentWindow::onUpdate()
{
    Component::onUpdate();

    auto sceneObjects = Brakeza::get()->copySceneObjects();
    for (const auto &o : sceneObjects) {
        if (!o->isEnabled()) continue;
        if (o->getTypeObject() != ObjectType::Image2D) continue;
        auto *img = static_cast<Image2D*>(o);
        if (img->isDrawInBackground()) img->drawBackground();
    }
}

void ComponentWindow::postUpdate()
{
    if (hoverPBO == 0)
        glGenBuffers(1, &hoverPBO);

    // Read result from last frame's async readback (no stall — GPU already done)
    if (hoverPBOPending) {
        glBindBuffer(GL_PIXEL_PACK_BUFFER, hoverPBO);
        const auto* rgb = static_cast<const unsigned char*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));
        if (rgb) {
            hoverPickResultID = ((unsigned int)rgb[0] << 16) | ((unsigned int)rgb[1] << 8) | (unsigned int)rgb[2];
            glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        }
        glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
        hoverPBOPending = false;
    }

    if (hoverPickRequestX < 0) return;

    // Issue async readback into PBO for next frame to consume
    glBindFramebuffer(GL_FRAMEBUFFER, pickingColorBuffer.FBO);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(pickingColorBuffer.FBO);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, hoverPBO);
    glBufferData(GL_PIXEL_PACK_BUFFER, 3, nullptr, GL_STREAM_READ);
    glReadPixels(hoverPickRequestX, getHeightRender() - hoverPickRequestY - 1, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);
    hoverPickRequestX = -1;
    hoverPBOPending   = true;
}

void ComponentWindow::onEnd()
{
    if (hoverPBO) { glDeleteBuffers(1, &hoverPBO); hoverPBO = 0; }
    TTF_CloseFont(fontDefault);
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}

void ComponentWindow::onSDLPollEvent(SDL_Event *event, bool &finish)
{
}

void ComponentWindow::InitWindow()
{
    LOG_MESSAGE("[Window] Init window...");

    std::string drivers;
    for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i) {
        SDL_RendererInfo rendererInfo = {};
        SDL_GetRenderDriverInfo(i, &rendererInfo);
        if (i > 0) drivers += ", ";
        drivers += rendererInfo.name;
    }
    LOG_MESSAGE("[Window] Drivers for rendering: %s", drivers.c_str());

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        LOG_ERROR("[Window] SDL could not initialize! SDL_Error: %s", SDL_GetError());
        exit(-1);
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,   24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
    SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );

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

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_GetRendererOutputSize(renderer, &widthRender, &heightRender);

    LOG_MESSAGE("[Window] Current video driver: %s", SDL_GetCurrentVideoDriver());

    if (window == nullptr) {
        LOG_ERROR("Window could not be created! SDL_Error: %s", SDL_GetError());
        exit(-1);
    }


    ResetOpenGLSettings();
    glewInit();
    // Antes forzaba vsync ON a fuego (SDL_GL_SetSwapInterval(1)) ignorando Config::V_SYNC -- si
    // un proyecto se guardaba con V_SYNC=false, al reabrirlo el estado REAL de OpenGL seguía en
    // vsync ON pese a que el checkbox del menú (GUIAddonMenu::MenuVideo) mostrara "desactivado",
    // hasta que el usuario lo tocara a mano esa sesión. Ahora respeta el valor cargado desde el
    // arranque, igual que hace el propio toggle del menú.
    SDL_GL_SetSwapInterval(Config::get()->V_SYNC ? 1 : 0);
    SDL_RenderSetVSync(renderer, Config::get()->V_SYNC ? 1 : 0);
    SDL_SetWindowIcon(window, applicationIcon);
}

void ComponentWindow::InitFontsTTF()
{
    LOG_MESSAGE("[Window] Init TrueTypeFonts...");

    if (TTF_Init() < 0) {
        LOG_MESSAGE(TTF_GetError());
        exit(-1);
    }

    std::string pathFont = SETUP->FONTS_FOLDER + "TroubleFont.ttf";
    LOG_MESSAGE("[Window] Loading default TTF: %s", pathFont.c_str());

    fontDefault = TTF_OpenFont(pathFont.c_str(), 35);

    if (!fontDefault) {
        LOG_MESSAGE(TTF_GetError());
        exit(-1);
    }
}

void ComponentWindow::ResetOpenGLSettings()
{
    //glActiveTexture(GL_TEXTURE0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void ComponentWindow::CreateFramebuffers()
{
    glGenFramebuffers(1, &openGLBuffers.globalFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, openGLBuffers.globalFBO);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(openGLBuffers.globalFBO);

    glGenTextures(1, &openGLBuffers.globalTexture);
    glBindTexture(GL_TEXTURE_2D, openGLBuffers.globalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthRender, heightRender, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLBuffers.globalTexture, 0);
    LOG_MESSAGE("[Render] Creating globalTexture(%d, %d)", widthRender, heightRender);

    GLenum framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("[Window] globalFBO incomplete (status 0x%X) at %dx%d", framebufferStatus, widthRender, heightRender);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);
    // ----

    glGenFramebuffers(1, &openGLBuffers.sceneFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, openGLBuffers.sceneFBO);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(openGLBuffers.sceneFBO);

    glGenTextures(1, &openGLBuffers.sceneTexture);
    glBindTexture(GL_TEXTURE_2D, openGLBuffers.sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthRender, heightRender, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLBuffers.sceneTexture, 0);
    LOG_MESSAGE("[Render] Creating sceneTexture(%d, %d)", widthRender, heightRender);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gBuffer.depth, 0);
    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("[Window] sceneFBO incomplete (status 0x%X) at %dx%d, gBuffer.depth=%u",
            framebufferStatus, widthRender, heightRender, gBuffer.depth);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);
    // ----

    glGenFramebuffers(1, &openGLBuffers.backgroundFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, openGLBuffers.backgroundFBO);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(openGLBuffers.backgroundFBO);

    glGenTextures(1, &openGLBuffers.backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, openGLBuffers.backgroundTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthRender, heightRender, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLBuffers.backgroundTexture, 0);
    LOG_MESSAGE("[Render] Creating backgroundTexture(%d, %d)", widthRender, heightRender);

    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("[Window] backgroundFBO incomplete (status 0x%X) at %dx%d", framebufferStatus, widthRender, heightRender);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);
    // ----

    glGenFramebuffers(1, &openGLBuffers.foregroundFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, openGLBuffers.foregroundFBO);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(openGLBuffers.foregroundFBO);

    glGenTextures(1, &openGLBuffers.foregroundTexture);
    glBindTexture(GL_TEXTURE_2D, openGLBuffers.foregroundTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthRender, heightRender, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLBuffers.foregroundTexture, 0);
    LOG_MESSAGE("[Render] Creating foregroundTexture(%d, %d)", widthRender, heightRender);

    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("[Window] foregroundFBO incomplete (status 0x%X) at %dx%d", framebufferStatus, widthRender, heightRender);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);

    // ----
    glGenFramebuffers(1, &openGLBuffers.uiFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, openGLBuffers.uiFBO);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(openGLBuffers.uiFBO);

    glGenTextures(1, &openGLBuffers.uiTexture);
    glBindTexture(GL_TEXTURE_2D, openGLBuffers.uiTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthWindow, heightWindow, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, openGLBuffers.uiTexture, 0);
    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    LOG_MESSAGE("[Render] Creating UITexture(%d, %d)", widthWindow, heightWindow);

    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("[Window] uiFBO incomplete (status 0x%X) at %dx%d", framebufferStatus, widthWindow, heightWindow);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);
}

void ComponentWindow::ResetFramebuffer()
{
    LOG_SUCCESS("[Render] Reset Main Framebuffers...");

    glDeleteFramebuffers(1, &openGLBuffers.globalFBO);
    glDeleteTextures(1, &openGLBuffers.globalTexture);

    glDeleteFramebuffers(1, &openGLBuffers.sceneFBO);
    glDeleteTextures(1, &openGLBuffers.sceneTexture);

    glDeleteFramebuffers(1, &openGLBuffers.backgroundFBO);
    glDeleteTextures(1, &openGLBuffers.backgroundTexture);

    glDeleteFramebuffers(1, &openGLBuffers.foregroundFBO);
    glDeleteTextures(1, &openGLBuffers.foregroundTexture);

    glDeleteFramebuffers(1, &openGLBuffers.uiFBO);
    glDeleteTextures(1, &openGLBuffers.uiTexture);

    ResizeGBuffer();
    CreateFramebuffers();

    Components::get()->Render()->resizeShadersFramebuffers();
    postProcessingManager->resize(widthRender, heightRender);
}

void ComponentWindow::FlipGlobalToWindow()
{
    glViewport(0,0, widthWindow, heightWindow);

    if (Config::get()->ENABLE_IMGUI) {
       ImGuiOnUpdate();
    } else {
        glViewport(0, 0, widthRender, heightRender);
        Components::get()->Render()->DrawSelectionBox();
    }

    // Restore window-space viewport explicitly — ImGui_ImplOpenGL3_RenderDrawData
    // overrides the viewport to (drawable_w, drawable_h) which may differ on HiDPI.
    glViewport(0, 0, widthWindow, heightWindow);

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    auto shaderOGLImage = Components::get()->Render()->getShaders()->shaderOGLImage;

    shaderOGLImage->renderTexture(openGLBuffers.backgroundTexture, 0, 0, widthWindow, heightWindow, widthWindow, heightWindow, 1, true, 0);
    shaderOGLImage->renderTexture(openGLBuffers.globalTexture, 0, 0, widthWindow, heightWindow, widthWindow, heightWindow, 1, true, 0);
    shaderOGLImage->renderTexture(openGLBuffers.foregroundTexture, 0, 0, widthWindow, heightWindow, widthWindow, heightWindow, 1, true, 0);
    shaderOGLImage->renderTexture(openGLBuffers.uiTexture, 0, 0, widthWindow, heightWindow, widthWindow, heightWindow, 1, true, 0);

    glEnable(GL_DEPTH_TEST);

    SDL_GL_SwapWindow(window);
}

void ComponentWindow::ClearOGLFrameBuffers() const
{
    glDisable(GL_SCISSOR_TEST);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    auto render = Components::get()->Render();

    // G-Buffer siempre a negro: sus canales codifican posición/normal/albedo,
    // no un color de fondo. El check "dot(fragPos,fragPos)<0.001" requiere (0,0,0).
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    render->ChangeOpenGLFramebuffer(gBuffer.FBO);            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // sceneFBO: color configurable — es el fondo visible donde no hay geometría
    glClearColor(clearColorR, clearColorG, clearColorB, 1.0f);
    render->ChangeOpenGLFramebuffer(openGLBuffers.sceneFBO); glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Picking y capas de composición siempre transparentes
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    render->ChangeOpenGLFramebuffer(pickingColorBuffer.FBO);          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render->ChangeOpenGLFramebuffer(openGLBuffers.globalFBO);         glClear(GL_COLOR_BUFFER_BIT);
    render->ChangeOpenGLFramebuffer(openGLBuffers.backgroundFBO);     glClear(GL_COLOR_BUFFER_BIT);
    render->ChangeOpenGLFramebuffer(openGLBuffers.foregroundFBO);     glClear(GL_COLOR_BUFFER_BIT);
    render->ChangeOpenGLFramebuffer(openGLBuffers.uiFBO);             glClear(GL_COLOR_BUFFER_BIT);

    if (Config::get()->ENABLE_SHADOW_MAPPING) {
        Components::get()->Render()->getShaders()->shaderShadowPass->clearDirectionalLightDepthTexture();
    }

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ComponentWindow::SaveImGuiCurrentLayout() const
{
    switch(ImGuiConfigChanged) {
        case Config::ImGUIConfigs::DEFAULT: {
            ImGui::SaveIniSettingsToDisk(std::string(Config::get()->CONFIG_FOLDER + "ImGuiDefault.ini").c_str());
            LOG_MESSAGE("Saving to ImGUIDefault.ini");
            break;
        }
        case Config::ImGUIConfigs::CODING: {
            ImGui::SaveIniSettingsToDisk(std::string(Config::get()->CONFIG_FOLDER + "ImGuiCoding.ini").c_str());
            LOG_MESSAGE("Saving to ImGuiCoding.ini");
            break;
        }
        case Config::ImGUIConfigs::DESIGN: {
            ImGui::SaveIniSettingsToDisk(std::string(Config::get()->CONFIG_FOLDER + "ImGuiDesign.ini").c_str());
            LOG_MESSAGE("Saving to ImGuiDesign.ini");
            break;
        }
    }
}

void ComponentWindow::ImGuiInitialize(const std::string& configFile)
{
    LOG_MESSAGE("[Window] Initializing ImGui...");

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
    Components::get()->Render()->ChangeOpenGLFramebuffer(getUIFramebuffer());

    if (ImGuiConfig != ImGuiConfigChanged) {
        switch(ImGuiConfigChanged) {
            case Config::ImGUIConfigs::DEFAULT: {
                ImGui::ClearIniSettings();
                LOG_MESSAGE("[Window] Loading layout ImGUIDefault.ini");
                ImGui::LoadIniSettingsFromDisk(std::string(Config::get()->CONFIG_FOLDER + "ImGuiDefault.ini").c_str());
                break;
            }
            case Config::ImGUIConfigs::CODING: {
                ImGui::ClearIniSettings();
                LOG_MESSAGE("[Window] Loading layout ImGuiCoding.ini");
                ImGui::LoadIniSettingsFromDisk(std::string(Config::get()->CONFIG_FOLDER + "ImGuiCoding.ini").c_str());
                break;
            }
            case Config::ImGUIConfigs::DESIGN: {
                ImGui::ClearIniSettings();
                LOG_MESSAGE("[Window] Loading layout ImGuiDesign.ini");
                ImGui::LoadIniSettingsFromDisk(std::string(Config::get()->CONFIG_FOLDER + "ImGuiDesign.ini").c_str());
                break;
            }
        }
        ImGuiConfig = ImGuiConfigChanged;
        Brakeza::get()->GUI()->setLayoutToDefault(getImGuiConfig());
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    Components::get()->Render()->ChangeOpenGLFramebuffer(0);
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

void ComponentWindow::ToggleFullScreen() const
{
    if (Config::get()->FULLSCREEN) {
        // SDL_WINDOW_FULLSCREEN = pantalla completa EXCLUSIVA (fuera de DWM en Windows).
        // SDL_WINDOW_FULLSCREEN_DESKTOP = "borderless", sigue compuesta por DWM -- ver comentario
        // de Config::EXCLUSIVE_FULLSCREEN.
        SDL_SetWindowFullscreen(
            window,
            Config::get()->EXCLUSIVE_FULLSCREEN ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_FULLSCREEN_DESKTOP
        );
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
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(pickingColorBuffer.FBO);

    GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, drawBuffers);

    glGenTextures(1, &pickingColorBuffer.rbgTexture);
    glBindTexture(GL_TEXTURE_2D, pickingColorBuffer.rbgTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, widthRender, heightRender, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pickingColorBuffer.rbgTexture, 0);

    glGenRenderbuffers(1, &pickingColorBuffer.depthTexture);
    glBindRenderbuffer(GL_RENDERBUFFER, pickingColorBuffer.depthTexture);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, widthRender, heightRender);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pickingColorBuffer.depthTexture);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("[Window] PickingColor: Framebuffer no está completo!");
        exit(-1);
    }

    // Must clear after the attachments exist -- clearing an FBO with no color/depth attachment
    // yet bound is a GL_INVALID_FRAMEBUFFER_OPERATION (harmless here since nothing reads the
    // buffer before the next real render, but it left a spurious error sitting in the GL error
    // queue on every resize).
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    LOG_MESSAGE("[Window] PickingColor-Buffer created successful (%d, %d)", widthRender,  heightRender);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);
}

void ComponentWindow::CreateGBuffer()
{
    if (gBuffer.FBO != 0) {
        glDeleteFramebuffers(1, &gBuffer.FBO);
        glDeleteTextures(1, &gBuffer.albedo);
        glDeleteTextures(1, &gBuffer.emission);
        glDeleteTextures(1, &gBuffer.normals);
        glDeleteTextures(1, &gBuffer.positions);
        glDeleteTextures(1, &gBuffer.depth);
    }

    glGenFramebuffers(1, &gBuffer.FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.FBO);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(gBuffer.FBO);

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

    // Emisión: adjunta siempre, pero FUERA de glDrawBuffers por defecto -- nadie la escribe ni la
    // limpia salvo ComponentRender::FlushEmissiveQueue(), que activa los 4 draw buffers solo en
    // frames con Mesh3D emisivos. Sin emisivos, coste cero (ni escritura, ni clear, ni lectura).
    glGenTextures(1, &gBuffer.emission);
    glBindTexture(GL_TEXTURE_2D, gBuffer.emission);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthRender, heightRender, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gBuffer.emission, 0);

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

    GLenum gBufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (gBufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("[Window] G-Buffer: Framebuffer no está completo! (status 0x%X) at %dx%d", gBufferStatus, widthRender, heightRender);
        exit(-1);
    }

    LOG_MESSAGE("[ComponentWindow] G-Buffer created successful (%d, %d)", widthRender, heightRender);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);
}

void ComponentWindow::ResizeGBuffer()
{
    LOG_MESSAGE("[Window] Resizing GBuffer...");
    CreateGBuffer();
    CreatePickingColorBuffer();
}

void ComponentWindow::UpdateWindowSize()
{
    SDL_GetWindowSize(window, &widthWindow, &heightWindow);
    if (!customRenderResolution) {
        widthRender  = widthWindow;
        heightRender = heightWindow;
    }
}

unsigned int ComponentWindow::getObjectIDByPickingColorFramebuffer(const int x, const int y) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, pickingColorBuffer.FBO);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(pickingColorBuffer.FBO);

    unsigned char pixel[4];
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    const int flippedY = getHeightRender() - y - 1;
    glReadPixels(x, flippedY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixel);

    const auto c = Color(
        (float) pixel[0] / 255.0f,
        (float) pixel[1] / 255.0f,
        (float) pixel[2] / 255.0f
    );

    return Color::colorToId(c);
}

std::set<unsigned int> ComponentWindow::getObjectIDsInPickingRect(int x1, int y1, int x2, int y2) const
{
    std::set<unsigned int> ids;

    // Normalise rect so x1<=x2, y1<=y2
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);

    const int w = x2 - x1 + 1;
    const int h = y2 - y1 + 1;
    if (w <= 0 || h <= 0) return ids;

    glBindFramebuffer(GL_FRAMEBUFFER, pickingColorBuffer.FBO);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(pickingColorBuffer.FBO);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);

    std::vector<unsigned char> pixels(w * h * 3);
    // OpenGL Y is flipped
    const int flippedY = getHeightRender() - y2 - 1;
    glReadPixels(x1, flippedY, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    for (int i = 0; i < w * h; i++) {
        const float r = pixels[i * 3 + 0] / 255.0f;
        const float g = pixels[i * 3 + 1] / 255.0f;
        const float b = pixels[i * 3 + 2] / 255.0f;
        const unsigned int id = Color::colorToId(Color(r, g, b));
        if (id != 0) ids.insert(id);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);
    return ids;
}

void ComponentWindow::beginAsyncPickingRect(int x1, int y1, int x2, int y2) const
{
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);

    const int w = x2 - x1 + 1;
    const int h = y2 - y1 + 1;
    if (w <= 0 || h <= 0) return;

    if (rectPickingPBO == 0) glGenBuffers(1, &rectPickingPBO);

    const size_t size = (size_t)w * h * 3;
    glBindBuffer(GL_PIXEL_PACK_BUFFER, rectPickingPBO);
    glBufferData(GL_PIXEL_PACK_BUFFER, (GLsizeiptr)size, nullptr, GL_STREAM_READ);

    glBindFramebuffer(GL_FRAMEBUFFER, pickingColorBuffer.FBO);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(pickingColorBuffer.FBO);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    const int flippedY = getHeightRender() - y2 - 1;
    glReadPixels(x1, flippedY, w, h, GL_RGB, GL_UNSIGNED_BYTE, nullptr); // async — offset 0

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    Profiler::get()->incrementFboChanges();
    Components::get()->Render()->setLastFrameBufferUsed(0);

    pendingPBOX1 = x1; pendingPBOY1 = y1;
    pendingPBOX2 = x2; pendingPBOY2 = y2;
    pendingPBOValid = true;
}

std::set<unsigned int> ComponentWindow::readAsyncPickingRect() const
{
    std::set<unsigned int> ids;
    if (!pendingPBOValid || rectPickingPBO == 0) return ids;

    const int w = pendingPBOX2 - pendingPBOX1 + 1;
    const int h = pendingPBOY2 - pendingPBOY1 + 1;

    glBindBuffer(GL_PIXEL_PACK_BUFFER, rectPickingPBO);
    const auto* data = static_cast<const GLubyte*>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY));

    if (data) {
        const int total = w * h;
        for (int i = 0; i < total; i++) {
            const float r = data[i * 3 + 0] / 255.0f;
            const float g = data[i * 3 + 1] / 255.0f;
            const float b = data[i * 3 + 2] / 255.0f;
            const unsigned int id = Color::colorToId(Color(r, g, b));
            if (id != 0) ids.insert(id);
        }
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
    }

    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
    pendingPBOValid = false;
    return ids;
}

void ComponentWindow::setImGuiConfig(Config::ImGUIConfigs c)
{
    ImGuiConfigChanged = c;
}

void ComponentWindow::forceReloadLayout()
{
    auto current = ImGuiConfig;
    ImGuiConfig = static_cast<Config::ImGUIConfigs>(-1);
    ImGuiConfigChanged = current;
}

void ComponentWindow::CheckForResizeOpenGLWindow(const SDL_Event &e)
{
    if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        LOG_WARNING(
            "[Window] Detected size windows changed! window=%dx%d render=%dx%d -> event=%dx%d",
            widthWindow, heightWindow, widthRender, heightRender, e.window.data1, e.window.data2
        );
        UpdateWindowSize();
        LOG_WARNING("[Window] After UpdateWindowSize: window=%dx%d render=%dx%d", widthWindow, heightWindow, widthRender, heightRender);
        glViewport(0,0, getWidth(), getHeight());
        ResetFramebuffer();
    }
}

void ComponentWindow::LoadCursorImage(const std::string &path)
{
    if (!Tools::FileExists(path.c_str())) {
        LOG_ERROR("[Window] Icon mouse file has failed: '%s'", path.c_str());
        return;
    }

    auto surface = Tools::SafeIMGLoad(path);

    cursor = SDL_CreateColorCursor(surface, 0, 0);
    SDL_SetCursor(cursor);

    SDL_FreeSurface(surface);

    if (!cursor) {
        LOG_ERROR("[Window] SDL_CreateColorCursor failed: %s", SDL_GetError());
        return;
    }

    SDL_SetCursor(cursor);

    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
    SDL_ShowCursor(SDL_ENABLE);

    LOG_MESSAGE("[Window] Icon from file '%s' loaded successfully...", path.c_str());
    customCursor = true;
}

void ComponentWindow::UpdateMouseCursor() const
{
    if (customCursor) {
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
        SDL_SetCursor(cursor);
        SDL_ShowCursor(SDL_ENABLE);
    } else {
        SDL_SetCursor(SDL_GetDefaultCursor());
        SDL_ShowCursor(SDL_ENABLE);
    }
}

void ComponentWindow::setImGuiMouse()
{
    customCursor = false;
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
    SDL_SetCursor(SDL_GetDefaultCursor());
    SDL_ShowCursor(SDL_ENABLE);
    io.WantCaptureMouse = true;
}

void ComponentWindow::setWindowSize(int w, int h)
{
    widthWindow = w;
    heightWindow = h;
}

void ComponentWindow::setRendererSize(int w, int h)
{
    LOG_SUCCESS("[Window] Set Renderer size to %dx%d", w, h);
    widthRender = w;
    heightRender = h;
    customRenderResolution = true;
    ResetFramebuffer();
}

// Used when restoring a project's last-known render size at load time -- this is just the
// starting size, not an intentional "lock render resolution" choice (that's setRendererSize(),
// used by the "Render size" menu). Must NOT set customRenderResolution, otherwise every project
// that has ever been saved permanently decouples render size from the window: UpdateWindowSize()
// stops following window resizes for the rest of the session (see git history around 2026-09-15).
void ComponentWindow::setInitialRendererSize(int w, int h)
{
    widthRender = w;
    heightRender = h;
}
