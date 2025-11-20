#ifndef BRAKEDA3D_COMPONENTINPUT_H
#define BRAKEDA3D_COMPONENTINPUT_H

#include <SDL2/SDL.h>
#include "Component.h"

class ComponentInput : public Component {
private:
    bool mouseMotion;
    float mouseMotionXRel;
    float mouseMotionYRel;
    bool mouseLeftButton;
    bool mouseRightButton;
    bool mouseButtonDown;
    bool mouseButtonUp;
    bool drag;

    Uint8 *keyboard;
    std::unordered_map<SDL_Keycode, bool> keyboardEvents;

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

    Uint8 controllerPadUp;
    Uint8 controllerPadDown;
    Uint8 controllerPadLeft;
    Uint8 controllerPadRight;

    Uint8 controllerShoulderLeft;
    Uint8 controllerShoulderRight;

    Uint8 controllerButtonBack;
    Uint8 controllerButtonGuide;
    Uint8 controllerButtonStart;

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

    void resetKeyboardMapping();

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    static void handleWindowEvents(SDL_Event *e, bool &);

    void updateMouseStates(SDL_Event *e);

    void handleKeyboardMovingCamera() const;

    void handleMouse(SDL_Event *);

    void handleProjectileDemo(SDL_Event *pEvent);

    void resetMouseMapping();

    void updateGamePadStates();

    [[nodiscard]] bool isAnyControllerButtonPressed() const;

    void initJoystick();
    
    [[nodiscard]] bool isLeftMouseButtonPressed() const;

    [[nodiscard]] bool isRightMouseButtonPressed() const;

    [[nodiscard]] bool isClickLeft() const;

    [[nodiscard]] bool isClickRight() const;

    [[nodiscard]] int getRelativeRendererMouseX() const;

    int getMouseX() const;

    int getMouseY() const;

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

    void handleToggleKeys(SDL_Event *event);

    [[maybe_unused]] bool isCharPressed(const char *character);

    [[maybe_unused]] bool isCharFirstEventDown(const char *character);

    void updateKeyboardStates(SDL_Event *event);

    [[nodiscard]] bool isKeyEventDown() const;

    [[nodiscard]] bool isKeyEventUp() const;

    [[nodiscard]] _SDL_GameController *getGameController() const;

    void handleCheckPadConnection(SDL_Event *pEvent);

    float getMouseMotionXRel() const;

    float getMouseMotionYRel() const;

    bool isMouseButtonUp() const;

    bool isMouseButtonDown() const;

    bool isDrag() const;

    void handleDeleteSelectedObject(SDL_Event *e);

    bool isGameControllerEnabled();

    Uint8 getControllerPadUp() const;

    Uint8 getControllerPadDown() const;

    Uint8 getControllerPadLeft() const;

    Uint8 getControllerPadRight() const;

    Uint8 getControllerShoulderLeft() const;

    Uint8 getControllerShoulderRight() const;

    Uint8 getControllerButtonBack() const;

    Uint8 getControllerButtonGuide() const;

    Uint8 getControllerButtonStart() const;
};


#endif //BRAKEDA3D_COMPONENTINPUT_H
