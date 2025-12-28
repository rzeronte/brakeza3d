#ifndef BRAKEDA3D_COMPONENTINPUT_H
#define BRAKEDA3D_COMPONENTINPUT_H

#include <SDL2/SDL.h>
#include "Component.h"

class ComponentInput : public Component
{
    bool keyboardEnabled = true;
    bool mouseEnabled = true;
    bool padEnabled = true;

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
    ComponentInput() = default;
    void onStart() override;
    void ResetKeyboardMapping();
    void preUpdate() override;
    void onUpdate() override;
    void postUpdate() override;
    void onEnd() override;
    void InitJoystick();
    void onSDLPollEvent(SDL_Event *event, bool &finish) override;
    void UpdateMouseStates(SDL_Event *e);
    void HandleKeyboardMovingCamera() const;
    void HandleMouseLook(SDL_Event *);
    void ResetMouseMapping();
    void UpdateGamePadStates();
    void HandleCheckPadConnection(SDL_Event *pEvent);
    void HandleDeleteSelectedObject(SDL_Event *e) const;

    void setKeyboardEnabled(bool value);

    void setMouseEnabled(bool value);

    void setPadEnabled(bool value);

    void UpdateKeyboardStates(SDL_Event *event);
    void HandleGUIShortCuts(SDL_Event *event) const;
    float getMouseMotionXRel() const { return mouseMotionXRel; }
    float getMouseMotionYRel() const { return mouseMotionYRel; }
    Uint8 getControllerPadUp() const                                { return controllerPadUp; }
    Uint8 getControllerPadDown() const                              { return controllerPadDown; }
    Uint8 getControllerPadLeft() const                              { return controllerPadLeft; }
    Uint8 getControllerPadRight() const                             { return controllerPadRight; }
    Uint8 getControllerShoulderLeft() const                         { return controllerShoulderLeft; }
    Uint8 getControllerShoulderRight() const                        { return controllerShoulderRight; }
    Uint8 getControllerButtonBack() const                           { return controllerButtonBack; }
    Uint8 getControllerButtonGuide() const                          { return controllerButtonGuide; }
    Uint8 getControllerButtonStart() const                          { return controllerButtonStart; }
    int getMouseX() const                                           { return mouseX; }
    int getMouseY() const                                           { return mouseY; }
    [[nodiscard]] Uint8 *getKeyboard() const                        { return keyboard; };
    [[nodiscard]] Uint8 getControllerButtonA() const                { return controllerButtonA; }
    [[nodiscard]] Uint8 getControllerButtonB() const                { return controllerButtonB; }
    [[nodiscard]] Uint8 getControllerButtonX() const                { return controllerButtonX; }
    [[nodiscard]] Uint8 getControllerButtonY() const                { return controllerButtonY; }
    [[nodiscard]] int getRelativeRendererMouseY() const             { return relativeRendererMouseY; }
    [[nodiscard]] float getControllerAxisTriggerLeft() const        { return controllerAxisTriggerLeft; }
    [[nodiscard]] float getControllerAxisTriggerRight() const       { return controllerAxisTriggerRight; }
    [[nodiscard]] float getControllerAxisLeftX() const              { return controllerAxisLeftX; }
    [[nodiscard]] float getControllerAxisLeftY() const              { return controllerAxisLeftY; }
    [[nodiscard]] float getControllerAxisRightX() const             { return controllerAxisRightX; }
    [[nodiscard]] float getControllerAxisRightY() const             { return controllerAxisRightY; }
    [[nodiscard]] _SDL_GameController *getGameController() const    { return gameController; }
    [[nodiscard]] int getRelativeRendererMouseX() const             { return relativeRendererMouseX; }
    [[nodiscard]] bool isMouseMotion() const                        { return mouseMotion;}
    [[nodiscard]] bool isKeyEventDown() const                       { return keyDownEvent; }
    [[nodiscard]] bool isKeyEventUp() const                         { return keyUpEvent; }
    [[nodiscard]] bool isLeftMouseButtonPressed() const             { return mouseLeftButton; }
    [[nodiscard]] bool isRightMouseButtonPressed() const            { return mouseRightButton; }
    [[nodiscard]] bool isClickLeft() const                          { return mouseLeftButton; }
    [[nodiscard]] bool isClickRight() const                         { return mouseRightButton; }
    [[nodiscard]] bool isKeyboardEnabled() const                    { return keyboardEnabled; }
    [[nodiscard]] bool isMouseEnabled() const                       { return mouseEnabled; }
    [[nodiscard]] bool isPadEnabled() const                         { return padEnabled; }
    bool isMouseButtonUp() const                                    { return mouseButtonUp; }
    bool isMouseButtonDown() const                                  { return mouseButtonDown; }
    bool isDrag() const                                             { return drag; }
    bool isGameControllerAvailable() const                          { return gameController != nullptr; }

    [[nodiscard]] bool isAnyControllerButtonPressed() const;
    [[maybe_unused]] bool isCharFirstEventDown(const char *character);
    [[maybe_unused]] bool isCharPressed(const char *character) const;

    static void HandleWindowEvents(SDL_Event *event, bool &);
};


#endif //BRAKEDA3D_COMPONENTINPUT_H
