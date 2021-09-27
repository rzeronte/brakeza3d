#ifndef BRAKEDA3D_COMPONENTGUI_H
#define BRAKEDA3D_COMPONENTGUI_H


#include "Component.h"
#include "../GUI/GUIManager.h"

class ComponentGUI : public Component {
public:
    ComponentGUI(bool &finish);

    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    SDL_Window *window{};
    SDL_Renderer *renderer{};
    SDL_GLContext *contextOpenGL{};
    GUIManager *managerGUI;

    bool &finish;

    SDL_Window *getWindow() const;

    void setWindow(SDL_Window *window);

    SDL_Renderer *getRenderer() const;

    void setRenderer(SDL_Renderer *renderer);

    SDL_GLContext *getContextOpenGl() const;

    void setContextOpenGl(SDL_GLContext *contextOpenGl);

    GUIManager *getManagerGUI() const;

    void setFinish(bool &finish);
};


#endif //BRAKEDA3D_COMPONENTGUI_H
