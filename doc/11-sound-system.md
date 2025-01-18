>[Back to index](https://github.com/rzeronte/brakeza3d/blob/master/doc/00-index.md)

- [Sistema de sonido](#sistema-de-sonido)
- [Formatos de sonido](#formatos-de-sonido)
- [Canales](#canales)
- [Carga de ficheros en memoria](#carga-de-ficheros-en-memoria)
- [Reproducior música](#reproducir-música)
- [Reproducir sonidos](#reproducir-sonidos)
- [Carga automática](#carga-automática)
  
---

# Sistema de sonido

Brakeza3D ofrece una interfaz básica para el manejo del sonido en tus juegos.

Podemos acceder a las funcionalidades de este sistema a través del componente ``ComponentSound``.

```lua
    systemSound = componentsManager:getComponentSound()
```

---

### Formatos de sonido

Podrás reproducir ficheros ``WAV`` y/o `MP3`. 

---

### Canales

Brakeza3D maneja el sonido através de la librería `SDL_mixer`, el cual soporta 16 canales de audio.

Si bien la gestión de estos canales es automática cuando trabajemos desde nuestros scripts, Brakeza3D asigna uno 
exclusivamente para la ``música``. El resto queda disponible para la emisión de `sonidos`.

Esta separación nos permitirá manejar los volumenes de la música y los sonidos por separado.

---
# Carga de ficheros en memoria

No es recomendable `cargar de disco`` un sonido en tiempo real justo en el momento que necesite ser reproducido, es por
este motivo por el que se recomienda pre-cargar los sonidos que posteriormente vayamos a utilizar.

El ``ComponenteSonido`` nos ofrece la posibilidad de cargar un sonido a memoria, el cual podrá ser reproducido tantas veces 
como sea necesario.

---
### Reproducir música

Precarga de un fichero destinado a reproducirse como `música`. La música se reproduce en búcle de forma
automática.

```lua
    systemSound = componentsManager:getComponentSound()
    systemSound:addMusic("../assets/sounds/music_demo.mp3", "music_demo")
```
---

Reproducir una música precargada:

```lua
    systemSound = componentsManager:getComponentSound()
    systemSound:playMusic("music_demo")
```
---
### Reproducir sonidos

Precarga de un fichero destinado a reproducirse como `sonido`:

```lua
    systemSound = componentsManager:getComponentSound()
    systemSound:addSound("../assets/sounds/music_demo.mp3", "sound_demo")
```

---

Reproducir una sonido precargado:

```lua
    systemSound = componentsManager:getComponentSound()
    systemSound:playSound("sound_demo")
```

---

### Volumen

Puedes manipular el volumen de la música y el resto de sonidos de forma separada.

Manipular el volumen de la música:

```lua
    systemSound = componentsManager:getComponentSound()
    systemSound:setMusicVolume(75) -- Rango [0 - 128]
```

Manipular el volumen de los sonidos:

```lua
    systemSound = componentsManager:getComponentSound()
    systemSound:setSoundsVolume(100) -- Rango [0 - 128]
```

---

### Carga automática

Brakeza3D ofrece un sistema de pre-carga inicial de sonidos y música a través de 
un fichero de configuración JSON.

Podrás encontrar este fichero en ``config/sounds.json`` .

````json
{
  "sounds": [
    { "file": "radio_beep", "label": "musicTest", "type": "music" },
    { "file": "radio_beep.wav", "label": "soundTest", "type": "sound" }
  ]
}
````

Automáticamente podrás acceder a los sonidos y músicas definidos en este fichero.