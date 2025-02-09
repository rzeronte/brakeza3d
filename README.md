# Brakeza3D - Free OpenSource 3D Engine

Brakeza3D is an open source 3D engine written from scratch in C++


- Cross Platform (Windows/Linux/OSX checked)
- Easy and simple architecture (Based in components easily maintainable)
- SDL2 + OpenGL 4 render
- Bullet Physics Integration
- ImGui Docking Integration
- Assimp library for loading
- Scene management (JSON based files)
- 2D Billboard / 3D Billboard (8 directions) 
- ADS light system
- Decals
- Shadow mapping
- LUA Scripting
- Extensible GLSL Shaders
- Basic component for add sounds
- Interface for XBOX/Playstation controller

---

## Dependences

- SDL2
- SDL2_image
- SDL2_ttf
- SDL2_mixer
- Bullet Physics
- Assimp
- OpenGL
- GLM
- LUA5.2

---

## Linux compilation

This project has been checked on Ubuntu 20.10 from CLI

In Unix systems we will only have to make sure that we have installed the development packages of the libraries that we need

``` 
apt-get install libsdl2-2.0-0 libsdl2-dev libsdl2-image-dev libsdl2-image-2.0-0 libsdl2-ttf-dev \
libsdl2-ttf-2.0-0 libsdl2-mixer-dev libsdl2-mixer-2.0-0 libbullet-dev libbullet3.06 libassimp-dev \
libassimp5 assimp-utils libglm-dev liblua5.2-dev \
liblua5.2-dev liblua5.2-0
```

``` 
git clone https://github.com/rzeronte/brakeza3d.git brakeza3D
cd brakeza3D
mkdir build && cd build
cmake ..
make
```
---

## Windows compilation

We have precompiled libraries for compiling with MinGW tested with CLion.

https://github.com/rzeronte/brakeza-mingw-precompiled-libraries

---

## OSX compilation

Currently, I do not compile for Mac, but the project was started on it, so that it shouldn't vary much compared to compiling with Linux, once the dependencies are satisfied.

---
## Documentation

Here you can find some documentation for starting:

https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md

---

## Brakeza3D - Screenshots

![Screenshoot01](/doc/screenshots/01.png)

![Screenshoot02](/doc/screenshots/02.png)

![Screenshoot03](/doc/screenshots/03.png)

![Screenshoot04](/doc/screenshots/04.png)

![Screenshoot05](/doc/screenshots/05.png)

![Screenshoot06](/doc/screenshots/06.png)

![Screenshoot07](/doc/screenshots/07.png)
