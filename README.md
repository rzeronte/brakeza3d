# Brakeza3D - Free OpenSource 3D Engine

Brakeza3D is an open source 3D engine written from scratch with SDL2.0

Custom CPU/OpenCL render is made (No OpenCL / Direct3D. )

Main features:

- Cross Platform (Windows/Linux/OSX checked)
- Easy architecture (Based in components easily maintainable)
- Bullet Physics Integration
- ImGui Integration
- CPU/OpenCL render engine
- Load models & animations with ASSIMP library
- 2D Billboard
- 3D Billboard (8 directions)
- Decals  
- UV Mapping
- Light points / Specularity
- Shadow real time
- Game Menu Interface
- Grid3D / Octrees
- Basic component for add sounds
- Interface for XBOX/Playstation controller
- Extensible Shaders vía custom OpenCL kernels

# Dependences

- SDL2
- SDL2_image
- SDL2_ttf
- SDL2_mixer
- Bullet Physics
- Assimp
- OpenCL SDK
- GLM

# Linux compilation

This project has been checked on Ubuntu 20.10 from CLI

In Unix systems we will only have to make sure that we have installed the development packages of the libraries that we need

``` 
apt-get install libsdl2-2.0-0 libsdl2-dev libsdl2-image-dev libsdl2-image-2.0-0 libsdl2-ttf-dev \
libsdl2-ttf-2.0-0 libsdl2-mixer-dev libsdl2-mixer-2.0-0 libbullet-dev libbullet2.88 libassimp-dev \
libassimp5 assimp-utils intel-opencl-icd intel-opencl-icd libglm-dev
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

https://github.com/rzeronte/brakeza3d/WINDOWS_BUILD.md

# OSX compilation

Currently, I do not compile for Mac, but the project was started on it, so that it shouldn't vary much compared to compiling with Linux, once the dependencies are satisfied.

# Brakeza3D - Videos

[![BSP Loader](http://i3.ytimg.com/vi/g_AP3GaFYQs/hqdefault.jpg)](https://youtu.be/g_AP3GaFYQs "BSP Loader")

[![Rasterizer](http://i3.ytimg.com/vi/aju_-6ZP7Uo/hqdefault.jpg)](https://youtu.be/aju_-6ZP7Uo "Rasterizer")

[![Rasterizer](http://i3.ytimg.com/vi/llORYHJkOj0/hqdefault.jpg)](https://youtu.be/llORYHJkOj0 "Lightmaps")

[![Liquid volumes](http://i3.ytimg.com/vi/JdtLIGsecaQ/hqdefault.jpg)](https://youtu.be/JdtLIGsecaQ "Liquid volumes")

[![OBJ/Billboards](http://i3.ytimg.com/vi/Txr7tlYJNVQ/hqdefault.jpg)](https://youtu.be/Txr7tlYJNVQ "OBJ/Billboards")

[![OBJ/Billboards](http://i3.ytimg.com/vi/dpvQxOO04F4/hqdefault.jpg)](https://youtu.be/dpvQxOO04F4 "FPS features")
