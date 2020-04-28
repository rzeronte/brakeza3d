//
// Created by darkhead on 8/1/20.
//

#ifndef BRAKEDA3D_COMPONENTWINDOW_H
#define BRAKEDA3D_COMPONENTWINDOW_H


#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include "Component.h"


class ComponentWindow : public Component {

public:

    ComponentWindow();

    void onStart();
    void preUpdate();
    void onUpdate();
    void postUpdate();
    void onEnd();
    void onSDLPollEvent(SDL_Event *event, bool &finish);

    SDL_GLContext contextOpenGL;
    SDL_Window    *window;
    SDL_Renderer  *renderer;
    SDL_Surface   *screenSurface;
    SDL_Texture   *screenTexture;
    SDL_Rect      screenRect;

    TTF_Font *fontSmall   = NULL;
    TTF_Font *fontDefault = NULL;
    TTF_Font *fontMedium  = NULL;
    TTF_Font *fontBig     = NULL;


    void initWindow();
    void initFontsTTF();
};


#endif //BRAKEDA3D_COMPONENTWINDOW_H
