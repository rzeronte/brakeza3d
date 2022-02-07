# Brakeza3D - Free OpenSource 3D Engine

Brakeza3D is an open source 3D engine written from scratch with SDL2.0

No 3D graphics libraries such as OpenGL or Direct3D have been used.

Main features:

- Cross Platform (Windows/Linux/OSX checked)
- BSP Quake Geometry/Texture/Light maps/Entities loader
- Bullet Physics Integration
- CPU Triangle rasterizer
- Load models & animations with ASSIMP library
- 3D Billboard
- Billboard 3D 8 - directions
- Decals  
- UV Mapping
- Lights/Shadow real time
- Game Menu Interface
- Octrees
- Grid3D
- Basic component for add sounds
- Basic component for input management

# Dependences

- SDL2 (https://www.libsdl.org/download-2.0.php)
- SDL2_image (https://www.libsdl.org/projects/SDL_image/)
- SDL2_ttf (https://www.libsdl.org/projects/SDL_ttf/)
- SDL2_mixer (https://www.libsdl.org/projects/SDL_mixer/)
- BulletSoftBody (https://github.com/bulletphysics/bullet3)
- BulletDynamics 
- BulletCollision
- LinearMath (Included with Bullet)
- Assimp (https://github.com/assimp/assimp)

# Linux compilation

This project has been checked on Ubuntu 20.10 from CLI

In Unix systems we will only have to make sure that we have installed the development packages of the libraries that we need

``` 
apt-get install libsdl2-2.0-0 libsdl2-dev libsdl2-image-dev libsdl2-image-2.0-0 libsdl2-ttf-dev \
libsdl2-ttf-2.0-0 libsdl2-mixer-dev libsdl2-mixer-2.0-0 libbullet-dev libbullet2.88 libassimp-dev \
libassimp5 assimp-utils
```

``` 
git clone https://github.com/rzeronte/brakeza3d.git brakeza3D
cd brakeza3D
mkdir build && cd build
cmake ..
make
```

# Windows compilation

This project has been checked on Windows in a CLion/MinGW setup.

You can download MinGW compiler from https://sourceforge.net/projects/mingw-w64/

You can download MinGW precompiled libraries for SDL, SDL_image, SDL_ttf and SDL_mixer. When you have the files you will have to unzip them in the folder where MinGw is installed. Usually you must to copy a structure with three folders: 'lib', 'bin' and 'include'.

In addition you will also need to compile specific libraries for Assimp and Bullet Physics compiling for MinGW and when are ready, move this libraries to MinGW compiler folder.

Finally, we need check routes for libraries and includes in CMakeList.txt according to your installation routes.

# Mac compilation

Currently I do not compile for Mac, but the project was started on this system, it should not vary much compared to compiling with Linux, once the dependencies are satisfied.

# Brakeza3D - Videos

[![BSP Loader](http://i3.ytimg.com/vi/g_AP3GaFYQs/hqdefault.jpg)](https://youtu.be/g_AP3GaFYQs "BSP Loader")

[![Rasterizer](http://i3.ytimg.com/vi/aju_-6ZP7Uo/hqdefault.jpg)](https://youtu.be/aju_-6ZP7Uo "Rasterizer")

[![Rasterizer](http://i3.ytimg.com/vi/llORYHJkOj0/hqdefault.jpg)](https://youtu.be/llORYHJkOj0 "Lightmaps")

[![Liquid volumes](http://i3.ytimg.com/vi/JdtLIGsecaQ/hqdefault.jpg)](https://youtu.be/JdtLIGsecaQ "Liquid volumes")

[![OBJ/Billboards](http://i3.ytimg.com/vi/Txr7tlYJNVQ/hqdefault.jpg)](https://youtu.be/Txr7tlYJNVQ "OBJ/Billboards")

[![OBJ/Billboards](http://i3.ytimg.com/vi/dpvQxOO04F4/hqdefault.jpg)](https://youtu.be/dpvQxOO04F4 "FPS features")
