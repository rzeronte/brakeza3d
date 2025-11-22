#ifndef BRAKEDA3D_COMPONENTINPUT_H
#define BRAKEDA3D_COMPONENTINPUT_H

#include <SDL2/SDL.h>
#include "Component.h"

class ComponentInput : public Component {
    bool mouseMotion = false;
    float mouseMotionXRel = 0.f;
    float mouseMotionYRel = 0.f;
    bool mouseLeftButton = false;
    bool mouseRightButton = false;
    bool mouseButtonDown = false;
    bool mouseButtonUp = false;
    bool drag = false;

    Uint8 *keyboard = nullptr;
    std::unordered_map<SDL_Keycode, bool> keyboardEvents;

    Uint8 controllerButtonA = 0;
    Uint8 controllerButtonB = 0;
    Uint8 controllerButtonX = 0;
    Uint8 controllerButtonY = 0;

    float controllerAxisLeftX = 0.f;
    float controllerAxisLeftY = 0.f;

    float controllerAxisRightX = 0.f;
    float controllerAxisRightY = 0.f;

    float controllerAxisTriggerLeft = 0.f;
    float controllerAxisTriggerRight = 0.f;

    Uint8 controllerPadUp = 0;
    Uint8 controllerPadDown = 0;
    Uint8 controllerPadLeft = 0;
    Uint8 controllerPadRight = 0;

    Uint8 controllerShoulderLeft = 0;
    Uint8 controllerShoulderRight = 0;

    Uint8 controllerButtonBack = 0;
    Uint8 controllerButtonGuide = 0;
    Uint8 controllerButtonStart = 0;

    Uint32 mouseButtons = 0;;

    int mouseX = 0;
    int mouseY = 0;
    int relativeRendererMouseX = 0;
    int relativeRendererMouseY = 0;

    _SDL_GameController *gameController = nullptr;

    bool keyUpEvent = false;
    bool keyDownEvent = false;

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

    void handleKeyboardMovingCamera();

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
