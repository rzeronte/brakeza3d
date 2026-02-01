---
sidebar_position: 1
title: Scripting introduction
---

# Scripting introduction
---

Although the engine is internally developed in C++ to ensure high performance and efficiency, **Brakeza3D** provides an **API accessible through the LUA language**.

Through this abstraction layer, developers can manipulate objects, control behaviors, and manage different aspects of the engine in a simple and flexible way, **without directly interacting with native code**. This enables rapid iteration, higher productivity, and a more accessible learning curve when creating logic and systems within the engine.

## Main concepts
---

### Oriented objects

In general, **any element that can be displayed on screen is an object**, for
example: 3D models, lights, particle emitter...

All objects **share the basic properties** of a 3D element, such as name, position, rotation, and scale. **Each
specific object type can extend these properties as needed.** From code, we can activate, move, rotate, delete, scale, and perform many other actions on objects.

### Script files

The engine supports scripting using `LUA language`, which can be attached to individual objects or entire scenes.
Scripts can be assigned via drag-and-drop directly from the GUI.

During execution, script variables **can be monitored and modified in real time**, allowing for efficient
debugging and behavior tuning without recompilation.

## Scripting System States
---

In the main loop of **Brakeza3D**, a series of actions are executed continuously. One of them is the scripting system. This system can be in `ON` or `OFF`. If it is `ON`, objects will execute their life cycle as implemented in their scripts.

You can also `RELOAD` the system to reset scripts and refresh them from disk. Additionally, these states can be controlled directly from the GUI, allowing you to toggle the scripting system or reload scripts without modifying code.



## Scripts LUA
---

`LUA scripts` are elements that can be linked to system elements. In them, we implement the logic and behavior
of objects.

### Object scripts

Associated with objects. The same script can be linked to multiple objects.

:::note
Object's scripts **instantiate their variables for each object to which they are linked**.
:::

### Global scripts

Associated with the scene or project, not with any specific object; they are general in nature.

:::note
Global scripts **share variables freely among themselves**.
:::

The main difference with an object script is the *scope* of their variables.


## Object life cycle
---

The objects `Object3D` have their own life cycle in `LUA Scripts`, which is important to understand when working with loaded objects.

### On Start

The moment execution begins. Triggered when the scripting system is activated (PLAY).

```lua
function onStart()
    -- code to execute at the start of the script, only once
end
```

### On Update

The current moment, i.e., every frame.

```lua
function onUpdate()
    -- code to execute every frame
end
```


## Variables
---

Any LUA script can define variables to help implement logic. The GUI allows easy management of script
variables.

Physically, variables are stored in a JSON file with the same name as the script.

```json
{
    "name":	"global_script_example.lua",
    "types": [
        {
          "name": "var1",
          "type": "string",
          "value": "hello my friend!"
        },
        {
          "name": "var2",
          "type": "int",
          "value": 10
        },
        {
          "name": "var3",
          "type": "float",
          "value": 0.3
        },
        {
          "name": "var4",
          "type": "Vertex3D",
          "value": {
            "x": 0,
            "y": 2,
            "z": 0
          }
        }
    ]
}
```

You can use the types: int, float, string, and Vertex3D.


### Global variables

Variables defined in scripts linked to projects and/or scenes are global.

You can access global variables directly from any other script:

```lua
function onUpdate()
    var1 = var1 .. "!"                  -- demo global variable
    print("Value of var1: " .. var1)
end
```

### Local variables

Variables defined in scripts linked to `Object3D` are local, meaning they are instantiated individually
for each object.

You can access local variables of another object from your LUA scripts as follows:

```lua
o = Brakeza:getSceneObjectByLabel("MyObject")
position = o:getLocalScriptVar("offset")                            -- we get a vertex3D!

print("Read variable 'offset' from object: ".. o:getName())
print("Value for 'offset': " .. position.x .. ", " .. position.y .. ", " .. position.z)

print("Read variable 'count' from object: ".. o:getName())
print("Value for 'count': " .. o:getLocalScriptVar("count"))        -- we get a int!
```

## Scene Management
---

You can load and save scenes from both the GUI and your LUA scripts:

```lua
function onStart()
    ...
    Components:Render():getSceneLoader():LoadScene("../scenes/scene_example.json")
    ...
    Components:Render():getSceneLoader():SaveScene("../scenes/scene_example.json")
    ...
end
```


## Linking Scripts
---

You can perform these operations from the **UI using drag-and-drop**. Specifically, you can link LUA scripts from the GUI to *projects*, *scenes*, or *individual objects*. These links are saved to disk at the scene and/or project level.

However, sometimes you may want to create these links dynamically from code. You can do it in the following way:  `Object3D` has a method `AttachScript(ScriptLUA)` that allows linking scripts:

```lua
lightpoint = ObjectFactory.LightPoint(
    Vertex3D.new(10, 20, 30),                               -- position
    Color.new(0.1, 0.1, 0.1),                               -- ambient component
    Color.new(0.2, 0.4, 0.6),                               -- diffuse component
    Color.new(0.5, 0.3, 1.0),                               -- specular component
);

script = ObjectFactory.ScriptLUA("../../scripts/MoveForwardObject.lua")
if script ~= nil then
    lightp:AttachScript(script)
end
```

Similarly, the component `Scripting` object has `AddSceneLUAScript()` to link a script to the scene:

```lua
script = ObjectFactory.ScriptLUA("../../scripts/global_script.lua")
if script ~= nil then
    Components:Scripting():AddSceneLUAScript(script)
end
```

Or a project:

```lua
script = ObjectFactory.ScriptLUA("../../scripts/global_script.lua")
if script ~= nil then
    Components:Scripting():AddProjectLUAScript(script)
end
```


## Deltatime
---

DeltaTime is the time it takes to render a frame. It is crucial in game development, as it ensures movements
and animations are consistent regardless of frame rate.

You can access it in your LUA scripts as follows:

```lua
...
print("DeltaTime: " .. Brakeza:getDeltaTime())                  -- seconds
print("DeltaTimeInMicro: " .. Brakeza:getDeltaTimeMicro())      -- microseconds
print("Execution Time: " .. Brakeza:getExecutionTime())         -- total execution time
...
function onUpdate()
    local speed = 5.0                                           -- units per second
    local movement = speed * Brakeza:getDeltaTime()
    myObject:addToPosition(Vertex3D.new(movement, 0, 0))
end
```


## Terminating Execution
---

If you want to terminate the application from code, you can do so as follows:

```lua
Brakeza:Shutdown()
```


## Auto-loading Projects or Scenes
---

When packaging your game or application, you may want Brakeza3D to automatically load and execute scripts
from a specific project. You can instruct Brakeza3D to do this via the command line:

Windows:
```bash
> brakeza3d.exe -p MyProject.json 
> brakeza3d.exe --project MyProject.json
```

This will run the project automatically without the UI.

:::note
The project file path is relative to the base projects directory: **/assets/projects/**
:::



## Components
---

**Brakeza3D** organizes its functionality into components. Each component represents a fundamental aspect of the engine core.

    - `Window`: Manages the operating system window
    - `Scripting`: Manages the scripting system
    - `Camera`: Manages the camera
    - `Collisions`: Manages the physics and collision engine
    - `Input`: Manages keyboard or gamepad input
    - `Sound`: Manages sound playback
    - `Render`: Manages rendering with OpenGL

You can access all of them through the component manager from your Lua scripts. Each component will be explained in detail later.