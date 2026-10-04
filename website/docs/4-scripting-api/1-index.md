---
sidebar_position: 1
title: Scripting introduction
description: Introduction to Lua scripting in Brakeza3D, including object lifecycle, variables, scene management, and components.
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
o = Brakeza:getObjectByName("MyObject")
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

Scenes, models and images load asynchronously in background jobs. You can check whether loading has finished, for example to keep a loading screen visible, and cancel what is still queued:

| Function | Description |
|----------|-------------|
| `Brakeza:getPendingJobsCount()` | Number of loading jobs still queued, running or waiting to be applied (`0` = everything loaded) |
| `Brakeza:cancelPendingJobs()` | Drops every queued job and every result waiting to be applied. Jobs already running finish, but their results are discarded. Useful before restarting a level |

```lua
function onUpdate()
    if loading and Brakeza:getPendingJobsCount() == 0 then
        loading = false
        hideLoadingScreen()
    end
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
print("DeltaTimeInMS: " .. Brakeza:getDeltaTimeMS())            -- milliseconds
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

This closes the application with exit code `0`. If you need to report a specific exit code instead
(for example, to tell an external script whether a test run passed or failed), use `requestExit`:

```lua
Brakeza:requestExit(1)   -- terminates the application, process exit code = 1
```

:::note
`Brakeza:requestExit(code)` is the mechanism used by CLI automation (see below) to report results —
for instance, a headless test runner can check the process exit code without parsing any log file.
:::


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

## Passing Custom Parameters to a Project
---

Beyond `-p`, Brakeza3D accepts a generic, repeatable `--set key=value` flag on the command line.
The engine itself does **not** interpret these values — it only stores them. What each key means is
entirely up to your project's own Lua scripts, which read them back through `Brakeza:getCliParam()`.
This keeps the engine agnostic of any project-specific vocabulary while still letting you drive a
project's behavior from the command line — useful for automated test scenarios, CI pipelines, batch
rendering, or any other unattended run.

```bash
> brakeza3d.exe -p MyProject.json --set level=level_03 --set skipIntro=true
```

```lua
function onStart()
    local level     = Brakeza:getCliParam("level")       -- "level_03" (string)
    local skipIntro = Brakeza:getCliParam("skipIntro")    -- true (boolean)

    if level then
        print("Booting into level: " .. level)
    end
end
```

### Declaring accepted parameters (`cli_params`)

A project can optionally declare, in its own project JSON (`/assets/projects/MyProject.json`), which
parameters it expects, along with their type and default value. This has two benefits: `getCliParam`
returns a properly typed value (`string`, `bool`, or `number`) instead of a raw string, and Brakeza3D
logs a warning for any `--set` that doesn't match a declared name — handy for catching typos.

```json
{
  "name": "MyProject",
  "cli_params": [
    { "name": "level",      "type": "string", "default": "level_01" },
    { "name": "skipIntro",  "type": "bool",   "default": false }
  ]
}
```

If a parameter isn't passed on the command line, `getCliParam` returns the declared `default`. If a
project doesn't declare `cli_params` at all, `getCliParam` simply returns `nil` for any key.

:::note
`--set` values are always read as strings from the command line; the declared `type` is what tells
Brakeza3D how to convert them (`"true"`/`"1"` → `true` for `bool`, numeric parsing for `number`).
:::


## Unattended / Headless Runs
---

Combine the parameters above with an exit watchdog to run Brakeza3D fully unattended — for example,
from a CI job or a script that launches the engine, waits for it to finish, and inspects the result:

```bash
> brakeza3d.exe -p MyProject.json --set level=level_03 --exit-after 300
```

`--exit-after <seconds>` force-quits the process after the given number of seconds regardless of what
the project does, as a safety net against a run that never finishes on its own — the project can
still request an earlier, deliberate exit at any point via `Brakeza:requestExit(code)` (see
[Terminating Execution](#terminating-execution) above).

:::note
When autoloading a project (`-p`), Brakeza3D also mirrors its log output to stdout, since there is no
ImGui console to look at in that mode — useful when piping output to a file for later inspection.
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