#ifndef BRAKEDA3D_COMPONENTINPUT_H
#define BRAKEDA3D_COMPONENTINPUT_H

#include <SDL2/SDL.h>
#include "Component.h"

class ComponentInput : public Component {
private:
    bool mouseMotion;
    bool mouseLeftButton;
    bool mouseRightButton;

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
    bool keyUpEvent;
    bool keyDownEvent;

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

    void handleMouse(SDL_Event *) const;

    void handleProjectileDemo(SDL_Event *pEvent);

    void updateMouseMapping();

    void updateGamePadStates();

    [[nodiscard]] bool isAnyControllerButtonPressed() const;

    void initJoystick();
    
    [[nodiscard]] bool isLeftMouseButtonPressed() const;

    [[nodiscard]] bool isRightMouseButtonPressed() const;

    [[nodiscard]] bool isClickLeft() const;

    [[nodiscard]] bool isClickRight() const;

    [[nodiscard]] int getRelativeRendererMouseX() const;
    
    [[nodiscard]] int getRelativeRendererMouseY() const;

    [[nodiscard]] bool isMouseMotion() const;

    [[nodiscard]] float getControllerAxisTriggerLeft() const;

    [[nodiscard]] float getControllerAxisTriggerRight() const;

    [[nodiscard]] Uint8 *getKeyboard() const;

    [[nodiscard]] Uint8 getControllerButtonA() const;

    [[nodiscard]] Uint8 getControllerButtonB() const;

    [[nodiscard]] Uint8 getControllerButtonX() const;

    [[nodiscard]] Uint8 getControllerButtonY() const;

    [[nodiscard]] float getControllerAxisLeftX() const;

    [[nodiscard]] float getControllerAxisLeftY() const;

    [[nodiscard]] float getControllerAxisRightX() const;

    [[nodiscard]] float getControllerAxisRightY() const;

    void handleToggleImGui(SDL_Event *event);

    [[maybe_unused]] bool isCharPressed(const char *character);

    void updateKeyboardStates(SDL_Event *event);

    [[nodiscard]] bool isKeyEventDown() const;

    [[nodiscard]] bool isKeyEventUp() const;

    [[nodiscard]] _SDL_GameController *getGameController() const;

    void handleCheckPadConnection(SDL_Event *pEvent);
};


#endif //BRAKEDA3D_COMPONENTINPUT_H
