---
sidebar_position: 10
title: Input system
---

# Input system
---

**Brakeza3D** incorporates mechanisms to handle keyboard, mouse, and gamepad input.
You can access most of the available input-related functionality through the `Input` component.

```lua
input = Components:Input()
isEnabled = input::isEnabled()
```


## Keyboard


    - isKeyEventDown(): Returns true when a key-down event is detected.
    - isKeyEventUp(): Returns true when a key-up (key release) event is detected.
    - isCharPressed(): Checks if a character key is currently pressed.
    - isCharFirstEventDown(): Returns true only on the first press of a character key, ignoring key repeat.
    - isAnyControllerButtonPressed(): Returns true if any game controller button is pressed.



## Mouse


    - isMouseMotion(): Returns true when mouse movement is detected.
    - isClickLeft(): Returns true when the left mouse button is clicked.
    - isClickRight(): Returns true when the right mouse button is clicked.
    - getRelativeRendererMouseX(): Returns the mouse X position relative to the renderer viewport.
    - getRelativeRendererMouseY(): Returns the mouse Y position relative to the renderer viewport.
    - getMouseMotionXRel(): Returns the relative horizontal mouse movement since the last frame.
    - getMouseMotionYRel(): Returns the relative vertical mouse movement since the last frame.
    - isLeftMouseButtonPressed(): Returns true while the left mouse button is being held down.
    - isRightMouseButtonPressed(): Returns true while the right mouse button is being held down.



## Pad


    - isGameControllerEnabled(): Returns true if a game controller is connected and enabled.
    - getControllerButtonA(): Returns the state of the controller A button.
    - getControllerButtonB(): Returns the state of the controller B button.
    - getControllerButtonX(): Returns the state of the controller X button.
    - getControllerButtonY(): Returns the state of the controller Y button.
    - getControllerAxisTriggerLeft(): Returns the analog value of the left trigger axis.
    - getControllerAxisTriggerRight(): Returns the analog value of the right trigger axis.
    - getControllerAxisLeftX(): Returns the horizontal axis value of the left analog stick.
    - getControllerAxisLeftY(): Returns the vertical axis value of the left analog stick.
    - getControllerAxisRightX(): Returns the horizontal axis value of the right analog stick.
    - getControllerAxisRightY(): Returns the vertical axis value of the right analog stick.
    - getControllerPadUp(): Returns true if the D-Pad Up button is pressed.
    - getControllerPadDown(): Returns true if the D-Pad Down button is pressed.
    - getControllerPadLeft(): Returns true if the D-Pad Left button is pressed.
    - getControllerPadRight(): Returns true if the D-Pad Right button is pressed.
    - getControllerShoulderLeft(): Returns true if the left shoulder button is pressed.
    - getControllerShoulderRight(): Returns true if the right shoulder button is pressed.
    - getControllerButtonBack(): Returns true if the Back / Select button is pressed.
    - getControllerButtonGuide(): Returns true if the Guide / Home button is pressed.
    - getControllerButtonStart(): Returns true if the Start button is pressed.