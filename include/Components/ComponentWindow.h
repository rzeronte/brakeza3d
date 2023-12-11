//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTWINDOW_H
#define BRAKEDA3D_COMPONENTWINDOW_H

#include <SDL2/SDL_video.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include "Component.h"
#include "../Render/ShaderOpenGL.h"
#include "../Render/ShaderOpenGLImage.h"
#include "../Render/ShaderOpenGLRender.h"
#include "../Render/ShaderOpenGLLine.h"

class ComponentWindow : public Component {
private:
    SDL_Window *window;

    SDL_Renderer *renderer;
    SDL_Surface *screenSurface;
    SDL_Texture *screenTexture;

    TTF_Font *fontDefault;
    TTF_Font *fontAlternative;

    ShaderOpenGLRender *shaderOGLRender;
    ShaderOpenGLImage *shaderOGLImage;
    ShaderOpenGLLine *shaderOGLLine;
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

    void clearVideoBuffers();

    TTF_Font *getFontAlternative();

    SDL_Surface *applicationIcon;

    void initOpenGL();

    SDL_GLContext context;

    [[nodiscard]] ShaderOpenGLImage *getShaderOGLImage() const;

    [[nodiscard]] ShaderOpenGLRender *getShaderOGLRender() const;

    ShaderOpenGLLine *getShaderOGLLine() const;

};


#endif //BRAKEDA3D_COMPONENTWINDOW_H
