# Brakeza3D — Debian packaging

## Archivos en debian/

| Archivo | Propósito |
|---|---|
| `control` | Metadatos del paquete + dependencias de build y runtime |
| `rules` | Script de construcción (llama a CMake + instala) |
| `changelog` | Requerido por dpkg-buildpackage |
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

## Para compilar en Linux

```bash
# 1. Instalar dependencias de build
sudo apt install debhelper cmake build-essential \
  libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev \
  libbullet-dev libassimp-dev liblua5.2-dev \
  libgl1-mesa-dev libglew-dev libcurl4-openssl-dev libglm-dev \
  libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libswresample-dev

# 2. Desde la raiz del proyecto
dpkg-buildpackage -us -uc -b
```

El flag `-us -uc` omite la firma GPG (util si no tienes clave configurada).
El `.deb` quedara un nivel arriba: `../brakeza3d_0.26.4-1_amd64.deb`.
