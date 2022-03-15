#ifndef BRAKEDA3D_COMPONENTINPUT_H
#define BRAKEDA3D_COMPONENTINPUT_H


#include <SDL2/SDL.h>
#include "Component.h"
#include "ComponentCamera.h"

class ComponentInput : public Component {
public:
    bool enabled;
    bool debug = true;
    bool click = false;
    bool mousePressed = false;
    bool mouseMotion = false;
    bool mouseLeftButton = false;
    bool mouseRightButton = false;

    Uint8 *keyboard;

    Uint8 controllerButtonA;
    Uint8 controllerButtonB;
    Uint8 controllerButtonX;
    Uint8 controllerButtonY;
    float controllerAxisLeftX;
    float controllerAxisLeftY;

    Uint32 mouseButtons;
    int mouseX;
    int mouseY;
    int relativeRendererMouseX;
    int relativeRendererMouseY;

    ComponentInput();

    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    bool isEnabled() const;

    void setEnabled(bool enabled);

    static void handleWindowEvents(SDL_Event *e, bool &);

    void updateKeyboardMapping();

    void updateMouseStates(SDL_Event *e);

    void handleKeyboardMovingCamera() const;

    void handleMouse(SDL_Event *);

    void handleProjectileDemo(SDL_Event *pEvent);

    bool isLeftMouseButtonPressed() const;

    bool isRightMouseButtonPressed() const;

    void updateMouseMapping();

    bool isClickLeft() const;

    bool isClickRight() const;

    int getRelativeRendererMouseX() const;
    int getRelativeRendererMouseY() const;

    bool isMouseMotion() const;

    void updateGamePadStates();
};


#endif //BRAKEDA3D_COMPONENTINPUT_H
