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

-- layout (100px margins all around)
local MARGIN    = 100
local COL1_X    = MARGIN
local COL2_X    = MARGIN + 320
local COL3_X    = MARGIN + 640
local ROW_H     = 28
local BASE_Y    = MARGIN

-- ── rebuild glyph atlas ───────────────────────────────────────────────────
local function rebuildAtlas()
    print("[TextRenderingTest] Rebuilding glyph atlas...")
    tw:buildGlyphAtlas(512)
    print("[TextRenderingTest] Glyph atlas rebuilt")
end

-- ── section helper ────────────────────────────────────────────────────────
local function drawSectionLabel(label, x, y, color)
    tw:writeTextTTFAutoSize(x, y, label, color, 0.70)
end

-- ── draw ──────────────────────────────────────────────────────────────────
function draw()
    if not show then return end

    local sw = win:getWidth();  if sw == 0 then sw = 1280 end
    local sh = win:getHeight(); if sh == 0 then sh = 720  end

    local row = 0
    local function nextRow(n) row = row + (n or 1); return BASE_Y + row * ROW_H end

    -- ── 1: TTF AutoSize ────────────────────────────────────────────────────
    row = 0
    if showAuto then
        drawSectionLabel("[1] TTF AutoSize — writeTextTTFAutoSize", COL1_X, nextRow(), C_AUTO)

        tw:writeTextTTFAutoSize(COL1_X + 10, nextRow(), SAMPLE .. "  (scale 1.0)", C_AUTO, 1.00)
        tw:writeTextTTFAutoSize(COL1_X + 10, nextRow(), SAMPLE .. "  (scale 0.8)",  C_AUTO, 0.80)
        tw:writeTextTTFAutoSize(COL1_X + 10, nextRow(), SAMPLE .. "  (scale 0.6)",  C_AUTO, 0.60)
        tw:writeTextTTFAutoSize(COL1_X + 10, nextRow(), SAMPLE .. "  (scale 0.4)",  C_AUTO, 0.40)
        tw:writeTextTTFAutoSize(COL1_X + 10, nextRow(), SAMPLE .. "  (scale 0.25)", C_AUTO, 0.25)
    end

    -- ── 2: TTF explicit size ───────────────────────────────────────────────
    if showSize then
        drawSectionLabel("[2] TTF Size — writeTextTTF", COL1_X, nextRow(), C_SIZE)

        tw:writeTextTTF(COL1_X + 10, nextRow(), 300, 40, SAMPLE, C_SIZE)
        tw:writeTextTTF(COL1_X + 10, nextRow(), 200, 28, SAMPLE, C_SIZE)
        tw:writeTextTTF(COL1_X + 10, nextRow(), 150, 20, SAMPLE, C_SIZE)
        tw:writeTextTTF(COL1_X + 10, nextRow(), 100, 14, SAMPLE, C_SIZE)
    end

    -- ── 3: Text Atlas (column 2) ───────────────────────────────────────────
    if showAtlas then
        local ax = COL2_X
        local ay = BASE_Y + ROW_H * 0

        -- dark background behind the whole atlas section
        render:DrawFilledRect(ax - 10, ay - 4, 290, ROW_H * 12 + 20, Color.new(0.05, 0.05, 0.30, 0.60))

        drawSectionLabel("[3] Text Atlas — writeTextAtlas (batched)", ax, ay, C_ATLAS)

        -- Courier font atlas (mixed case — realistic scenario)
        tw:writeTextAtlas(ax + 10, ay + ROW_H * 1, "Hello Brakeza3D! HUD text with ascenders", C_ATLAS, 0.80)
        tw:writeTextAtlas(ax + 10, ay + ROW_H * 2, "FPS: 60  HP: 100  SCORE: 9999", C_ATLAS, 0.70)
        tw:writeTextAtlas(ax + 10, ay + ROW_H * 3, "Player hit by enemy: -25 HP", C_ATLAS, 0.60)
        tw:writeTextAtlas(ax + 10, ay + ROW_H * 4, "0123456789 !@#$%^&*() abcdefghijklmnopqrstuvwxyz", C_ATLAS, 0.55)
        tw:writeTextAtlas(ax + 10, ay + ROW_H * 5, "ABC DEF GHI JKL MNO PQR STU VW XYZ  (scale 0.40)", C_ATLAS, 0.40)
        tw:writeTextAtlas(ax + 10, ay + ROW_H * 6, "Mixed case with Caps: 0.25", C_ATLAS, 0.25)

        -- live counter to prove it updates every frame
        local execTime = string.format("%.3f", Brakeza:getExecutionTime())
        tw:writeTextAtlas(ax + 10, ay + ROW_H * 7, "Engine time: " .. execTime .. "s  (live — changes each frame!)", Color.new(0.30, 1.00, 1.00, 1.0), 0.65)

        -- engine default glyph atlas (TroubleFont) - different visual style
        local engTw = render:getTextWriter()
        if engTw and engTw:getGlyphAtlas() and engTw:getGlyphAtlas():isBuilt() then
            local ga = engTw:getGlyphAtlas()
            engTw:writeTextAtlas(ax + 10, ay + ROW_H * 9, "Engine TroubleFont atlas (GL tex " .. ga:getAtlasTexture() .. ")", Color.new(0.50, 0.80, 1.00, 1.0), 0.65)
            engTw:writeTextAtlas(ax + 10, ay + ROW_H * 10, "ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789", Color.new(0.50, 0.80, 1.00, 1.0), 0.55)
        else
            tw:writeTextAtlas(ax + 10, ay + ROW_H * 9, "Engine glyph atlas NOT BUILT — check logs", Color.new(1.00, 0.30, 0.30, 1.0), 0.65)
        end
    end

    -- ── 4: Centered / Aligned (column 3) ───────────────────────────────────
    if showCent then
        drawSectionLabel("[4] Centered — writeTextTTFMiddleScreen / writeTTFCenterHorizontal", COL3_X, BASE_Y + ROW_H * 0, C_CENTER)

        tw:writeTextTTFMiddleScreen("MiddleScreen — " .. SAMPLE, C_CENTER, 0.65)
        tw:writeTTFCenterHorizontal(BASE_Y + ROW_H * 2, "CenterH row 2 — " .. SAMPLE, C_CENTER, 0.55)
        tw:writeTTFCenterHorizontal(BASE_Y + ROW_H * 3, "CenterH row 3 — " .. SAMPLE, C_CENTER, 0.45)
        tw:writeTTFCenterHorizontal(BASE_Y + ROW_H * 4, "CenterH row 4 — " .. SAMPLE, C_CENTER, 0.35)
    end

    -- ── 5: Text Cache (below col 1) ────────────────────────────────────────
    local row5_start = BASE_Y + ROW_H * 12
    if showCache then
        drawSectionLabel("[5] Text Cache — beginTextCache / endTextCache / drawTextCache", COL1_X, row5_start, C_CACHE)

        if not cacheBuilt then
            tw:beginTextCache("tc_header", 380, 30)
                tw:writeTextTTFAutoSize(0, 0, "CACHED HEADER — this text is baked into an FBO", C_CACHE, 0.65)
            tw:endTextCache()

            tw:beginTextCache("tc_body", 380, 150)
                local y = 0
                tw:writeTextTTFAutoSize(0, y, "Cached body line 1: persistent, no per-frame cost", C_CACHE, 0.55); y = y + 26
                tw:writeTextTTFAutoSize(0, y, "Cached body line 2: only the drawTextCache blit",  C_CACHE, 0.55); y = y + 26
                tw:writeTextTTFAutoSize(0, y, "Cached body line 3: happens each frame",            C_CACHE, 0.55); y = y + 26
                tw:writeTextTTFAutoSize(0, y, "Cached body line 4: content is static",             C_CACHE, 0.55); y = y + 26
                tw:writeTextTTFAutoSize(0, y, "Cached body line 5: only rebuilt if dirty flag",    C_CACHE, 0.55)
            tw:endTextCache()
            cacheBuilt = true
        end

        tw:drawTextCache("tc_header", COL1_X + 10, row5_start + ROW_H * 1)
        tw:drawTextCache("tc_body",   COL1_X + 10, row5_start + ROW_H * 3)
    end

    -- ── FPS (always, using atlas, within margins) ──────────────────────────
    local fps = render:getFps()
    local fpsColor = fps >= 50 and Color.new(0.20, 1.00, 0.30, 1.0) or (fps >= 30 and Color.new(1.00, 0.80, 0.00, 1.0) or Color.new(1.00, 0.20, 0.20, 1.0))
    tw:writeTextAtlas(sw - MARGIN - 100, MARGIN - 40, "FPS: " .. fps, fpsColor, 0.70)

    -- ── legend (within bottom margin) ──────────────────────────────────────
    local function leg_color(on, c) if on then return c else return C_DIM end end
    if showLegend then
    local ly = sh - MARGIN
    local LEG = { MARGIN, MARGIN + 160, MARGIN + 320, MARGIN + 480, MARGIN + 640, MARGIN + 800, MARGIN + 960, MARGIN + 1120 }
    tw:writeTextTTFAutoSize(LEG[1],           ly, "0",               C_KEY,                0.55)
    tw:writeTextTTFAutoSize(LEG[1] + 20,      ly, "toggle ALL",      leg_color(show, C_GREY), 0.55)
    tw:writeTextTTFAutoSize(LEG[2],           ly, "1",               C_KEY,                0.55)
    tw:writeTextTTFAutoSize(LEG[2] + 20,      ly, "TTF AutoSize",    leg_color(showAuto, C_AUTO), 0.55)
    tw:writeTextTTFAutoSize(LEG[3],           ly, "2",               C_KEY,                0.55)
    tw:writeTextTTFAutoSize(LEG[3] + 20,      ly, "TTF Size",        leg_color(showSize, C_SIZE), 0.55)
    tw:writeTextTTFAutoSize(LEG[4],           ly, "3",               C_KEY,                0.55)
    tw:writeTextTTFAutoSize(LEG[4] + 20,      ly, "Atlas (batched)", leg_color(showAtlas, C_ATLAS), 0.55)
    tw:writeTextTTFAutoSize(LEG[5],           ly, "4",               C_KEY,                0.55)
    tw:writeTextTTFAutoSize(LEG[5] + 20,      ly, "Center",          leg_color(showCent, C_CENTER), 0.55)
    tw:writeTextTTFAutoSize(LEG[6],           ly, "5",               C_KEY,                0.55)
    tw:writeTextTTFAutoSize(LEG[6] + 20,      ly, "Cache",           leg_color(showCache, C_CACHE), 0.55)
    tw:writeTextTTFAutoSize(LEG[7],           ly, "6",               C_KEY,                0.55)
    tw:writeTextTTFAutoSize(LEG[7] + 20,      ly, "Legend",          leg_color(showLegend, C_GREY), 0.55)
    tw:writeTextTTFAutoSize(LEG[8],           ly, "R",               C_KEY,                0.55)
    tw:writeTextTTFAutoSize(LEG[8] + 20,      ly, "rebuild atlas",   C_GREY,               0.55)
    end
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
