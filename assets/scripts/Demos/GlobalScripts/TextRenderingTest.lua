-- TextRenderingTest.lua
-- Demos ALL text rendering methods with toggle keys
--   0       toggle ALL on/off
--   1       toggle TTF AutoSize
--   2       toggle TTF explicit size
--   3       toggle Text Atlas (batched)
--   4       toggle Centered / Aligned
--   5       toggle Text Cache
--   6       toggle Legend
--   R       rebuild glyph atlas

tw        = nil
engineTw  = nil
input     = nil
win       = nil
render    = nil

-- toggles
local show     = true
local showAuto = true
local showSize = true
local showAtlas= true
local showCent = true
local showCache= true
local showLegend= true

-- colours per method
local C_HEAD     = Color.new(0.20, 0.85, 1.00, 1.0)
local C_AUTO     = Color.new(0.30, 1.00, 0.30, 1.0)
local C_SIZE     = Color.new(1.00, 0.60, 0.10, 1.0)
local C_ATLAS    = Color.new(0.40, 0.55, 1.00, 1.0)
local C_CENTER   = Color.new(1.00, 0.85, 0.00, 1.0)
local C_CACHE    = Color.new(1.00, 0.40, 0.80, 1.0)
local C_DIM      = Color.new(0.45, 0.45, 0.48, 1.0)
local C_WHITE    = Color.new(1.00, 1.00, 1.00, 1.0)
local C_GREY     = Color.new(0.70, 0.70, 0.70, 1.0)
local C_KEY      = Color.new(1.00, 0.90, 0.20, 1.0)
local C_LABEL    = Color.new(0.55, 0.55, 0.60, 1.0)

local SAMPLE     = "Hello Brakeza3D 0123"

-- cache tracking
local cacheBuilt = false

-- layout: zonas fijas que no se cruzan (pantalla ~1920x1010; a la derecha queda libre el panel de
-- información del DemoBrowser y abajo su barra de 56 px)
--   arriba izquierda : [1] AutoSize + [2] TTF size
--   arriba centro    : [3] Atlas
--   franja central   : [4] Centered (centra sobre la pantalla entera, por eso va sola)
--   abajo izquierda  : [5] Text Cache
--   abajo            : leyenda + FPS
local COL1_X    = 40
local COL2_X    = 760
local TOP_Y     = 30
local CHAR_W    = 20.5      -- ancho aprox. de un carácter de Courier a escala 1.0 (para la leyenda)

-- alto de línea para un texto TTF/atlas a escala s
local function lh(s) return math.floor(32 * s + 8) end

-- ── rebuild glyph atlas ───────────────────────────────────────────────────
local function rebuildAtlas()
    print("[TextRenderingTest] Rebuilding glyph atlas...")
    tw:buildGlyphAtlas(512)
    print("[TextRenderingTest] Glyph atlas rebuilt")
end

-- ── section helper ────────────────────────────────────────────────────────
local function drawSectionLabel(label, x, y, color)
    tw:writeTextTTFAutoSize(x, y, label, color, 0.60)
end

-- ── draw ──────────────────────────────────────────────────────────────────
function draw()
    if not show then return end

    local sw = win:getWidth();  if sw == 0 then sw = 1280 end
    local sh = win:getHeight(); if sh == 0 then sh = 720  end

    -- ── 1 + 2: columna izquierda, de arriba abajo ─────────────────────────
    local y = TOP_Y
    if showAuto then
        drawSectionLabel("[1] TTF AutoSize - writeTextTTFAutoSize", COL1_X, y, C_AUTO); y = y + lh(0.6)
        for _, sc in ipairs({ 0.80, 0.60, 0.45, 0.35, 0.25 }) do
            tw:writeTextTTFAutoSize(COL1_X + 10, y, SAMPLE .. string.format("  (scale %.2f)", sc), C_AUTO, sc)
            y = y + lh(sc)
        end
        y = y + 12
    end
    if showSize then
        drawSectionLabel("[2] TTF Size - writeTextTTF", COL1_X, y, C_SIZE); y = y + lh(0.6)
        for _, sz in ipairs({ { 300, 40 }, { 200, 28 }, { 150, 20 }, { 100, 14 } }) do
            tw:writeTextTTF(COL1_X + 10, y, sz[1], sz[2], SAMPLE, C_SIZE)
            y = y + sz[2] + 6
        end
    end

    -- ── 3: Text Atlas (columna central) ───────────────────────────────────
    if showAtlas then
        local ax, ay = COL2_X, TOP_Y
        render:DrawFilledRect(ax - 10, ay - 6, 600, 300, Color.new(0.05, 0.05, 0.30, 0.60))
        drawSectionLabel("[3] Text Atlas - writeTextAtlas (batched)", ax, ay, C_ATLAS)
        ay = ay + lh(0.6)

        local lines = {
            { "Hello Brakeza3D! HUD text with ascenders", 0.60 },
            { "FPS: 60  HP: 100  SCORE: 9999", 0.55 },
            { "Player hit by enemy: -25 HP", 0.50 },
            { "0123456789 !@#$%^&*() abcdefghijklmnopqrstuvwxyz", 0.40 },
            { "ABC DEF GHI JKL MNO PQR STU VW XYZ  (scale 0.35)", 0.35 },
            { "Mixed case with Caps: 0.25", 0.25 },
        }
        for _, l in ipairs(lines) do
            tw:writeTextAtlas(ax + 10, ay, l[1], C_ATLAS, l[2]); ay = ay + lh(l[2])
        end

        -- live counter to prove it updates every frame
        local execTime = string.format("%.1f", Brakeza:getExecutionTime())
        tw:writeTextAtlas(ax + 10, ay, "Engine time: " .. execTime .. "s (live)", Color.new(0.30, 1.00, 1.00, 1.0), 0.55)
        ay = ay + lh(0.55) + 10

        -- engine default glyph atlas (TroubleFont) - different visual style
        local engTw = render:getTextWriter()
        if engTw and engTw:getGlyphAtlas() and engTw:getGlyphAtlas():isBuilt() then
            local ga = engTw:getGlyphAtlas()
            engTw:writeTextAtlas(ax + 10, ay, "Engine TroubleFont atlas (GL tex " .. ga:getAtlasTexture() .. ")", Color.new(0.50, 0.80, 1.00, 1.0), 0.55)
            engTw:writeTextAtlas(ax + 10, ay + lh(0.55), "ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789", Color.new(0.50, 0.80, 1.00, 1.0), 0.50)
        else
            tw:writeTextAtlas(ax + 10, ay, "Engine glyph atlas NOT BUILT - check logs", Color.new(1.00, 0.30, 0.30, 1.0), 0.55)
        end
    end

    -- ── 4: Centered (franja central, sola) ─────────────────────────────────
    -- writeTextTTFMiddleScreen también existe (centro de la pantalla), pero centra con la resolución
    -- interna del motor, que Lua no conoce, y con escalas < 1 no centra en horizontal (TextWriter.cpp
    -- convierte la escala a int): no se usa aquí para no pisar las otras secciones.
    local my = 360
    if showCent then
        tw:writeTTFCenterHorizontal(my,      "[4] Centered - writeTTFCenterHorizontal", C_CENTER, 0.60)
        tw:writeTTFCenterHorizontal(my + 34, "CenterH - " .. SAMPLE .. " (0.60)", C_CENTER, 0.60)
        tw:writeTTFCenterHorizontal(my + 64, "CenterH - " .. SAMPLE .. " (0.50)", C_CENTER, 0.50)
        tw:writeTTFCenterHorizontal(my + 90, "CenterH - " .. SAMPLE .. " (0.40)", C_CENTER, 0.40)
    end

    -- ── 5: Text Cache (abajo a la izquierda) ───────────────────────────────
    local cy = my + 140
    if showCache then
        drawSectionLabel("[5] Text Cache - beginTextCache / endTextCache / drawTextCache", COL1_X, cy, C_CACHE)

        if not cacheBuilt then
            tw:beginTextCache("tc_header", 700, 30)
                tw:writeTextTTFAutoSize(0, 0, "CACHED HEADER - this text is baked into an FBO", C_CACHE, 0.55)
            tw:endTextCache()

            tw:beginTextCache("tc_body", 700, 140)
                local by = 0
                tw:writeTextTTFAutoSize(0, by, "Cached body line 1: persistent, no per-frame cost", C_CACHE, 0.50); by = by + 26
                tw:writeTextTTFAutoSize(0, by, "Cached body line 2: only the drawTextCache blit",  C_CACHE, 0.50); by = by + 26
                tw:writeTextTTFAutoSize(0, by, "Cached body line 3: happens each frame",            C_CACHE, 0.50); by = by + 26
                tw:writeTextTTFAutoSize(0, by, "Cached body line 4: content is static",             C_CACHE, 0.50); by = by + 26
                tw:writeTextTTFAutoSize(0, by, "Cached body line 5: only rebuilt if dirty flag",    C_CACHE, 0.50)
            tw:endTextCache()
            cacheBuilt = true
        end

        tw:drawTextCache("tc_header", COL1_X + 10, cy + lh(0.6))
        tw:drawTextCache("tc_body",   COL1_X + 10, cy + lh(0.6) + 36)
    end

    -- ── leyenda + FPS (encima de la barra del DemoBrowser) ────────────────
    local ly = sh - 100
    local function leg_color(on, c) if on then return c else return C_DIM end end
    if showLegend then
        local entries = {
            { "0", "toggle ALL",      leg_color(show, C_GREY) },
            { "1", "TTF AutoSize",    leg_color(showAuto, C_AUTO) },
            { "2", "TTF Size",        leg_color(showSize, C_SIZE) },
            { "3", "Atlas (batched)", leg_color(showAtlas, C_ATLAS) },
            { "4", "Center",          leg_color(showCent, C_CENTER) },
            { "5", "Cache",           leg_color(showCache, C_CACHE) },
            { "6", "Legend",          leg_color(showLegend, C_GREY) },
            { "R", "rebuild atlas",   C_GREY },
        }
        local s = 0.50
        local x = COL1_X
        for _, e in ipairs(entries) do
            tw:writeTextTTFAutoSize(x, ly, e[1], C_KEY, s)
            tw:writeTextTTFAutoSize(x + math.floor(2 * CHAR_W * s), ly, e[2], e[3], s)
            x = x + math.floor((#e[2] + 5) * CHAR_W * s)
        end
    end

    local fps = render:getFps()
    local fpsColor = fps >= 50 and Color.new(0.20, 1.00, 0.30, 1.0) or (fps >= 30 and Color.new(1.00, 0.80, 0.00, 1.0) or Color.new(1.00, 0.20, 0.20, 1.0))
    tw:writeTextAtlas(sw - 180, ly, "FPS: " .. fps, fpsColor, 0.60)
end

-- ── lifecycle ─────────────────────────────────────────────────────────────
function onStart()
    tw = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf", 2)
    print("[TextRenderingTest] Building glyph atlas...")
    tw:buildGlyphAtlas(512)

    input  = Components:Input()
    win    = Components:Window()
    render = Components:Render()

    print("[TextRenderingTest] Ready. Controls:")
    print("  0  toggle ALL")
    print("  1  toggle TTF AutoSize")
    print("  2  toggle TTF explicit size")
    print("  3  toggle Text Atlas")
    print("  4  toggle Centered / Aligned")
    print("  5  toggle Text Cache")
    print("  6  toggle Legend")
    print("  R  rebuild glyph atlas")
end

function onUpdate()
    input  = Components:Input()
    win    = Components:Window()
    render = Components:Render()

    if input:isCharFirstEventDown("0") then
        show = not show
        print("[TextRenderingTest] show ALL: " .. tostring(show))
    end
    if input:isCharFirstEventDown("1") then
        showAuto = not showAuto
        print("[TextRenderingTest] show AutoSize: " .. tostring(showAuto))
    end
    if input:isCharFirstEventDown("2") then
        showSize = not showSize
        print("[TextRenderingTest] show Size: " .. tostring(showSize))
    end
    if input:isCharFirstEventDown("3") then
        showAtlas = not showAtlas
        print("[TextRenderingTest] show Atlas: " .. tostring(showAtlas))
    end
    if input:isCharFirstEventDown("4") then
        showCent = not showCent
        print("[TextRenderingTest] show Center: " .. tostring(showCent))
    end
    if input:isCharFirstEventDown("5") then
        showCache = not showCache
        print("[TextRenderingTest] show Cache: " .. tostring(showCache))
    end
    if input:isCharFirstEventDown("6") then
        showLegend = not showLegend
        print("[TextRenderingTest] show Legend: " .. tostring(showLegend))
    end
    if input:isCharFirstEventDown("R") then
        rebuildAtlas()
    end

    local ok, err = pcall(draw)
    if not ok then
        print("[TextRenderingTest] ERROR: " .. tostring(err))
    end
end

function onEnd()
end
