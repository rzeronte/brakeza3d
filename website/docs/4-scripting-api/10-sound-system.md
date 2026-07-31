---
sidebar_position: 11
title: Sound system
description: Sound system API in Brakeza3D for playing music and sound effects with volume control.
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

**Brakeza3D** manages sound through the SDL_mixer library, which supports **32 audio channels**.

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

## Per-Channel Control
---

Advanced control over individual audio channels. These methods are essential for implementing spatial audio, vehicle engine pitch variation, and channel management in a SoundManager.

### Channel Frequency (Pitch)

Set the playback frequency of a specific channel. Useful for simulating engine RPM changes or Doppler effect.

```lua
    -- Range: 11025 (slow) to 44100 (normal) to 88200 (fast)
    -- Default SDL_mixer frequency is 44100
    local speed = obj:getVelocity():length()
    local freq = 22050 + (speed / maxSpeed) * 22050
    Components:Sound():setChannelFrequency(channel, freq)
```

### Channel Volume

Control the volume of a single channel independently (unlike `setSoundsVolume` which affects all channels globally).

```lua
    -- Range [0 - 128]
    local dist = (cameraPos - objPos):length()
    local vol = math.max(0, 128 - dist * 0.5)
    Components:Sound():setChannelVolume(channel, vol)
```

### Channel Position (3D Spatialization)

Set the simulated 3D position of a sound source relative to the listener, using angle and distance.

```lua
    -- angle: 0-360 (0=front, 90=right, 180=behind)
    -- distance: 0-255 (0=at listener, 255=faint)
    Components:Sound():setChannelPosition(channel, 45, 50)
```

### Query Channel State

Check if a specific channel is currently playing a sound. Useful for channel allocation and stealing logic.

```lua
    if Components:Sound():isChannelPlaying(channel) then
        -- Channel is busy, decide to steal or skip
    else
        -- Channel is free, safe to use
    end
```

### Query Sound State by Label

Check if a sound loaded by label is currently playing on any channel.

```lua
    if Components:Sound():isSoundPlaying("engine_idle") then
        -- Sound is already playing, skip
    else
        Components:Sound():PlaySound("engine_idle", 2, -1)
    end
```

## Ambience Volume
---

`setAmbienceVolume` controls a global multiplier applied to all positional `Sound3D` sources
(0 = silence, 100 = full volume). It does not affect music or flat sound effects.

```lua
    -- Duck world sounds during a cutscene
    Components:Sound():setAmbienceVolume(0)

    -- Restore when gameplay resumes
    Components:Sound():setAmbienceVolume(100)
```

See **[Sound3D — Global Ambience Volume](./17-sound3d#global-ambience-volume)** for the full
interaction with `baseVolume` and distance attenuation.

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

---

## Positional audio
---

For sounds that should change volume based on the camera's distance to a point in the world,
see **[Sound3D](./17-sound3d)** — a dedicated scene object that handles distance attenuation
automatically and supports zone-based ambient crossfade.