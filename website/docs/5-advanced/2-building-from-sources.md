---
title: 5.2) Building from sources
---

## Linux compilation

In Unix systems we will only have to make sure that we have installed the development packages of the libraries that we need

```bash
apt-get install libsdl2-2.0-0 libsdl2-dev libsdl2-image-dev libsdl2-image-2.0-0 libsdl2-ttf-dev \
libsdl2-ttf-2.0-0 libsdl2-mixer-dev libsdl2-mixer-2.0-0 libbullet-dev libbullet3.06 libassimp-dev \
libassimp5 assimp-utils libglm-dev liblua5.2-dev \
liblua5.2-dev liblua5.2-0
```

```bash
git clone https://github.com/rzeronte/brakeza3d.git brakeza3D
cd brakeza3D
mkdir build && cd build
cmake ..
make
```
---

## Windows compilation

With the goal of providing a ready-to-use Windows build environment, we have created a package containing all the required libraries precompiled.

https://github.com/rzeronte/brakeza-mingw-precompiled-libraries

---

## OSX compilation

Currently, I do not compile for Mac, but the project was started on it, so that it shouldn't vary much compared to compiling with Linux, once the dependencies are satisfied.

---