//
// Created by eduardo on 7/11/21.
//

#ifndef BRAKEDA3D_COMPONENTCONSOLE_H
#define BRAKEDA3D_COMPONENTCONSOLE_H


#include "Component.h"
#include "../../console/ConsoleManager.h"

class ComponentConsole : public Component {
public:
    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    void setRenderer(SDL_Renderer *newRenderer);
    void setTextureRender(SDL_Texture *newTexture);

private:
    bool enabled;

    SDL_Renderer *renderer;
    SDL_Texture *screenTexture;

    ConsoleManager manager;

    SDL_Surface *backgroundCanvas;
    SDL_Surface *foregroundCanvas;
};


#endif //BRAKEDA3D_COMPONENTCONSOLE_H
