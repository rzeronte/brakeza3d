//
// Created by darkhead on 13/1/20.
//

#ifndef BRAKEDA3D_COMPONENTGUI_H
#define BRAKEDA3D_COMPONENTGUI_H


#include "Component.h"
#include "../GUI/GUIManager.h"

class ComponentGUI : public Component {
public:
    ComponentGUI();

    void onStart();
    void preUpdate();
    void onUpdate();
    void postUpdate();
    void onEnd();
    void onSDLPollEvent(SDL_Event *event, bool &finish);

    SDL_Window    *window;
    SDL_Renderer  *renderer;
    SDL_GLContext *contextOpenGL;
    GUIManager    *managerGUI;

    SDL_Window   *getWindow() const;
    void         setWindow(SDL_Window *window);

    SDL_Renderer *getRenderer() const;
    void         setRenderer(SDL_Renderer *renderer);

    SDL_GLContext *getContextOpenGl() const;
    void           setContextOpenGl(SDL_GLContext *contextOpenGl);

    GUIManager *getManagerGui() const;

    void setManagerGui(GUIManager *managerGui);
};


#endif //BRAKEDA3D_COMPONENTGUI_H
