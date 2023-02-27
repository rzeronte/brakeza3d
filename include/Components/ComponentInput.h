#ifndef BRAKEDA3D_COMPONENTINPUT_H
#define BRAKEDA3D_COMPONENTINPUT_H


#include <SDL2/SDL.h>
#include "Component.h"
#include "ComponentCamera.h"

class ComponentInput : public Component {
private:
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

    float controllerAxisRightX;
    float controllerAxisRightY;

    float controllerAxisTriggerLeft;
    float controllerAxisTriggerRight;

    Uint32 mouseButtons;
    int mouseX;
    int mouseY;
    int relativeRendererMouseX;
    int relativeRendererMouseY;

    _SDL_GameController *gameController;
public:
    ComponentInput();

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    static void handleWindowEvents(SDL_Event *e, bool &);

    void updateKeyboardMapping();

    void updateMouseStates(SDL_Event *e);

    void handleKeyboardMovingCamera() const;

    void handleMouse(SDL_Event *);

    void handleProjectileDemo(SDL_Event *pEvent);

    [[nodiscard]] bool isLeftMouseButtonPressed() const;

    [[nodiscard]] bool isRightMouseButtonPressed() const;

    void updateMouseMapping();

    [[nodiscard]] bool isClickLeft() const;

    [[nodiscard]] bool isClickRight() const;

    [[nodiscard]] int getRelativeRendererMouseX() const;
    [[nodiscard]] int getRelativeRendererMouseY() const;

    [[nodiscard]] bool isMouseMotion() const;

    void updateGamePadStates();

    bool isAnyControllerButtonPressed();

    void initJostick();

    [[nodiscard]] float getControllerAxisTriggerLeft() const;

    [[nodiscard]] float getControllerAxisTriggerRight() const;

    [[nodiscard]] Uint8 *getKeyboard() const;

    [[nodiscard]] Uint8 getControllerButtonA() const;

    [[nodiscard]] Uint8 getControllerButtonB() const;

    [[nodiscard]] Uint8 getControllerButtonX() const;

    [[nodiscard]] Uint8 getControllerButtonY() const;

    float getControllerAxisLeftX() const;

    float getControllerAxisLeftY() const;

    float getControllerAxisRightX() const;

    float getControllerAxisRightY() const;
};


#endif //BRAKEDA3D_COMPONENTINPUT_H
