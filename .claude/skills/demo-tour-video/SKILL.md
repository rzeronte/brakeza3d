---
name: demo-tour-video
description: |
  Grabar el vídeo del tour de demos de Brakeza3D (TutorialsProject): el juego se maneja solo (teclas y ratón reales),
  se graba la ventana con ffmpeg y se queman subtítulos en inglés. Incluye tramos con el editor (GUI) visible y una zona
  final que enseña sus ventanas. Úsala cuando el usuario pida "grabar el vídeo/tour de las demos", "regrabar el vídeo",
  "mejorar/añadir una demo al vídeo", "subtítulos del vídeo" o "versión definitiva a 60 fps".
---

# Vídeo del tour de demos

Herramientas (todas en `tools/`):

| Fichero | Qué hace |
|---|---|
| `gen_demo_tour_plan.py` | Genera `demo_tour_plan.json`: por demo, qué teclas/ratón se pulsan (`steps`) y los subtítulos (`subs`). **Es lo que se edita** para cambiar el vídeo. Comprueba que los subtítulos no se solapen. |
| `demo_tour_plan.json` | Salida del anterior (se commitea). No editar a mano. |
| `record_demo_tour.ps1` | Lanza el juego, graba con ffmpeg, ejecuta el plan, quema subtítulos y une los tramos. |
| `assets/scripts/Browser/DemoBrowserTour.lua` | Recorre las demos; en modo grabación (`--set demoTourRecord=true`) espera a que el PS confirme cada una (`logs/demo_tour_ack.txt`). `demoTourTourOnly=true` = solo las 10 del tour. |

## Uso

```
python tools/gen_demo_tour_plan.py                      # tras tocar el guion
powershell -ExecutionPolicy Bypass -File C:\ruta\tools\record_demo_tour.ps1 -Out C:\ruta\logs\demo_tour.mp4
  -Only 21,24        # solo esas demos (pruebas rápidas; las demás se saltan)
  -AllDemos          # las 35 del catálogo en vez de las 10 del tour
  -Fps 60            # definitivo (los borradores a 30)
  -Safe              # definitivo a prueba de memoria/CPU, más lento: implica -Fps 60 -NoVsync -Light (ver abajo)
```

- **Rutas absolutas** al lanzarlo (el cwd puede no ser el repo) y `Set-Location` al repo antes.
- Lanzarlo con `run_in_background`; tarda ~7 min el tour completo. El script usa el **teclado y el ratón reales**: el usuario no debe tocar nada y la ventana no puede quedar tapada (un editor delante sale en el vídeo; ya ocurrió).
- No hace falta compilar para cambiar guion/Lua; sí si se tocó C++ (avisar siempre).
- Borradores a 30 fps; el definitivo, cuando el usuario diga "definitivo", a **60 fps** (ver memoria `feedback-demo-video-fps`). Con `-Fps 60` comprobar que el juego va a 60.

## Modo `-Safe` (2026-10-19)

`record_demo_tour.ps1 -Safe` = 60 fps garantizados sin que el PC se quede sin memoria/CPU, a costa de tiempo:
- **Captura en MJPEG a `.mkv`** (solo intra, sin lookahead ni buffers de x264 mientras corre el juego), cola de entrada de 1024 y ffmpeg de captura en prioridad AboveNormal. Ocupa ~6 MB/s (un tour completo, unos GB en `logs/`).
- **Montaje de un segmento cada vez**, BelowNormal, hilos limitados, preset `medium` con `rc-lookahead=20:ref=3:sync-lookahead=0`; el bruto se borra tras cada codificación y si ffmpeg falla se conserva y se aborta.
- **Verificación**: con ffprobe cuenta paquetes reales por segundo de cada segmento; si baja del 95 % del objetivo avisa al final (la captura perdió fotogramas; el vídeo solo los repite).
Uso: `-Safe` solo (60 fps) o `-Safe -Fps 30` para un borrador seguro. El resto (recortar la cola, no tocar el equipo) sigue igual. Probado solo el montaje con un clip sintético (240 paquetes en 4 s = 60 fps); la captura gdigrab MJPEG, no.

## Actualización 2026-10-19 (leer antes que lo de abajo)

- **Las coordenadas en píxeles de más abajo están obsoletas**: el editor cambió. Las vigentes están en `tools/gen_demo_tour_plan.py` (medidas sobre capturas reales). Antes de tocar un paso con clics, medirlo con `tools/gui_probe.ps1` (ver su cabecera): lanza el juego como hijo, ejecuta órdenes de un `cmd.txt` y saca capturas solo de la ventana del juego.
- **Bug del grabador (corregido):** `[Math]::Max(0, $t)` truncaba los tiempos de los eventos a segundos enteros; clics perdidos, punteros a saltos. Usar `Max(0.0, $t)`. `-MoveSpeed` (1,7 por defecto) acelera el puntero.
- **Partes sueltas:** grabar cada demo con `-Only N -Out logs/parts/demo_tour_NN.mp4` (bucle en PowerShell) permite revisar y regrabar una sola.
- Reglas de GUI y trampas nuevas: ver la memoria `demo-tour-video-2026-10-19` (Undo solo con scripts parados, Play duplica la escena en el navegador, cerrar pestaña de nodos deja ventana vacía, X rojo = quitar shader, etc.).

## Cómo funciona (para depurar)

- **Ventana**: 1280x720 en el tour; con el editor (`RCtrl+F5`) se **maximiza** (1920x1009). Cada tamaño es un *segmento* de grabación (gdigrab de la región de la ventana); al final cada uno se escala a 1280x720 dentro de un lienzo **1280x800** (franja inferior de 80 px solo para subtítulos, así nunca tapan el HUD de la demo) y se unen con concat.
- **Pasos del plan** (`steps`, `t` = segundos desde que la demo está lista): `{"key":"W","hold":0.8}`, combinaciones `"RCTRL+F7"`, `{"click":[fx,fy],"button":"left"}`, `{"move":[x0,y0,x1,y1],"dur":2}`, `{"drag":[x0,y0,x1,y1],"button":"left","dur":1}`, `{"key":"LCTRL+click","pos":[fx,fy]}`, `{"wheel":-4,"at":[fx,fy]}` (n muescas de rueda, + arriba), `{"editor":true|false}` (alterna el editor y parte el segmento; el reloj del plan se para durante el cambio). Coordenadas = **fracciones de la ventana**.
- **Ratón humano**: siempre se desliza hasta el destino (curva, aceleración/frenado suaves, temblor) y hace una pausa antes de pulsar; las teclas tienen holds algo irregulares.
- **Subtítulos**: se generan en ASS desde los eventos `sub` (instante real de grabación), por segmento. `SubLag` (0,6 s) compensa el arranque de ffmpeg.
- Teclas del editor (con Right Ctrl): F5 GUI on/off, F6/F7/F8 layouts, F9 Scene Objects, F10 Object Properties, F11 Project Setup, F12 File Browser, F2 Logs. Menú `Windows` (x≈1200 px de 1920) lista todas las ventanas.

## Coordenadas del editor (ventana 1920x1009; layout por defecto)

Menú Windows: clic en (1143,12); entradas a x=1200: Scene Objects 120, Object Properties 144, Project Setup 168, File Browser 192, Object Shaders 216, Object Scripts 240, Object Variables 264, Global Variables 288, Keyboard/Mouse 312, Media Browser 336, Logs 360, Lights DepthMaps 384, **Profiler 408**, **Code/Nodes editor 432**, GUI Icons 456, Threads 480. Panel de proyecto: nodo "Scene Shaders" en (1555,340) y, desplegado, los puntos verdes de cada shader en x=1729, y=404,428,452... Navegador de objetos (izquierda): flechas de grupo a x=26 (y=235 Mesh3D, 260 Light Point, 285 Light Spot; expandir de abajo a arriba) y casillas de activar a x=43.
**Editar un shader de nodos / un script** (demo 20, panel de proyecto, ventana 1920x1009): nodo "Scene Shaders" (1555,340) → icono de editar de la fila (x=1757; Sepia y=404, Duotone y=428...) abre el editor de nodos flotante; icono de ampliar (560,299); clic en un hueco del lienzo (900,500) y **tecla F = "navigate to content"**: centra el grafo entero. Cerrar: aspa roja (1684,158). Scripts: nodo "Scene Scripts" (1529,308) → lápiz de la fila (1796,371): abre el editor de código y **cambia solo al layout "coding"**; cerrar con el aspa (1525,198) y volver con `RCtrl+F6` antes de apagar el editor (si no, la siguiente vez el editor sale en layout de código).
Si el layout cambia, **sacar capturas con PrintWindow del juego** (no del escritorio) y recalcular.

## Trampas aprendidas

- **Menú Windows con cuidado (2026-10-19)**: no abrir por Windows una ventana que el layout/modo actual ya muestra, ni encender/apagar ventanas por parejas, ni abrir File Browser "para cerrarlo". Primero fijar el estado (Layouts > Default + Reset to default) y mostrar lo docked tal cual; Windows solo para lo que el layout no trae (Profiler, UI Manager, editores de código/nodos), cerrándolo limpio después.

- Capturar **solo la ventana del juego** al explorar la GUI (PrintWindow), nunca el escritorio: lleva datos privados del usuario.
- `-Only` con varios números llega como UN string con `-File`: se parsea aparte (`$OnlyList`; una variable `[string]` no admite asignarle un array).
- El ack del tour compara por igualdad (`== target`): con el orden del tour (17, 21, 18...) un `>=` hacía pasar demos sin esperar.
- Parar la grabación en `ready list`, antes de que el juego se cierre, o salen las ventanas del escritorio.
- Los subtítulos solapados se ven "duplicados": el generador lo rechaza.
- Demos con texto propio abajo/derecha (twin-stick, proyectiles) chocaban con la barra/panel del navegador: el HUD de las demos debe dejar libres los 70 px inferiores y el panel de la derecha.
- `camera:LookAt(objeto)` deja la vista vacía en cámaras: usar `setRotationFromEulerAngles(-pitchArriba, yaw, 0)` (yaw 0 mira a -Z).
- La interfaz del `DemoBrowser` escala con la altura de la ventana (`applyScale`, diseño 1009 px).
- ImGui: en menús el puntero debe **descansar** sobre el elemento antes de pulsar (`"pre": 0.7` en el paso `click`) y entre el clic del botón del menú y el del elemento hay que dejar ≥ 2 s (si no, el deslizamiento hacia el elemento arranca antes de soltar el botón y el menú no se abre).
- Una segunda selección en la misma lista del menú Windows cayó en la fila vecina en las pruebas (Profiler ↔ Code/Nodes): en la zona final solo se abre el Profiler desde esa lista.
- `RCtrl+F6/F7/F8` y `RCtrl+F9..F12/F2` no respondían en la grabación aunque sí a mano. Causa real: si el bucle del script se retrasaba, el modificador y la tecla llegaban en el mismo milisegundo y el motor solo ve el modificador en el fotograma SIGUIENTE. Ahora cada `kd` se mantiene pulsado en tiempo real (140 ms los modificadores, 90 ms el resto) antes de seguir. (Además el toque de Alt de `Focus-Game` solo se hace si el juego no está en primer plano, porque pone ImGui en modo de navegación.) Los layouts no cambian con una ventana flotante abierta (profiler...): ponerlos antes.
- Un clic derecho en la vista del juego deja un popup de ImGui abierto que reaparece al abrir el editor: no usar botón derecho en demos que luego abren el editor.
- **Seguridad de la entrada**: el script solo envía teclas/ratón mientras la ventana del juego es la de primer plano (si otra app la tapa, recupera el foco y retiene la entrada) y deja de enviar si el juego se cierra. Antes, con el juego ya cerrado, los clics del guion podían caer en el editor de código del usuario.
- Si el usuario cierra o tapa la ventana del juego mientras graba, el tramo sale mal o la captura falla (0x0): avisarle antes.
- **Nunca enviar un "soltar botón" sin su "pulsar"**: un soltar-derecho suelto sobre una ventana de ImGui (editor de código, pestañas) abre su menú contextual y se queda abierto en el vídeo. El grabador lleva el estado de los botones y solo suelta los que pulsó.
- El log del juego crece mucho: el grabador lee solo su final (`Get-Content -Tail 400`) para detectar `ready N`; y en modo grabación el juego espera 9 s en la lista final (`DemoBrowserTour.lua`) para que el grabador pare el último tramo antes de que se cierre (si no sale tu escritorio al final).
- **El estado de los árboles del editor persiste entre demos** (p. ej. "Scene Shaders" abierto en la 19 sigue abierto en la 20): un clic de "abrir/cerrar" invierte el estado y los clics siguientes caen en otra fila. Un clic mal puesto llegó al icono **Save scene** (salió el modal "This will overwrite the saved scene file"; nunca se pulsó OK, pero cuidado: comprobar `git status assets/scenes` tras las pruebas). Cada demo debe dejar los árboles como los encontró (la 19 cierra "Scene Shaders" al terminar).
- **Layouts y ventanas por menú, no por atajo**: `RCtrl+F6..F12` no tomaron efecto en las grabaciones (en el log del juego se cargan los layouts, pero la GUI no cambia a la vista). Menú Layouts (clic 890,12): Default y=72, Coding 96, Design 120 (x=910). Menú Windows: Scene Objects y=120, Object Properties 144, Logs 360 (x=1200), Profiler 408. Hay que dejar ~2,5 s entre el botón del menú y el elemento y descansar ~1 s sobre él.
- **Profiler (menú Windows > Profiler, ventana flotante)**: pestañas Overview (606,415), Render (672), Scripts (793), Resources (868), Physics (943); casilla "Enable GPU timing" en Render (583,493), "Enable script timing" en Scripts (583,454); cierre aspa (1351,216). Con la demo 19 (8 shaders) se ve cómo sube/baja la fila Post-Processing al pulsar 0 / 9.
- **Inspector y deshacer** (demo 20, layout Default): flecha "Mesh 3D" (26,235), objeto Lava (130,426), cabecera Transformations (95,624), campo Position Y (285,684) se arrastra para mover; menú Edit (174,12): Undo (160,45), Redo (160,66).
- **Orden del tour** (TOUR_ORDER en `DemoBrowserTour.lua` y "tour" en `demos.json`): 17,21,18,19,20,22,23,24,13,15,25,26,27,33,34,29,30,31,32,35,28 (la 28 siempre al final: lleva la zona final del editor).
- **Definitivo = `-Fps 60 -NoVsync`** (el motor con `--no-vsync`: con vsync iba a 24-35 fps y el vídeo a 60 solo repetía fotogramas). Con eso salieron ~40-58 fotogramas distintos por cada 60 (mpdecimate). Al final del vídeo la ventana del juego se cierra con una transición que graba lo que haya detrás (el escritorio del usuario): **recortar la cola** (en la última grabación sobraba desde ~0,3 s después de "And that's the tour") con `ffmpeg -i f.mp4 -t <seg> -c:v libx264 -crf 18 ...` y revisar fotogramas finales.
- **`-Light`** (2026-10-15): captura con `-preset ultrafast`, montaje final a prioridad BelowNormal, `-preset veryfast` y la mitad de los hilos (`-Threads N` para fijarlos). Evita que el PC se cuelgue al montar el vídeo a 60 fps. Uso: `-Fps 60 -NoVsync -Light`.
- **Seleccionar un objeto en el editor: clic en el visor, no en el navegador de objetos.** La demo 20 deja un nodo de escena fantasma ("Shader Showcase") en el navegador y desplaza todas las filas; en la demo 13 se selecciona al personaje (en idle, centro del visor) con un clic en la vista. Inspector de Mesh3DAnimation (1920x1009): cabecera "Mesh3D Animation" (100,810), rueda -6 en (200,700) lo baja; lista de clips con rueda -3 en (200,690) → WALKING (100,743), RUNNING (100,722); rueda +3 → CROUCH_WALKING (100,625). **Speed está limitado a 0..1** (`Mesh3DGUIAnimation.cpp`): arrastrar 60 px a la izquierda = 0,4.
