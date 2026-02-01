---
title: Download and Install
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
    libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev \
    libbullet-dev libassimp-dev liblua5.2-dev \
    libgl1-mesa-dev libglu1-mesa-dev libglew-dev \
    libcurl4-openssl-dev \
    libglm-dev
```

### Windows compilation

#### Download compiler and libraries

| Item                | Description                                                       | Link                                                                                                                                                                                      |
|---------------------|-------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| MinGW Download      | MinGW x86_64 compiler (Source [winlibs.com](https://winlibs.com)) | [MinGW x86_64](https://github.com/brechtsanders/winlibs_mingw/releases/download/13.2.0-16.0.6-11.0.0-ucrt-r1/winlibs-x86_64-posix-seh-gcc-13.2.0-llvm-16.0.6-mingw-w64ucrt-11.0.0-r1.zip) |
| Brakeza3D (Windows) | Precompiled Windows libraries ready to use                        | [Precompiled libraries](https://github.com/rzeronte/brakeza3d/releases/download/precompiled-libs/brakeza-win-x86_64-mingw.zip)                                                                                                                                                                |

:::note
Unzip `Precompiled libraries` into same directory of MinGW compiler
:::

#### CLion Toolchain setup

![CLion Toolchain setup](/img/getting-started/clion-toolchain-demo.png)

#### CLion CMake setup

![CLion Cmake setup](/img/getting-started/clion-cmake-demo.png)
