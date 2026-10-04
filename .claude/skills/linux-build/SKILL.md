---
name: linux-build
description: |
  Hacer la build de Linux de Brakeza3D (paquete .deb Brakeza3D-x64_86-Linux-installer.deb), siempre desde
  master, con WSL (Ubuntu) y tools/package_linux.ps1. Úsala cuando el usuario pida "hacer la build de Linux",
  "generar el .deb" o preparar una release.
---

# Build de Linux (.deb)

Igual que la de Windows (skill `windows-build`), **siempre sale de `master`**. Se hace desde Windows con WSL:
el binario se compila en CLion con el toolchain WSL y `tools/package_linux.ps1` monta el `.deb` dentro de WSL
con `debian/` (ver `debian/BUILD.md`).

## Requisitos

- WSL2 con la distro **Ubuntu**. WSL2 necesita la **virtualización activada en la BIOS** (Intel VT-x / AMD SVM)
  y la característica de Windows "Plataforma de máquina virtual". Síntoma si falta:
  `HCS_E_HYPERV_NOT_INSTALLED` / "Se habilitó la virtualización en el firmware: No" en `systeminfo`
  (detectado 2026-10-04: la virtualización estaba desactivada en la BIOS de este PC).
- En WSL: `dpkg-dev debhelper` y las dependencias de build de `debian/control` (lista en `debian/BUILD.md`).
  El script lo comprueba con `dpkg-checkbuilddeps`.

## Pasos

1. **Versión**: `ENGINE_VERSION` en `include/Config.h` de master y **primera entrada de `debian/changelog`**
   con la misma versión (de ahí sale la versión del `.deb`; el script falla si no coinciden).
2. **Compilar** (lo hace el usuario en CLion; nunca lanzar cmake): con `master` activa, perfil **Release con
   toolchain WSL**, carpeta `cmake-build-release-wsl` → `cmake-build-release-wsl/Brakeza3D`.
3. **Empaquetar**, desde la raíz del repo: `.\tools\package_linux.ps1`
4. Resultado: `..\Brakeza3D-x64_86-Linux-installer.deb` (nombre fijo: la web enlaza a
   `releases/download/<tag>/Brakeza3D-x64_86-Linux-installer.deb`). Subirlo a la release de GitHub junto al
   instalador de Windows y actualizar el tag en `website/src/components/DownloadCards.jsx`.

## Qué hace el script

1. Versión de `master:include/Config.h` y comprobación de `master:debian/changelog`.
2. El binario debe contener `v<versión>` y ser posterior al último commit del core de master (`-Force` lo salta).
3. Comprueba que WSL arranca y que están `dpkg-buildpackage`, `dh` y `dpkg-shlibdeps`.
4. Árbol limpio en WSL (`~/brakeza3d-deb/brakeza3d-<versión>`): `assets`, `GLSL`, `config` y `debian` con
   `git -c core.autocrlf=false archive master` (solo ficheros con seguimiento y con LF: `debian/rules` en CRLF
   rompe `make`) + el binario.
5. `dpkg-checkbuilddeps` y `dpkg-buildpackage -us -uc -b` (sin firma GPG). `dh_shlibdeps` calcula las
   dependencias de runtime a partir del binario.
6. Copia el `.deb` fuera, muestra `Package/Version/Depends` y comprueba el contenido (binario, wrapper
   `/usr/bin/brakeza3d`, assets, GLSL; nada del RTS ni `.bak`).

## Layout instalado

`/opt/brakeza3d/{bin/Brakeza3D, assets, GLSL, config}` + wrapper `/usr/bin/brakeza3d` (hace `cd` a `bin` y
ejecuta). Los paths de assets son relativos al ejecutable (`../assets`).
