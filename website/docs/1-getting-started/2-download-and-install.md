---
title: Download and Install
description: How to download precompiled binaries or build Brakeza3D from source on Windows and Linux.
---

# Download and Install
---

You can install **Brakeza3D** either by downloading the precompiled binaries or by building it yourself from the source code available on GitHub.

## Precompiled Versions

Go to the [downloads](../../downloads) section to get a ready-to-use installer for Windows or Linux.

---

## Build from Source

The project has been successfully built using the [CLion IDE](https://www.jetbrains.com/clion/download/download-thanks.html) on both Linux and Windows. As a CMake-based project, it can be built from the command line or with any CMake-compatible IDE.


### Linux compilation

On Unix-based systems, you only need to ensure that the required development packages are installed.

```bash
sudo apt update && sudo apt install -y \
    build-essential cmake git \
    libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev \
    libbullet-dev libassimp-dev liblua5.2-dev \
    libgl1-mesa-dev libglu1-mesa-dev libglew-dev \
    libcurl4-openssl-dev \
    libglm-dev \
    libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev
```

Audio uses [miniaudio](https://miniaud.io), bundled with the sources: `libsdl2-mixer-dev` is no longer needed.

### Windows compilation

#### Download compiler and libraries

| Item                | Description | Link |
|---------------------|-------------|------|
| MinGW x86_64        | GCC 13.2.0 + LLVM 16.0.6, MinGW-w64 11.0.1 UCRT, MCF threads, release 2 (source [winlibs.com](https://winlibs.com)) | [MinGW x86_64 (winlibs r2 MCF)](https://github.com/brechtsanders/winlibs_mingw/releases/download/13.2.0mcf-16.0.6-11.0.1-ucrt-r2/winlibs-x86_64-mcf-seh-gcc-13.2.0-llvm-16.0.6-mingw-w64ucrt-11.0.1-r2.zip) |
| Brakeza3D libraries | SDL2, SDL2_image, SDL2_ttf, Bullet, Assimp, Lua 5.2, GLEW, cURL, FFmpeg and glm, built for that MinGW | [Windows libraries 0.26.10](https://github.com/rzeronte/brakeza3d/releases/download/0.26.10-windows-libs/brakeza3d-windows-libs-0.26.10.zip) |

:::note
Use exactly that MinGW build (MCF threads, UCRT): the libraries are compiled against it. Unzip both files in the
same folder: the libraries zip contains a `mingw64` folder that merges into the compiler's `mingw64`. Then select
that `mingw64` as the CLion toolchain.
:::

#### CLion Toolchain setup

![CLion Toolchain setup](/img/getting-started/clion-toolchain-demo.png)

#### CLion CMake setup

![CLion Cmake setup](/img/getting-started/clion-cmake-demo.png)
