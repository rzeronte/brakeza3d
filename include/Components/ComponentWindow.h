//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTWINDOW_H
#define BRAKEDA3D_COMPONENTWINDOW_H

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include "Component.h"
#include "../OpenGL/ShaderOpenGL.h"
#include "../OpenGL/ShaderOpenGLImage.h"
#include "../OpenGL/ShaderOpenGLRender.h"
#include "../OpenGL/ShaderOpenGLLine.h"
#include "../OpenGL/ShaderOpenGLWireframe.h"
#include "../OpenGL/ShaderOpenGLShading.h"
#include "../OpenGL/ShaderOpenGLPoints.h"
#include "../OpenGL/ShaderOpenGLOutline.h"
#include "../OpenGL/ShaderOpenGLColor.h"
#include "../OpenGL/ShaderOpenGLParticles.h"
#include "../OpenGL/ShaderOpenGLDOF.h"
#include "../OpenGL/ShaderOpenGLDepthMap.h"
#include "../OpenGL/ShaderOpenGLFOG.h"

class ComponentWindow : public Component {
private:
    SDL_Window *window;

    SDL_Renderer *renderer;
    SDL_Surface *screenSurface;
    SDL_Texture *screenTexture;

    TTF_Font *fontDefault;
    TTF_Font *fontAlternative;

    GLuint sceneFramebuffer;
    GLuint sceneTexture;
    GLuint depthTexture;

    GLuint backgroundFramebuffer;
    GLuint backgroundTexture;

    GLuint foregroundFramebuffer;
    GLuint foregroundTexture;

    GLuint uiFramebuffer;
    GLuint uiTexture;

    GLuint globalFramebuffer;
    GLuint globalTexture;

    ShaderOpenGLRender *shaderOGLRender;
    ShaderOpenGLImage *shaderOGLImage;
    ShaderOpenGLLine *shaderOGLLine;
    ShaderOpenGLWireframe *shaderOGLWireframe;
    ShaderOpenGLShading *shaderOGLShading;
    ShaderOpenGLPoints *shaderOGLPoints;
    ShaderOpenGLOutline *shaderOGLStencil;
    ShaderOpenGLColor *shaderOGLColor;
    ShaderOpenGLParticles *shaderOGLParticles;
    ShaderOpenGLDOF *shaderOGLDOF;
    ShaderOpenGLDepthMap *shaderOGLDepthMap;
    ShaderOpenGLFOG *shaderOGLFOG;
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

    void renderToWindow();

    TTF_Font *getFontAlternative();

    SDL_Surface *applicationIcon;

    void initOpenGL();

    SDL_GLContext context;

    [[nodiscard]] ShaderOpenGLImage *getShaderOGLImage() const;

    [[nodiscard]] ShaderOpenGLRender *getShaderOGLRender() const;

    [[nodiscard]] ShaderOpenGLLine *getShaderOGLLine() const;

    [[nodiscard]] ShaderOpenGLWireframe *getShaderOglWireframe() const;

    [[nodiscard]] ShaderOpenGLShading *getShaderOglShading() const;

    [[nodiscard]] ShaderOpenGLPoints *getShaderOGLPoints() const;

    [[nodiscard]] ShaderOpenGLOutline *getShaderOglStencil() const;

    [[nodiscard]] ShaderOpenGLColor *getShaderOglColor() const;

    [[nodiscard]] ShaderOpenGLParticles *getShaderOglParticles() const;

    [[nodiscard]] GLuint getSceneFramebuffer() const;

    void renderFramebuffer();
    void resetFramebuffer();

    [[nodiscard]] GLuint getBackgroundFramebuffer() const;

    [[nodiscard]] GLuint getUIFramebuffer() const;

    [[nodiscard]] GLuint getForegroundFramebuffer() const;

    ShaderOpenGLDOF *getShaderOGLDOF() const;

    bool screenShoot = false;

    void cleanFrameBuffers() const;

    ShaderOpenGLFOG *getShaderOGLFOG() const;

    int getWidth() const;

    int getHeight() const;


    int width;
    int height;

    void createFramebuffer();
};


#endif //BRAKEDA3D_COMPONENTWINDOW_H

