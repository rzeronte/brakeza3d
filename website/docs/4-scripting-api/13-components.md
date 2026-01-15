---
sidebar_position: 14
title: 4.13) Components
---

# Components
---

Below is a detailed list of the methods available through each component.


## Component Window
---

The `Window` component is responsible for initializing the operating system window and managing the
framebuffers that are displayed on screen.

Through your LUA scripts, you can access the following methods:

| Function                       | Description                                              |
|--------------------------------|----------------------------------------------------------|
| `getWidth()`                   | Returns the window width in pixels                       |
| `getHeight()`                  | Returns the window height in pixels                      |
| `getWidthRender()`             | Returns the render width in pixels                       |
| `getHeightRender()`            | Returns the render height in pixels                      |
| `isWindowMaximized()`          | Returns whether the window is currently maximized        |
| `setWindowTitle(string)`       | Sets the window title                                    |
| `ToggleFullScreen()`           | Switches the window between fullscreen and windowed mode |
| `setWindowSize(int, int)`      | Changes the window size                                  |
| `setRendererSize(int, int)`    | Changes the renderer size                                |
| `LoadCursorImage(string path)` | Loads the specified image as the mouse cursor            |



## Component Render
---

The `Render` component is responsible for handling global illumination, shader management among other
rendering-related tasks.

Through your LUA scripts, you can access the following methods:

| Function                           | Description                                                                  |
|------------------------------------|------------------------------------------------------------------------------|
| `getSceneLoader()`                 | Returns the scene loader component used to load and manage scenes            |
| `setGlobalIlluminationDirection()` | Sets the direction of the global illumination light                          |
| `setGlobalIlluminationAmbient()`   | Sets the ambient color or intensity of the global illumination               |
| `setGlobalIlluminationDiffuse()`   | Sets the diffuse color or intensity of the global illumination               |
| `setGlobalIlluminationSpecular()`  | Sets the specular color or intensity of the global illumination              |
| `getSceneShaderByLabel()`          | Returns a scene shader by its assigned label                                 |
| `getFps()`                         | Returns frames per second                                                    |
| `getSelectedObject()`              | Returns a pointer to the selected object in the GUI                          |
| `MakeScreenShot(string path)`      | Makes a PNG screenshot at the given path                                     |
| `DrawLine()`                       | Draws a line in the scene, typically for debugging or visualization purposes |


## Component Input
---

The Input component provides an interface to access keyboard, mouse, and game controller input methods.

| Function                   | Description                                         |
|----------------------------|-----------------------------------------------------|
| `setKeyboardEnabled(bool)` | Enables or disables camera movement via keyboard    |
| `setMouseEnabled(bool)`    | Enables or disables mouse look/rotation             |
| `setPadEnabled(bool)`      | Enables or disables gamepad look/rotation           |
| `isKeyboardEnabled()`      | Returns whether keyboard camera movement is enabled |
| `isMouseEnabled()`         | Returns whether mouse look/rotation is enabled      |
| `isPadEnabled()`           | Returns whether gamepad look/rotation is enabled    |

Through your LUA scripts, you can access the following methods:

| Function                          | Description                                                                  |
|-----------------------------------|------------------------------------------------------------------------------|
| `isKeyEventDown()`                | Returns true when a key-down event is detected                               |
| `isKeyEventUp()`                  | Returns true when a key-up (key release) event is detected                   |
| `isCharPressed()`                 | Checks if a character key is currently pressed                               |
| `isCharFirstEventDown()`          | Returns true only on the first press of a character key, ignoring key repeat |
| `isAnyControllerButtonPressed()`  | Returns true if any game controller button is pressed                        |
| `isMouseMotion()`                 | Returns true when mouse movement is detected                                 |
| `isClickLeft()`                   | Returns true when the left mouse button is clicked                           |
| `isClickRight()`                  | Returns true when the right mouse button is clicked                          |
| `getRelativeRendererMouseX()`     | Returns the mouse X position relative to the renderer viewport               |
| `getRelativeRendererMouseY()`     | Returns the mouse Y position relative to the renderer viewport               |
| `getMouseMotionXRel()`            | Returns the relative horizontal mouse movement since the last frame          |
| `getMouseMotionYRel()`            | Returns the relative vertical mouse movement since the last frame            |
| `isLeftMouseButtonPressed()`      | Returns true while the left mouse button is being held down                  |
| `isRightMouseButtonPressed()`     | Returns true while the right mouse button is being held down                 |
| `isGameControllerEnabled()`       | Returns true if a game controller is connected and enabled                   |
| `getControllerButtonA()`          | Returns the state of the controller A button                                 |
| `getControllerButtonB()`          | Returns the state of the controller B button                                 |
| `getControllerButtonX()`          | Returns the state of the controller X button                                 |
| `getControllerButtonY()`          | Returns the state of the controller Y button                                 |
| `getControllerAxisTriggerLeft()`  | Returns the analog value of the left trigger axis                            |
| `getControllerAxisTriggerRight()` | Returns the analog value of the right trigger axis                           |
| `getControllerAxisLeftX()`        | Returns the horizontal axis value of the left analog stick                   |
| `getControllerAxisLeftY()`        | Returns the vertical axis value of the left analog stick                     |
| `getControllerAxisRightX()`       | Returns the horizontal axis value of the right analog stick                  |
| `getControllerAxisRightY()`       | Returns the vertical axis value of the right analog stick                    |
| `getControllerPadUp()`            | Returns true if the D-Pad Up button is pressed                               |
| `getControllerPadDown()`          | Returns true if the D-Pad Down button is pressed                             |
| `getControllerPadLeft()`          | Returns true if the D-Pad Left button is pressed                             |
| `getControllerPadRight()`         | Returns true if the D-Pad Right button is pressed                            |
| `getControllerShoulderLeft()`     | Returns true if the left shoulder button is pressed                          |
| `getControllerShoulderRight()`    | Returns true if the right shoulder button is pressed                         |
| `getControllerButtonBack()`       | Returns true if the Back / Select button is pressed                          |
| `getControllerButtonGuide()`      | Returns true if the Guide / Home button is pressed                           |
| `getControllerButtonStart()`      | Returns true if the Start button is pressed                                  |



## Component Camera
---

| Function                       | Description                            |
|--------------------------------|----------------------------------------|
| `getCamera()`                  | Returns the camera's `Object3D`        |
| `getGLMMat4ViewMatrix()`       | Returns the camera's view matrix       |
| `getGLMMat4ProjectionMatrix()` | Returns the camera's projection matrix |


## Component Collisions
---

| Function               | Description                                             |
|------------------------|---------------------------------------------------------|
| `setEnabled()`         | Enables or disables the component or object             |
| `isRayCollisionWith()` | Checks if a ray collides with the object/component      |
| `setEnableDebugMode()` | Enables or disables debug mode for the object/component |


## Component Scripting
---

The Scripting component provides an interface to control the execution flow of scripts within your application.

Through your LUA scripts, you can access the following methods:

| Function                | Description                                            |
|-------------------------|--------------------------------------------------------|
| `PlayLUAScripts()`      | Starts the execution of all active LUA scripts         |
| `StopLUAScripts()`      | Stops the execution of all running LUA scripts         |
| `ReloadLUAScripts()`    | Reloads all LUA scripts and restarts their execution   |
| `AddSceneLUAScript()`   | Adds a LUA script that is executed at the scene level  |
| `AddProjectLUAScript()` | Adds a global LUA script executed at the project level |