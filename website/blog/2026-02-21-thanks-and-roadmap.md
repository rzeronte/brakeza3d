---
slug: 2026-02-21-thanks-and-road-map
title: Thanks and Roadmap!
authors: [eduardo]
tags: [brakeza3d, roadmap, community]
---

First of all, **thank you**.

I want to take a moment to thank everyone who has emailed me and shown so much interest after the latest video about the **Brakeza3D 0.26.2** release. The response from the community has been incredibly positive and motivating, and it truly means a lot to see so many people interested, testing the engine, and asking how they can use it or contribute.

## About the roadmap

One of the questions that keeps coming up in messages is:

> *What is the project's roadmap?*

The answer is clear and honest:  
**The current priority is not to add lots of new features, but to stabilize and consolidate the project.**

The main focus right now is:

- Stabilizing the engine
- Improving internal consistency
- Exposing **100% of the engine’s functionality to LUA**
- Properly documenting that API

At the moment, the LUA API is still limited, and before expanding the engine with new capabilities, I want to make sure everything that already exists is:

- accessible
- stable
- documented
- maintainable

This step is essential to make the project truly usable in the long term.


## Priorities for the coming months

The priority order will be:

<!-- truncate -->

1. **Bug fixing**
2. **Documenting the LUA API and the C++ core**
3. **Desired features**
4. **Long-term features**

This order is intentional. An engine with many features but poor stability or documentation ends up being frustrating for users. I prefer building a solid foundation before accelerating development.

## Desired features (mid-term)

These are some improvements planned that will be implemented progressively:

- Improve user experience in the interface
- Enable geometry streaming for Mesh3D objects
- Add a basic version of node-based geometry shaders

Many of these ideas come directly from community feedback, which helps a lot when deciding what actually brings value.

## Long-term features (or subject to reconsideration)

There are also deeper evolution paths that require analysis and experimentation before any decision is made:

- Decouple OpenGL / Create a Vulkan backend
- Explore alternative scripting language options
- Change the project's license type

These decisions affect the architecture, philosophy, and long-term direction of the engine, so they will be evaluated carefully and with community feedback.

## Thanks again

To close, I want to repeat the most important thing:  
**thank you for the interest, feedback, and support.**

This project keeps moving forward thanks to you.

Let’s keep going 🚀