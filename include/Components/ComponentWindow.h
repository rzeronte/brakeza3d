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
    void initWindow();
    void initFontsTTF();
    OpenGLGBuffer& getGBuffer();
    OpenGLGlobalFramebuffers &getGlobalBuffers();
    OpenGLPickingBuffer &getPickingColorFramebuffer();
    void resetFramebuffer();
    void ClearOGLFrameBuffers() const;
    void CheckForResizeOpenGLWindow(const SDL_Event &e);
    [[nodiscard]] int getWidth() const;
    [[nodiscard]] int getHeight() const;
    [[nodiscard]] int getWidthRender() const;
    [[nodiscard]] int getHeightRender() const;
    [[nodiscard]] bool isWindowMaximized() const;
    [[nodiscard]] GLuint getSceneFramebuffer() const;
    [[nodiscard]] GLuint getBackgroundFramebuffer() const;
    [[nodiscard]] GLuint getUIFramebuffer() const;
    [[nodiscard]] GLuint getForegroundFramebuffer() const;
    [[nodiscard]] GLuint getGlobalFramebuffer() const;
    [[nodiscard]] GLuint getPostProcessingFramebuffer() const;
    [[nodiscard]] GLuint getDepthTexture() const;
    [[nodiscard]] GLuint getSceneTexture() const;
    [[nodiscard]] GLuint getGlobalTexture() const;
    [[nodiscard]] SDL_Window *getWindow() const;
    [[nodiscard]] SDL_Renderer *getRenderer() const;
    [[nodiscard]] TTF_Font *getFontDefault() const;
    void createFramebuffer();
    void RenderLayersToMain();
    void ImGuiInitialize(const std::string &configFile);
    void saveImGuiCurrentLayout() const;
    void ImGuiOnUpdate();
    void setWindowTitle(const char *title) const;
    void toggleFullScreen() const;
    void setGuiZmoOperation(ImGuizmo::OPERATION operation);
    [[nodiscard]] ImGuizmo::OPERATION getGuiZmoOperation() const;
    [[nodiscard]] int getObjectIDByPickingColorFramebuffer(int x, int y) const;
    [[nodiscard]] Config::ImGUIConfigs getImGuiConfig() const;
    void createPickingColorBuffer();
    void createGBuffer();
    void resizeGBuffer();
    void UpdateWindowSize();
    void setImGuiConfig(Config::ImGUIConfigs c);
    static void initOpenGL();
};


#endif //BRAKEDA3D_COMPONENTWINDOW_H

