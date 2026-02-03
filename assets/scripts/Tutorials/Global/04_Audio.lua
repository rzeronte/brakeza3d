function onStart()
    musicPlaying = false
    musicVolume = 100
    soundsVolume = 100
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf", 2)
    print("Audio tutorial loaded!")
end

function onUpdate()
    local input = Components:Input()
    local sound = Components:Sound()

    -- Play sounds
    if input:isCharFirstEventDown("Q") then
        sound:PlaySound("beep", 0, 1)
        print("Playing: beep")
    end

    if input:isCharFirstEventDown("W") then
        sound:PlaySound("click", 1, 1)
        print("Playing: click")
    end

    if input:isCharFirstEventDown("E") then
        sound:PlaySound("coin", 2, 1)
        print("Playing: coin")
    end

    if input:isCharFirstEventDown("R") then
        sound:PlaySound("hit", 3, 1)
        print("Playing: hit")
    end

    if input:isCharFirstEventDown("T") then
        sound:PlaySound("success", 4, 1)
        print("Playing: success")
    end

    -- Toggle music
    if input:isCharFirstEventDown("M") then
        if not musicPlaying then
            sound:PlayMusic("bgmusic")
            musicPlaying = true
            print("Music started")
        else
            sound:StopMusic()
            musicPlaying = false
            print("Music stopped")
        end
    end

    -- Music volume (I/K)
    if input:isCharPressed("I") then
        musicVolume = math.min(128, musicVolume + 1)
        sound:setMusicVolume(musicVolume)
    end

    if input:isCharPressed("K") then
        musicVolume = math.max(0, musicVolume - 1)
        sound:setMusicVolume(musicVolume)
    end

    -- Sounds volume (U/J)
    if input:isCharPressed("U") then
        soundsVolume = math.min(128, soundsVolume + 1)
        sound:setSoundsVolume(soundsVolume)
    end

    if input:isCharPressed("J") then
        soundsVolume = math.max(0, soundsVolume - 1)
        sound:setSoundsVolume(soundsVolume)
    end

    -- RENDER INFO
    local screenW = Components:Window():getWidth()
    local screenH = Components:Window():getHeight()
    local centerY = 100

    textWriter:writeTextTTFAutoSize(screenW/2 - 120, centerY, "=== AUDIO TUTORIAL ===", Color.new(1, 1, 0, 1), 1.2)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 40, "Q/W/E/R/T = Play sounds", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 70, "M = Toggle music on/off", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 110, "I/K = Music volume up/down", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 140, "U/J = Sounds volume up/down", Color.new(0.7, 0.7, 0.7, 1), 1.0)

    textWriter:writeTextTTFAutoSize(screenW/2 - 120, centerY + 190, "Music Vol: " .. musicVolume .. "/128", Color.new(0, 1, 0, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 120, centerY + 220, "Sound Vol: " .. soundsVolume .. "/128", Color.new(0, 1, 0, 1), 1.0)

    local musicStatus = "OFF"
    if musicPlaying then musicStatus = "ON" end
    textWriter:writeTextTTFAutoSize(screenW/2 - 80, centerY + 260, "Music: " .. musicStatus, Color.new(0, 0.8, 1, 1), 1.0)
end

function onEnd()
    Components:Sound():StopMusic()
end
