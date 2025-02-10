//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTWINDOW_H
#define BRAKEDA3D_COMPONENTWINDOW_H

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include "Component.h"


enum class ImGUIConfigs {
    DEFAULT,
    DESIGN,
    CODING
};

class ComponentWindow : public Component {
private:
    SDL_Window *window;

    SDL_Renderer *renderer;
    SDL_Surface *screenSurface;
    SDL_Texture *screenTexture;

    TTF_Font *fontDefault;

    GLuint sceneFramebuffer;
    GLuint depthTexture;

    GLuint backgroundFramebuffer;
    GLuint backgroundTexture;

    GLuint foregroundFramebuffer;
    GLuint foregroundTexture;

    GLuint uiFramebuffer;
    GLuint uiTexture;

    GLuint globalFramebuffer;
    GLuint postProcessingFramebuffer;


    ImGUIConfigs ImGuiConfig;

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

    [[nodiscard]] SDL_Window *getWindow() const;

    [[nodiscard]] SDL_Renderer *getRenderer() const;

    [[nodiscard]] TTF_Font *getFontDefault();

    SDL_Surface *applicationIcon;

    static void initOpenGL();

    SDL_GLContext context;

    [[nodiscard]] GLuint getSceneFramebuffer() const;

    void RenderLayersToGlobalFramebuffer() const;
    void resetFramebuffer();
    bool isWindowMaximized();

    [[nodiscard]] GLuint getBackgroundFramebuffer() const;

    [[nodiscard]] GLuint getUIFramebuffer() const;

    [[nodiscard]] GLuint getForegroundFramebuffer() const;


    bool screenShoot = false;

    void cleanFrameBuffers() const;


    [[nodiscard]] int getWidth() const;

    [[nodiscard]] int getHeight() const;

    int width;
    int height;

    void createFramebuffer();


    GLuint sceneTexture;

    [[nodiscard]] GLuint getGlobalFramebuffer() const;

    GLuint globalTexture;
    GLuint postProcessingTexture;

    [[nodiscard]] GLuint getPostProcessingFramebuffer() const;

    void RenderLayersToMain();

    void ImGuiInitialize(const std::string &configFile);
    void saveImGuiCurrentLayout() const;
    void ImGuiOnUpdate();

    ImGUIConfigs ImGuiConfigChanged;

    void setWindowTitle(const char *title);

    void toggleFullScreen();
};


#endif //BRAKEDA3D_COMPONENTWINDOW_H

