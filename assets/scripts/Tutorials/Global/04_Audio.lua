function onStart()
    musicPlaying = false
    currentVolume = 100
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf", 2)
    print("Audio tutorial loaded!")
end

function onUpdate()
    local input = Components:Input()
    local sound = Components:Sound()

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

    if input:isCharPressed("U") then
        currentVolume = math.min(128, currentVolume + 1)
        sound:setMusicVolume(currentVolume)
        sound:setSoundsVolume(currentVolume)
    end

    if input:isCharPressed("J") then
        currentVolume = math.max(0, currentVolume - 1)
        sound:setMusicVolume(currentVolume)
        sound:setSoundsVolume(currentVolume)
    end

    -- RENDER INFO
    local screenW = Components:Window():getWidth()
    local screenH = Components:Window():getHeight()
    local centerY = 100

    textWriter:writeTextTTFAutoSize(screenW/2 - 120, centerY, "=== AUDIO TUTORIAL ===", Color.new(1, 1, 0, 1), 1.2)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 40, "Q = Play beep sound", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 70, "W = Play click sound", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 100, "E = Play coin sound", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 130, "R = Play hit sound", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 160, "T = Play success sound", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 190, "M = Toggle music on/off", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 220, "U/J = Volume up/down", Color.new(0.7, 0.7, 0.7, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 80, centerY + 260, "Volume: " .. currentVolume .. "/128", Color.new(0, 1, 0, 1), 1.0)

    local musicStatus = "OFF"
    if musicPlaying then musicStatus = "ON" end
    textWriter:writeTextTTFAutoSize(screenW/2 - 60, centerY + 290, "Music: " .. musicStatus, Color.new(0, 0.8, 1, 1), 1.0)
end

function onEnd()
    Components:Sound():StopMusic()
end
