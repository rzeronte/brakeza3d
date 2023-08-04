# Windows compilation 

We are gonna to compile 64bits version of Brakeza3D on Windows using CLion like IDE and MinGW like toolchain for compiling. We will have to download some tools and libraries:

- CLion: IDE
- MinGW: toolchain + compilator
- SDL2: CrossPlatform Basics
- SDL2_image: CrossPlatform image manipulation
- SDL2_ttf: CrossPlatform TTF manipulation
- SDL2_mixer: CrossPlatform sound mixer
- Assimp: CrossPlatform interface to 3D files
- Bullet Physics: CrossPlatform physic’s engine
- GLM: CrossPlatform math’s library
- SDK OpenCL: CrossPlatform GPU generic interface access

---

### CLion
- Download: https://www.jetbrains.com/clion/download/download-thanks.html

### MinGW

- Source: https://winlibs.com
- Download: https://github.com/brechtsanders/winlibs_mingw/releases/download/13.2.0-16.0.6-11.0.0-ucrt-r1/winlibs-x86_64-posix-seh-gcc-13.2.0-llvm-16.0.6-mingw-w64ucrt-11.0.0-r1.zip

![Download MinGW](https://github.com/rzeronte/brakeza3d/blob/master/doc/images/mingw_screenshot.png?raw=true)

### SDL2

- Source: https://github.com/libsdl-org/SDL/releases
- Download: https://github.com/libsdl-org/SDL/releases/download/release-2.28.2/SDL2-devel-2.28.2-mingw.zip

![Download SDL2](https://github.com/rzeronte/brakeza3d/blob/master/doc/images/sdl2_image_screenshot.png?raw=true)

### SDL2_image

- Source: https://github.com/libsdl-org/SDL_image/releases
- Download: https://github.com/libsdl-org/SDL_image/releases/download/release-2.6.3/SDL2_image-devel-2.6.3-mingw.zip

![Download SDL2_image](https://github.com/rzeronte/brakeza3d/blob/master/doc/images/sdl2_image_screenshot.png?raw=true)

### SDL2_ttf

- Source: https://www.libsdl.org/projects/SDL_ttf/release/
- Download: https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-2.20.1-win32-x86.zip

![Download SDL2_ttf](https://github.com/rzeronte/brakeza3d/blob/master/doc/images/sdl2_ttf_screenshot.png?raw=true)

### SDL2_mixer

- Source: https://www.libsdl.org/projects/SDL_mixer/release/
- Download: https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-devel-2.6.3-mingw.zip

![Download SDL2_mixer](https://github.com/rzeronte/brakeza3d/blob/master/doc/images/sdl2_mixer_screenshot.png?raw=true)

### Assimp

- Source: https://github.com/assimp/assimp/releases
- Download: https://github.com/assimp/assimp/archive/refs/tags/v5.2.5.zip

![Download Assimp](https://github.com/rzeronte/brakeza3d/blob/master/doc/images/assimp_screenshot.png?raw=true)

### Bullet Physics

- Source: https://github.com/bulletphysics/bullet3/releases
- Download: https://github.com/bulletphysics/bullet3/archive/refs/tags/3.25.zip

![Download Bullet Physics](https://github.com/rzeronte/brakeza3d/blob/master/doc/images/bullet_screenshot.png?raw=true)

### GLM

- Source: https://github.com/g-truc/glm
- Download: https://github.com/g-truc/glm/archive/refs/heads/master.zip

![Download GLM](https://github.com/rzeronte/brakeza3d/blob/master/doc/images/glm_screenshot.png?raw=true)


---

### Install CLion + MinGW

Not explanation is required for install Clion. Follow his installer instructions and go will get.
MinGW only required decompress the previously downloaded file.

Once MinGW had been decompressed, you need setup CMake toolchain in CLion

File > Setting > Build, Execution, Deployment > Toolchain. 

In this option you can select the MinGW previously decompressed folder like this:

---
### Notes

We will only need to compile for ourselves two things. 

- Bullet
- Assimp

Once had been decompressed into his individual folder, we can open each of them and compile them individually. This may take a minutes.

Rest of libraries (SDL2, SDL2_image, SDL2_ttf, SDL2_mixer, GLM and SDK OpenCL) do not require to be compiled.

GLM is a header library, so that it not have DLL associated. Only development files will be needed.

---
We are interested in two things about each of these projects:

- lib and bin files (.ddl / .a): Files used for compiling and linking. We will move this files into a folder inside MinGW scope.
- includes (.h, .hpp...): Used for development includes. We will make a folder with all of them to keep easy maintainable

---
### Move DLLs (lib and bin folder's content to MinGW folder)

We will be able to find a LIB and a BIN folder inside our MinGW toolchain.

In each of the libraries you will also find these folders, so that we will go copying this files inside MinGW folders.

### Move development header files and update CMakeLists

Now we are gonna to join in a folder all headers needed for development. So we could create a 'include' empty folder.

Same way with lib/bin, you will find a 'include' folder inside each library project. We will go copying to newly 'include' folder.

---

At last, you will have to update CMakeLists.txt folders route, with newly 'include' folder.

---

Now you can try to compile Brakeza3D in Windows!