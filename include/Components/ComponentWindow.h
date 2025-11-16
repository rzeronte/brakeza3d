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


enum class ImGUIConfigs {
    DEFAULT,
    DESIGN,
    CODING
};

struct GBuffer {
    GLuint FBO;
    GLuint gPosition;
    GLuint gNormal;
    GLuint gAlbedoSpec;
    GLuint rboDepth;

    [[nodiscard]] GLuint getFBO() const {
        return FBO;
    }
    [[nodiscard]] GLuint getPositions() const {
        return gPosition;
    }
    [[nodiscard]] GLuint getAlbedo() const {
        return gAlbedoSpec;
    }
    [[nodiscard]] GLuint getNormal() const {
        return gNormal;
    }
    [[nodiscard]] GLuint getDepth() const {
        return rboDepth;
    }
};

class ComponentWindow : public Component
{
    SDL_Window *window;

    SDL_Renderer *renderer;
    SDL_Surface *screenSurface;
    SDL_Texture *screenTexture;

    TTF_Font *fontDefault;

    GLuint globalFramebuffer;
    GLuint sceneFramebuffer;
    GLuint backgroundFramebuffer;
    GLuint foregroundFramebuffer;
    GLuint uiFramebuffer;
    GLuint postProcessingFramebuffer;

    GLuint globalTexture;
    GLuint sceneTexture;
    GLuint backgroundTexture;
    GLuint foregroundTexture;
    GLuint uiTexture;
    GLuint postProcessingTexture;
    GLuint depthTexture;

    ImGuizmo::OPERATION guizmoOperation;

    ImGUIConfigs ImGuiConfig;

    GBuffer gBuffer;

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

    [[nodiscard]] TTF_Font *getFontDefault() const;

    SDL_Surface *applicationIcon;

    static void initOpenGL();

    SDL_GLContext context;

    [[nodiscard]] GLuint getSceneFramebuffer() const;

    void RenderLayersToGlobalFramebuffer() const;
    void resetFramebuffer();
    bool isWindowMaximized() const;

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

    [[nodiscard]] GLuint getGlobalFramebuffer() const;

    [[nodiscard]] GLuint getPostProcessingFramebuffer() const;

    void RenderLayersToMain() const;

    void ImGuiInitialize(const std::string &configFile);
    void saveImGuiCurrentLayout() const;
    void ImGuiOnUpdate();

    ImGUIConfigs ImGuiConfigChanged;

    void setWindowTitle(const char *title) const;

    void toggleFullScreen() const;

    ImGuizmo::OPERATION getGuizmoOperation() const;

    void setGuizmoOperation(ImGuizmo::OPERATION guizmoOperation);

    GLuint getDepthTexture() const;

    GLuint getSceneTexture() const;

    GLuint getGlobalTexture() const;

    GBuffer& getGBuffer();

    void createGBuffer();

    void resizeGBuffer();
};


#endif //BRAKEDA3D_COMPONENTWINDOW_H

