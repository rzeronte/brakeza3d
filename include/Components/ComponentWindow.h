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


class ComponentWindow : public Component
{
    int widthWindow = 0;
    int heightWindow = 0;
    int widthRender = 0;
    int heightRender = 0;

    bool screenShoot = false;

    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Surface *screenSurface = nullptr;
    SDL_Texture *screenTexture = nullptr;

    TTF_Font *fontDefault = nullptr;

    OpenGLGBuffer gBuffer;
    OpenGLPickingBuffer pickingColorBuffer;
    OpenGLGlobalFramebuffers openGLBuffers;

    PostProcessingManager *postProcessingManager;

    ImGuizmo::OPERATION ImGuiOperationGuizmo = ImGuizmo::TRANSLATE;

    Config::ImGUIConfigs ImGuiConfig = Config::ImGUIConfigs::DEFAULT;
    Config::ImGUIConfigs ImGuiConfigChanged = Config::ImGUIConfigs::DEFAULT;

    SDL_GLContext context = nullptr;
    SDL_Surface *applicationIcon;
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
    OpenGLGBuffer& getGBuffer()                         { return gBuffer; }
    OpenGLGlobalFramebuffers &getGlobalBuffers()        { return openGLBuffers; }
    OpenGLPickingBuffer &getPickingColorFramebuffer()   { return pickingColorBuffer;}

    int getWidth() const                                { return widthWindow;}
    int getHeight() const                               { return heightWindow;}
    int getWidthRender() const                          { return widthRender;}
    int getHeightRender() const                         { return heightRender;}
    bool isWindowMaximized() const;
    GLuint getSceneFramebuffer() const                  { return openGLBuffers.sceneFBO;}
    GLuint getBackgroundFramebuffer() const             { return openGLBuffers.backgroundFBO;}
    GLuint getUIFramebuffer() const                     { return openGLBuffers.uiFBO;}
    GLuint getForegroundFramebuffer() const             { return openGLBuffers.foregroundFBO;}
    GLuint getGlobalFramebuffer() const                 { return openGLBuffers.globalFBO;}
    GLuint getPostProcessingFramebuffer() const         { return openGLBuffers.postProcessingFBO;}
    GLuint getDepthTexture() const                      { return openGLBuffers.sceneDepthTexture;}
    GLuint getSceneTexture() const                      { return openGLBuffers.sceneTexture;}
    GLuint getGlobalTexture() const                     { return openGLBuffers.globalTexture;}
    SDL_Window *getWindow() const                       { return window;}
    SDL_Renderer *getRenderer() const                   { return renderer;}
    TTF_Font *getFontDefault() const                    { return fontDefault;}
    ImGuizmo::OPERATION getGuiZmoOperation() const      { return ImGuiOperationGuizmo;}
    Config::ImGUIConfigs getImGuiConfig() const         { return ImGuiConfig;}
    PostProcessingManager *getPostProcessingManager()   { return postProcessingManager;}

    void CreateFramebuffer();
    void FlipGlobalToWindow();
    void ImGuiInitialize(const std::string &configFile);
    void SaveImGuiCurrentLayout() const;
    void ImGuiOnUpdate();
    void setWindowTitle(const char *title) const;
    void toggleFullScreen() const;
    void setGuiZmoOperation(ImGuizmo::OPERATION operation);
    unsigned int getObjectIDByPickingColorFramebuffer(int x, int y) const;
    void CreatePickingColorBuffer();
    void CreateGBuffer();
    void ResizeGBuffer();
    void UpdateWindowSize();
    void setImGuiConfig(Config::ImGUIConfigs c);

    static void InitOpenGL();
};


#endif //BRAKEDA3D_COMPONENTWINDOW_H

