---
name: windows-build
description: |
  Hacer la build (paquete de distribución) de Windows de Brakeza3D, siempre desde master: comprobar el exe,
  generar ..\build, el zip y el instalador Inno Setup (Brakeza3D-x64-86-Windows-installer.exe) con
  tools/package_windows.ps1. Úsala cuando el usuario pida "hacer la build de Windows", "empaquetar para
  Windows", "generar el instalador" o preparar una release.
---

# Build de Windows (paquete de distribución + instalador)

La build de Windows **siempre sale de `master`** (motor + demos genéricas, nada del RTS). El script
`tools/package_windows.ps1` sustituye al antiguo `..\copy_files.bat` (que copiaba las carpetas tal cual del
disco: arrastraba ficheros sin seguimiento del RTS y no vaciaba el paquete anterior) y al `.iss` del escritorio.

## Estructura fuera del repo (`C:\Users\darkh\CLionProjects\`)

| Ruta | Qué es |
|------|--------|
| `..\dlls\` | DLLs de runtime de Windows (SDL2, SDL2_image, SDL2_ttf, FFmpeg, glew, assimp, curl, lua, runtime MinGW) |
| `..\build\` | Carpeta del paquete (`bin\`, `assets\`, `GLSL\`, `config\`). Se regenera entera |
| `..\Brakeza3D-<versión>-windows.zip` | Zip del paquete |
| `..\Brakeza3D-x64-86-Windows-installer.exe` | **Instalador** (Inno Setup): es lo que se sube a la release de GitHub |
| `..\copy_files.bat`, `Desktop\InnoDBSetupBrakeza.iss` | Script y `.iss` antiguos, ya no se usan |

## Pasos

1. **Versión**: `ENGINE_VERSION` en `include/Config.h` de `master` debe ser la de la release (p. ej. `"v0.26.10"`).
   Si cambia, es un fichero del core: aplicar el mismo cambio en `rts-videogame` para que el core siga idéntico.
2. **Compilar** (lo hace el usuario en CLion; nunca lanzar cmake): con `master` activa, configuración
   **Release-MinGW-BrakezaBundle** → `cmake-build-release-mingw-brakezabundle\Brakeza3D.exe`.
3. **Empaquetar**, desde la raíz del repo:
   ```powershell
   .\tools\package_windows.ps1                 # vacía ..\build, genera paquete + zip + instalador
   .\tools\package_windows.ps1 -KeepPrevious   # renombra el ..\build anterior en vez de borrarlo
   .\tools\package_windows.ps1 -Strip          # quita símbolos al exe empaquetado (descarga más pequeña)
   .\tools\package_windows.ps1 -NoInstaller    # sin instalador (más rápido, ~30 s)
   ```
4. **Revisar** la salida: versión del exe OK, "All N imported DLLs are present", tamaños del paquete, zip e instalador.
5. **Probar** el exe de `..\build\bin\` con un PATH limpio (solo Windows) y un proyecto de demo antes de subir
   el instalador a la release de GitHub. Después, actualizar el tag en `website/src/components/DownloadCards.jsx`.

## Qué comprueba el script

- Lee la versión de `master:include/Config.h` y exige que `Brakeza3D.exe` contenga ese `vX.Y.Z` y sea **posterior
  al último commit del core** de master (`src`, `include`, `GLSL`, `CMakeLists.txt`, `third_party`, `resources`).
  Si falla: el exe es de otra rama o está sin recompilar. `-Force` lo salta (solo para pruebas).
- Exporta `assets\`, `GLSL\` y `config\` con `git archive master`: solo ficheros con seguimiento en master,
  aunque la rama activa sea otra (los assets sin seguimiento del RTS que quedan en disco no entran).
- Copia las DLLs de `..\dlls` excepto las obsoletas (`SDL2_mixer.dll`: el audio es miniaudio desde la 0.26.10).
- Verifica con `objdump -p` que toda DLL importada por el exe está en `bin\` o es de sistema.
- Zip con `tar.exe -a`.

## Instalador (Inno Setup)

- Definición versionada en el repo: `tools/installer/Brakeza3D.iss` (traída del escritorio). Conserva el
  `AppId` original, así el instalador nuevo actualiza una instalación anterior: **no cambiarlo**.
- El script lo compila con `ISCC.exe` (Inno Setup 6 en Program Files; `-Iscc <ruta>` si está en otro sitio)
  pasando `/DMyAppVersion=<versión>`, `/DSourceDir=..\build`, `/DOutputDir=..` y `/DRepoDir=<repo>`. El `.iss`
  se lee de la rama empaquetada.
- Nombre de fichero fijo (`Brakeza3D-x64-86-Windows-installer.exe`): la web enlaza a
  `releases/download/<tag>/Brakeza3D-x64-86-Windows-installer.exe`, así que en cada release solo cambia el tag.
- Icono: `resources/windows/application.ico` (el mismo que lleva incrustado el exe).
- Tarda ~2-3 min (compresión sólida).

## Notas

- El exe Release pesa ~300 MB porque lleva símbolos (permiten resolver crashes con `addr2line`). `-Strip` solo
  afecta a la copia empaquetada.
- Si se añade una dependencia nueva al motor, copiar su DLL a `..\dlls\`; si deja de usarse, añadirla a
  `$ObsoleteDlls` en el script. El chequeo de imports avisa si falta alguna.
- Al ejecutarse, el motor escribe `brakeza_events.jsonl` y `brakeza_state.json` en la carpeta superior al exe:
  si se prueba el exe dentro de `..\build`, borrarlos antes de volver a empaquetar (o regenerar con el script).
- Para probar el script sin tocar `..\build`: `-OutDir <carpeta temporal> -Force -NoInstaller`.
- Linux: pendiente de documentar (ver `debian/` en master).
