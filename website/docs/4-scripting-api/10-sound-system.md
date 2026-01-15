---
sidebar_position: 11
title: 4.10) Sound system
---

# Sound system
---

**Brakeza3D** offers a basic interface for handling sound in your games. We can access
the functionalities of this system through ComponentSound.

```lua
    ...
    sound = Components:Sound()
    ...
```

## Sound Formats
---
You will be able to play **WAV** and/or **MP3** files.


## Channels
---

**Brakeza3D** manages sound through the SDL_mixer library, which supports **16 audio channels**.

Although the management of these channels is automatic when working from our scripts, Brakeza3D assigns one
exclusively for music. The rest remain available for sound playback.

This separation allows us to control the volume of music and sounds independently.


## Loading Files into Memory
---
It is not recommended to load a sound from disk in real time at the exact moment it needs to be played.
For this reason, it is recommended to pre-load the sounds that will be used later.

The component `Sound` offers the possibility to load a sound into memory, which can then be played as many times
as necessary.


## Playing Music
---

Preload a file intended to be played as music. Music will loop automatically.

```lua
    ...
    Components:Sound():AddMusic("../assets/sounds/music_demo.mp3", "music_demo")
    ...
```

Play a preloaded music track:

```lua
    ...
    Components:Sound():PlayMusic("music_demo")
    ...
```

## Playing Sounds
---

Preload a file intended to be played as a sound:

```lua
    ...
    Components:Sound():AddSound("../assets/sounds/music_demo.mp3", "sound_demo")
    ...
```

Play a preloaded sound:

```lua
    ...
    Components:Sound():PlaySound(
        "music_demo",               -- tag preloaded
        1,                          -- channel sound
        1                           -- times
    )
    ...
```


## Volume
---

You can manipulate the volume of music and other sounds separately.

Manipulate music volume:

```lua
    ...
    Components:Sound():setMusicVolume(75) -- Range [0 - 128]
    ...
```

Manipulate sound volume:

```lua
    ...
    Components:Sound():setSoundsVolume(100) -- Range [0 - 128]
    ...
```

## Automatic Loading
---

**Brakeza3D** offers an initial preloading system for sounds and music through a JSON configuration file.

You can find this file at config/sounds.json.

```json
{
  "sounds": [
    { "file": "radio_beep", "label": "musicTest", "type": "music" },
    { "file": "radio_beep.wav", "label": "soundTest", "type": "sound" }
  ]
}
```

You can automatically access the sounds and music defined in this file by label.