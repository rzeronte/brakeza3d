# Brakeza3D — Debian packaging

## Archivos en debian/

| Archivo | Propósito |
|---|---|
| `control` | Metadatos del paquete + dependencias de build y runtime |
| `rules` | Instala un binario ya compilado (`cmake-build-release-wsl/Brakeza3D`) + assets en `/opt/brakeza3d` |
| `changelog` | Versión del paquete: la primera entrada debe coincidir con `ENGINE_VERSION` de `include/Config.h` |
| `copyright` | Licencia GPLv3 |

## Layout de instalación

```
/opt/brakeza3d/
  bin/Brakeza3D   <- ejecutable
  assets/         <- recursos
  GLSL/           <- shaders
  config/
/usr/bin/brakeza3d  <- wrapper script (cd + exec)
```

## Generar el .deb (Windows + WSL, recomendado)

1. Añadir una entrada en `debian/changelog` con la versión de la release (la misma que `ENGINE_VERSION`).
2. Compilar en CLion con el toolchain **WSL** (Release, carpeta `cmake-build-release-wsl`) con `master` activa.
3. Desde la raíz del repo, en PowerShell:

   ```powershell
   .\tools\package_linux.ps1
   ```

   Comprueba versión y fecha del binario, prepara un árbol limpio en WSL con `git archive master` (solo
   ficheros con seguimiento, finales de línea LF), lanza `dpkg-buildpackage -us -uc -b` y deja
   `..\Brakeza3D-x64_86-Linux-installer.deb` (el nombre al que enlaza la web).

WSL2 necesita la virtualización activada en la BIOS (Intel VT-x / AMD SVM) y la característica de Windows
"Plataforma de máquina virtual".

## Dependencias en WSL / Linux

```bash
sudo apt install dpkg-dev debhelper cmake build-essential \
  libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev \
  libbullet-dev libassimp-dev liblua5.2-dev \
  libgl1-mesa-dev libglu1-mesa-dev libglew-dev libcurl4-openssl-dev libglm-dev \
  libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev
```

El audio va con miniaudio (incluido en `third_party/`): ya no hace falta `libsdl2-mixer-dev`.

## A mano, en una máquina Linux

Con el binario ya compilado en `cmake-build-release-wsl/Brakeza3D`, desde la raíz del proyecto:

```bash
dpkg-buildpackage -us -uc -b
```

El flag `-us -uc` omite la firma GPG. El `.deb` queda un nivel arriba: `../brakeza3d_<versión>_amd64.deb`.
