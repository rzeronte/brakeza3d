---
slug: ai-friendly-game-engine
title: "What You Can Build with Brakeza3D, and Using It with AI Agents"
description: "What projects suit Brakeza3D, an open source C++ game engine with Lua, and why its text formats and command-line tests work well with AI coding agents."
authors: [eduardo]
tags: [brakeza3d, ai, gamedev, candelabra]
---

Two questions keep coming up in messages: *what is Brakeza3D good for?* and *can I use it with Claude, Copilot or other coding agents?* This post answers both, because the answers are connected.

<!-- truncate -->

## What kind of projects suit Brakeza3D

Brakeza3D is a free, open source engine written in C++ (OpenGL, deferred rendering, Bullet physics, Lua scripting, GLSL and node-based shaders, integrated editor). It is built for **small and medium real-time projects**, not for a studio shipping a 100-person AAA game. In practice, that covers:

- **Prototypes and game jams.** Open the editor, drop in a model, attach a Lua script, press play. No recompiling the engine to try an idea.
- **Strategy and simulation games.** [Candelabra](/blog/making-of-candelabra), a city-sized RTS with hundreds of units, civilians, traffic, police and a day/night cycle, runs on it. It is also the hardest test the engine has had, and it keeps pushing the engine forward.
- **3D action and exploration games** with physics, lighting, particles and animated characters. The tutorial project in the engine shows each piece in isolation: physics, characters, particles and crowds, UI, audio and video, shaders.
- **Visual experiments and shader work.** The node-based shader editor and GLSL support make it a comfortable sandbox for graphics programming.
- **Learning.** The whole engine is readable C++ with a documented Lua API, so you can follow a feature from the script down to the renderer.

What it is not: a drop-in replacement for the largest commercial engines, with their asset stores and console toolchains. If you want full control over the pipeline and a small, understandable codebase, it fits.

## Why it works well with AI agents

I did not design Brakeza3D around AI. But a lot of the choices that make an engine pleasant for a human who likes plain files turn out to be exactly what a coding agent needs.

**1. Almost everything is text.** Projects, scenes and game data are JSON. Gameplay is Lua. Shaders are GLSL. There is no opaque binary project format between the agent and your game: it reads a file, changes a file, and you can review the change in git like any other.

**2. The scripting API is documented.** The [Scripting API](/docs/scripting-api/index) covers objects, physics, sound, shaders, UI widgets and more, and the engine's own source tree has a project instruction file for agents (`CLAUDE.md`) describing the architecture, the frame loop order and the conventions to follow. An agent that reads it starts with the same map I use.

**3. Lua means fast feedback.** An agent can write a script, run the game and look at the result without a C++ build in the loop. When it does need to touch the engine, the code follows the conventions described in that instruction file.

**4. Command-line parameters turn the game into a test.** A project can declare parameters that you pass at launch with `--set name=value`. In Candelabra I use them for scenario tests: spawn 200 units against 200, run for a fixed time, exit, write a report to a log file. An agent can launch that, read the report and decide the next step, with no human clicking in the editor.

**5. The engine can measure itself.** There is a built-in profiler exposed to Lua, so a script can collect CPU and GPU time per phase, draw calls and per-script cost and write them as text. An agent that can read numbers can tell whether its change helped.

### A real example

I used an AI agent on exactly that loop to optimize the 200 vs 200 battle in Candelabra. The agent profiled the scenario, found that every bullet created a physical object with a script, replaced straight bullets with a targeted-hit model, capped the visual effects, and lowered the update rate of systems that did not need one. In the steady scenario the frame went from about **205 ms to about 117-124 ms**, in measured steps, and every change was a reviewable commit. It also made mistakes along the way (a wrong guess about where time went, an optimization that gave no gain and was reverted), and the measurements are what caught them. That is the real point: the engine gives the agent a way to check its own work.

## Getting started with an agent

1. [Download](/downloads) the engine or clone the [repository](https://github.com/rzeronte/brakeza3d).
2. Open the repository in your agent of choice, and point it at `CLAUDE.md` and the [Scripting API documentation](/docs/scripting-api/index).
3. Start with a small task: a script that moves an object, a new shader, a UI widget.
4. Ask it to run the project with a command-line parameter and to read the log, not just to write code.

The pattern that has worked best for me: **small tasks, a way to run the result, and a number or a log to check it against**. Brakeza3D makes the second and third easy.

If you build something with it, I would like to see it: the [gallery](/gallery) and the [Assets Hub](/assets-hub) are open to the community.
