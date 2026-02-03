function onStart()
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf", 2)
    pressedKeys = ""
    mouseInfo = ""
    gamepadInfo = ""
    Components:Input():setKeyboardEnabled(true)
    Components:Input():setMouseEnabled(true)
    Components:Input():setPadEnabled(true)
    print("Input tutorial loaded!")
end

function onUpdate()
    local input = Components:Input()

    -- KEYBOARD INPUT
    pressedKeys = "Keys: "
    if input:isCharPressed("W") then pressedKeys = pressedKeys .. "W " end
    if input:isCharPressed("A") then pressedKeys = pressedKeys .. "A " end
    if input:isCharPressed("S") then pressedKeys = pressedKeys .. "S " end
    if input:isCharPressed("D") then pressedKeys = pressedKeys .. "D " end
    if input:isCharPressed("SPACE") then pressedKeys = pressedKeys .. "SPACE " end
    if input:isCharFirstEventDown("E") then pressedKeys = pressedKeys .. "[E PRESSED!] " end

    -- MOUSE INPUT
    local mouseX = input:getRelativeRendererMouseX()
    local mouseY = input:getRelativeRendererMouseY()
    mouseInfo = "Mouse: X=" .. mouseX .. " Y=" .. mouseY
    if input:isLeftMouseButtonPressed() then mouseInfo = mouseInfo .. " | LEFT" end
    if input:isRightMouseButtonPressed() then mouseInfo = mouseInfo .. " | RIGHT" end

    -- GAMEPAD INPUT
    if input:isGameControllerAvailable() then
        local leftX = input:getControllerAxisLeftX()
        local leftY = input:getControllerAxisLeftY()
        gamepadInfo = "Gamepad: L(" .. math.floor(leftX*100)/100 .. "," .. math.floor(leftY*100)/100 .. ")"
        if input:getControllerButtonA() then gamepadInfo = gamepadInfo .. " A" end
        if input:getControllerButtonB() then gamepadInfo = gamepadInfo .. " B" end
    else
        gamepadInfo = "Gamepad: Not connected"
    end

    -- RENDER INFO
    local screenW = Components:Window():getWidth()
    local screenH = Components:Window():getHeight()
    local centerY = 100

    textWriter:writeTextTTFAutoSize(screenW/2 - 120, centerY, "=== INPUT TUTORIAL ===", Color.new(1, 1, 0, 1), 1.2)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 40, "W/A/S/D = Movement keys", Color.new(0.7, 0.7, 0.7, 1), 0.8)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 70, "SPACE = Jump key", Color.new(0.7, 0.7, 0.7, 1), 0.8)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 100, "E = Single press detection", Color.new(0.7, 0.7, 0.7, 1), 0.8)
    textWriter:writeTextTTFAutoSize(screenW/2 - 180, centerY + 130, "Mouse = Move and click", Color.new(0.7, 0.7, 0.7, 1), 0.8)
    textWriter:writeTextTTFAutoSize(screenW/2 - 80, centerY + 170, pressedKeys, Color.new(0, 1, 0, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 150, centerY + 200, mouseInfo, Color.new(0, 0.8, 1, 1), 1.0)
    textWriter:writeTextTTFAutoSize(screenW/2 - 150, centerY + 230, gamepadInfo, Color.new(1, 0.5, 0, 1), 1.0)
end

function onEnd()
end
