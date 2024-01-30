# Brakeza3D - Free OpenSource 3D Engine

Brakeza3D is an open source 3D engine written from scratch in C++

![alt text](https://brakeza.com/wp-content/uploads/2024/01/brakeza3d-2024.png)

Main features:

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

# Dependences

- SDL2
- SDL2_image
- SDL2_ttf
- SDL2_mixer
- Bullet Physics
- Assimp
- OpenGL
- GLM
- LUA5.2

# Linux compilation

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

# Windows compilation

Is not complex, but you will have to download the compiler and libraries to compile it by yourself.

Here for detailed instructions for installing with CLion + MinGW:

https://github.com/rzeronte/brakeza3d/blob/master/WINDOWS_BUILD.md

# OSX compilation

Currently, I do not compile for Mac, but the project was started on it, so that it shouldn't vary much compared to compiling with Linux, once the dependencies are satisfied.

# Brakeza3D - Videos

![Screenshoot01](https://brakeza.com/wp-content/uploads/2024/01/screenshoot_2024_04.png)

![Screenshoot02](https://brakeza.com/wp-content/uploads/2024/01/screenshoot_2024_03.png)

![Screenshoot03](https://brakeza.com/wp-content/uploads/2024/01/screenshoot_2024_02.png)

![Screenshoot04](https://brakeza.com/wp-content/uploads/2024/01/screenshoot_2024_01.png)

![Screenshoot05](https://brakeza.com/wp-content/uploads/2024/01/brakeza3d-2024-menu.png)
