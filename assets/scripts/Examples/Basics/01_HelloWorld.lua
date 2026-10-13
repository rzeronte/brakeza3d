-- ============================================================================
-- Tutorial 01: Hello World
-- Type: Global Script (runs for the whole scene)
-- ============================================================================
-- The smallest script: it draws text on the screen every frame.
--
-- A script has up to four functions the engine calls for you:
--   onStart()   once, when the scene starts
--   onUpdate()  every frame
--   onEnd()     when the script stops
-- Here onUpdate() writes a message, the run time and the FPS with a TextWriter.
-- ============================================================================

function onStart()
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf", 2)
    print("Hello World tutorial loaded!")
end

function onUpdate()
    local screenW = Components:Window():getWidth()
    local screenH = Components:Window():getHeight()
    local centerY = screenH / 2

    textWriter:writeTextTTFAutoSize(screenW/2 - 200, centerY, "Hello World from Brakeza3D!", Color.new(1, 1, 1, 1), 1.5)

    local time = Brakeza:getExecutionTime()
    textWriter:writeTextTTFAutoSize(screenW/2 - 100, centerY + 40, "Time: " .. math.floor(time) .. " sec", Color.new(0.8, 0.8, 0.8, 1), 1.0)

    local fps = Components:Render():getFps()
    textWriter:writeTextTTFAutoSize(screenW/2 - 50, centerY + 70, "FPS: " .. fps, Color.new(0, 1, 0, 1), 1.0)
end

function onEnd()
end
