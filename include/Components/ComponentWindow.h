//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTWINDOW_H
#define BRAKEDA3D_COMPONENTWINDOW_H

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include "Component.h"
#include "ImGuizmo.h"
#include "../OpenGL/CommonOGLShaderTypes.h"
#include "../Render/PostProcessingManager.h"


struct Resolution {
    int width;
    int height;
    const char* label;
};

static const Resolution RENDER_RESOLUTIONS[] = {
    {320, 240, "320x240 (Very Low)"},
    {640, 480, "640x480 (Low)"},
    {800, 600, "800x600"},
    {960, 540, "960x540"},
    {1024, 768, "1024x768"},
    {1280, 720, "1280x720 (HD)"},
    {1366, 768, "1366x768"},
    {1600, 900, "1600x900"},
    {1920, 1080, "1920x1080 (Full HD)"},
};

class ComponentWindow : public Component
{
    int widthWindow = Config::get()->screenWidth;
    int heightWindow = Config::get()->screenHeight;
    int widthRender = Config::get()->renderWidth;
    int heightRender = Config::get()->renderHeight;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    TTF_Font *fontDefault = nullptr;

    OpenGLGBuffer gBuffer;
    OpenGLPickingBuffer pickingColorBuffer;
    OpenGLGlobalFramebuffers openGLBuffers;

    PostProcessingManager *postProcessingManager = nullptr;

    ImGuizmo::OPERATION ImGuiOperationGuizmo = ImGuizmo::TRANSLATE;

    Config::ImGUIConfigs ImGuiConfig = Config::ImGUIConfigs::DEFAULT;
    Config::ImGUIConfigs ImGuiConfigChanged = Config::ImGUIConfigs::DEFAULT;

    SDL_GLContext context = nullptr;
    SDL_Surface *applicationIcon = nullptr;

    bool customCursor = false;
    SDL_Cursor* cursor = nullptr;

public:
    ComponentWindow();
    void onStart() override;
    void preUpdate() override;
    void onUpdate() override;
    void postUpdate() override;
    void onEnd() override;
    void onSDLPollEvent(SDL_Event *event, bool &finish) override;
    void InitWindow();
    void InitFontsTTF();
    void ResetFramebuffer();
    void ClearOGLFrameBuffers() const;
    void CheckForResizeOpenGLWindow(const SDL_Event &e);
    void CreateGBuffer();
    void ResizeGBuffer();
    void UpdateWindowSize();
    void CreateFramebuffer();
    void FlipGlobalToWindow();
    void SaveImGuiCurrentLayout() const;
    void ImGuiOnUpdate();
    void ToggleFullScreen() const;
    void CreatePickingColorBuffer();
    void ImGuiInitialize(const std::string &configFile);
    void UpdateMouseCursor() const;
    void LoadCursorImage(const std::string &path);

    [[nodiscard]] bool isWindowMaximized() const;
    OpenGLGBuffer& getGBuffer()                                             { return gBuffer; }
    OpenGLGlobalFramebuffers &getGlobalBuffers()                            { return openGLBuffers; }
    OpenGLPickingBuffer &getPickingColorFramebuffer()                       { return pickingColorBuffer; }
    [[nodiscard]] int getWidth() const                                      { return widthWindow; }
    [[nodiscard]] int getHeight() const                                     { return heightWindow; }
    [[nodiscard]] int getWidthRender() const                                { return widthRender; }
    [[nodiscard]] int getHeightRender() const                               { return heightRender; }
    [[nodiscard]] GLuint getSceneFramebuffer() const                        { return openGLBuffers.sceneFBO; }
    [[nodiscard]] GLuint getBackgroundFramebuffer() const                   { return openGLBuffers.backgroundFBO; }
    [[nodiscard]] GLuint getUIFramebuffer() const                           { return openGLBuffers.uiFBO; }
    [[nodiscard]] GLuint getForegroundFramebuffer() const                   { return openGLBuffers.foregroundFBO; }
    [[nodiscard]] GLuint getGlobalFramebuffer() const                       { return openGLBuffers.globalFBO; }
    [[nodiscard]] GLuint getSceneTexture() const                            { return openGLBuffers.sceneTexture; }
    [[nodiscard]] GLuint getGlobalTexture() const                           { return openGLBuffers.globalTexture; }
    [[nodiscard]] SDL_Window *getWindow() const                             { return window; }
    [[nodiscard]] SDL_Renderer *getRenderer() const                         { return renderer; }
    [[nodiscard]] TTF_Font *getFontDefault() const                          { return fontDefault; }
    [[nodiscard]] ImGuizmo::OPERATION getGuiZmoOperation() const            { return ImGuiOperationGuizmo; }
    [[nodiscard]] Config::ImGUIConfigs getImGuiConfig() const               { return ImGuiConfig; }
    [[nodiscard]] PostProcessingManager *getPostProcessingManager() const   { return postProcessingManager; }
    [[nodiscard]] unsigned int getObjectIDByPickingColorFramebuffer(int x, int y) const;

    void setWindowTitle(const char *title) const;
    void setGuiZmoOperation(ImGuizmo::OPERATION operation);
    void setImGuiConfig(Config::ImGUIConfigs c);
    void setImGuiMouse();
    void setWindowSize(int w, int h);
    void setRendererSize(int w, int h);

    static void ResetOpenGLSettings();
};


#endif //BRAKEDA3D_COMPONENTWINDOW_H

