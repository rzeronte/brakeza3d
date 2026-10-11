-- DemoBrowser.lua (global script of TutorialsProject)
-- On-screen browser for the demo scenes of the project: a bottom bar to go to the previous/next demo,
-- a list with every demo and an info panel (what the demo shows, its controls, what to look at in the
-- code, and the files it uses).
--
-- The demo catalog (titles, descriptions, controls) lives in demos.json, next to this script. The
-- files/scripts/shaders shown in the info panel are read from each scene JSON, so they never go stale.
--
-- Switching demos, all from Lua while the scripts are running:
--   1. unloadScene(<current>)      objects, scene scripts and scene shaders of the current demo
--   2. removeAllObjects()          plus anything its scripts spawned at runtime (cubes, projectiles...)
--   3. wait two frames             the engine frees removed objects one frame later
--   4. loadSceneAdditive(path, scripts, shaders, camera, renderSettings, startScripts=true)
--      startScripts: the scene scripts get onStart() right away (otherwise only Play calls it)
--
-- Keys: PageDown / PageUp next / previous, Home reload, F1 demo list, F2 info panel.
-- They are chosen not to clash with the demos (letters, numbers and arrows are used by them).
--
-- From other scripts: scripting:callGlobalScriptFunction("DemoBrowser", "goToDemo", i) (or
-- "showDemoList", true/false) and the
-- globals currentDemo / demoCount / browserBusy (scripting:getGlobalScriptVar("DemoBrowser", ...)).

local CATALOG = "../assets/scripts/Tutorials/Global/demos.json"

local render, input, win, scripting, loader, tw

local demos      = {}        -- demos.json entries + facts read from the scene JSON
local current    = 0         -- loaded demo (0 = none)
local sceneName  = nil       -- Scene name of the loaded demo (file name without .json)
local state      = "idle"    -- idle | clearing | loading
local stateFrames = 0
local pending    = 0         -- demo being loaded
local showList   = false
local showInfo   = true
local prevLeft   = false
local infoCache  = { key = nil, lines = nil }

-- ── Style ─────────────────────────────────────────────────────────────────
local S_TITLE = 0.62
local S_TEXT  = 0.46
local S_SMALL = 0.40
local BAR_H   = 56
local PAD     = 10

local C = {
    bar      = Color.new(0.06, 0.07, 0.09, 0.88),
    panel    = Color.new(0.06, 0.07, 0.09, 0.82),
    btn      = Color.new(0.16, 0.18, 0.22, 1.0),
    btnHover = Color.new(0.24, 0.30, 0.40, 1.0),
    btnOn    = Color.new(0.15, 0.42, 0.62, 1.0),
    btnOff   = Color.new(0.11, 0.12, 0.14, 1.0),
    accent   = Color.new(0.30, 0.75, 1.00, 1.0),
    title    = Color.new(1.00, 1.00, 1.00, 1.0),
    text     = Color.new(0.86, 0.87, 0.90, 1.0),
    dim      = Color.new(0.55, 0.57, 0.62, 1.0),
    key      = Color.new(1.00, 0.85, 0.30, 1.0),
    section  = Color.new(0.45, 0.80, 0.55, 1.0),
    loading  = Color.new(1.00, 0.75, 0.25, 1.0),
}

-- ── Drawing helpers (window coordinates, same as the raw mouse position) ──
local function rect(x, y, w, h, c)
    render:DrawFilledRect(math.floor(x), math.floor(y), math.floor(w), math.floor(h), c)
end

local function text(x, y, s, c, scale)
    tw:writeTextAtlas(math.floor(x), math.floor(y), s, c, scale)
end

local function textW(s, scale)
    return tw:measureTextWidthAtlas(s, scale)
end

local function lineH(scale)
    return math.floor(40 * scale + 6)
end

-- Mouse state for this frame
local mouse = { x = 0, y = 0, clicked = false, overUI = false }

local function inside(x, y, w, h)
    return mouse.x >= x and mouse.x <= x + w and mouse.y >= y and mouse.y <= y + h
end

-- Marks an area as UI: clicks inside it are not meant for the demo
local function uiArea(x, y, w, h)
    if inside(x, y, w, h) then mouse.overUI = true end
end

-- Draws a button and returns true when it is clicked. opts: active, disabled
local function button(x, y, w, h, label, opts)
    opts = opts or {}
    local hover = not opts.disabled and inside(x, y, w, h)
    local bg = C.btn
    if opts.disabled then bg = C.btnOff
    elseif opts.active then bg = C.btnOn
    elseif hover then bg = C.btnHover end
    rect(x, y, w, h, bg)
    local tc = opts.disabled and C.dim or C.title
    local tx = x + (w - textW(label, S_TEXT)) / 2
    local ty = y + (h - lineH(S_TEXT)) / 2 + 2
    text(tx, ty, label, tc, S_TEXT)
    return hover and mouse.clicked
end

-- ── Catalog and scene facts ───────────────────────────────────────────────
local function stem(path)
    return path:match("([^/\\]+)%.json$") or path
end

local function baseName(path)
    return tostring(path):match("([^/\\]+)$") or tostring(path)
end

-- What the scene JSON says about the demo: object count, scripts, shaders, lights file
local function readSceneFacts(path)
    local facts = { objects = 0, sceneScripts = {}, objectScripts = {}, shaders = {}, lightsFile = nil }
    local d = scripting:loadJSON(path)
    if d == nil then return facts end

    local seen = {}
    local function walk(o)
        facts.objects = facts.objects + 1
        for _, s in ipairs(o.scripts or {}) do
            local f = type(s) == "table" and (s.codeFile or s.name) or s
            if f ~= nil and not seen[f] then
                seen[f] = true
                facts.objectScripts[#facts.objectScripts + 1] = baseName(f)
            end
        end
        for _, c in ipairs(o.children or {}) do walk(c) end
    end
    for _, o in ipairs(d.objects or {}) do walk(o) end
    for _, s in ipairs(d.scripts or {}) do
        facts.sceneScripts[#facts.sceneScripts + 1] = baseName(s.codeFile or s.name)
    end
    for _, s in ipairs(d.shaders or {}) do
        facts.shaders[#facts.shaders + 1] = tostring(s.name or s.label or "?")
    end
    facts.lightsFile = d.lightsFile
    return facts
end

local function loadCatalog()
    local cat = scripting:loadJSON(CATALOG)
    demos = {}
    for _, e in ipairs((cat and cat.demos) or {}) do
        if e.scene ~= nil then
            e.title    = e.title or stem(e.scene)
            e.summary  = e.summary or ""
            e.controls = e.controls or {}
            e.learn    = e.learn or {}
            e.facts    = readSceneFacts(e.scene)
            demos[#demos + 1] = e
        end
    end
    print("[DemoBrowser] " .. #demos .. " demos in " .. CATALOG)
end

-- ── Switching demos ───────────────────────────────────────────────────────
local function clearWorld()
    Brakeza:cancelPendingJobs()
    if sceneName ~= nil then loader:unloadScene(sceneName) end
    Brakeza:removeAllObjects()
    render:clearSceneShaders()
    sceneName = nil
end

local function requestDemo(i)
    if #demos == 0 or state ~= "idle" then return end
    i = ((i - 1) % #demos) + 1
    pending = i
    showList = false
    clearWorld()
    state, stateFrames = "clearing", 0
    print("[DemoBrowser] -> " .. demos[i].title .. " (" .. demos[i].scene .. ")")
end

local function updateState()
    if state == "idle" then return end
    stateFrames = stateFrames + 1
    if state == "clearing" then
        if stateFrames >= 2 then
            local d = demos[pending]
            loader:loadSceneAdditive(d.scene, true, true, true, true, true)
            sceneName = stem(d.scene)
            current = pending
            state, stateFrames = "loading", 0
        end
    elseif state == "loading" then
        if stateFrames >= 2 and Brakeza:getPendingJobsCount() == 0 then
            state = "idle"
        end
    end
end

-- ── Info panel ────────────────────────────────────────────────────────────
-- Word wrap with the atlas font metrics
local function wrap(str, scale, maxW, out, color)
    local line = ""
    for word in tostring(str):gmatch("%S+") do
        local candidate = (line == "") and word or (line .. " " .. word)
        if line ~= "" and textW(candidate, scale) > maxW then
            out[#out + 1] = { t = line, c = color, s = scale }
            line = word
        else
            line = candidate
        end
    end
    if line ~= "" then out[#out + 1] = { t = line, c = color, s = scale } end
end

local function section(out, title)
    out[#out + 1] = { gap = 8 }
    out[#out + 1] = { t = title, c = C.section, s = S_SMALL }
end

local function buildInfo(d, maxW)
    local out = {}
    wrap(d.title, S_TITLE, maxW, out, C.title)
    out[#out + 1] = { gap = 4 }
    wrap(d.summary, S_TEXT, maxW, out, C.text)

    if #d.controls > 0 then
        section(out, "CONTROLS")
        for _, l in ipairs(d.controls) do wrap(l, S_SMALL, maxW, out, C.key) end
    end
    if #d.learn > 0 then
        section(out, "WHAT TO LOOK AT")
        for _, l in ipairs(d.learn) do wrap("- " .. l, S_SMALL, maxW, out, C.text) end
    end

    local f = d.facts
    section(out, "FILES")
    wrap("Scene: " .. d.scene:gsub("^%.%./", ""), S_SMALL, maxW, out, C.dim)
    wrap("Objects in scene: " .. f.objects, S_SMALL, maxW, out, C.dim)
    if #f.sceneScripts > 0 then wrap("Scene scripts: " .. table.concat(f.sceneScripts, ", "), S_SMALL, maxW, out, C.dim) end
    if #f.objectScripts > 0 then wrap("Object scripts: " .. table.concat(f.objectScripts, ", "), S_SMALL, maxW, out, C.dim) end
    if #f.shaders > 0 then wrap("Post-processing: " .. table.concat(f.shaders, ", "), S_SMALL, maxW, out, C.dim) end
    if f.lightsFile ~= nil then wrap("Lights file: " .. baseName(f.lightsFile), S_SMALL, maxW, out, C.dim) end

    out[#out + 1] = { gap = 10 }
    wrap("PgDn/PgUp next/previous - Home reload - F1 list - F2 hide this panel", S_SMALL, maxW, out, C.dim)
    return out
end

local function drawInfo(sw)
    local d = demos[current]
    if d == nil then return end
    local w = math.min(540, math.max(360, math.floor(sw * 0.30)))
    local x = sw - w - 16
    local y = 16
    local key = current .. ":" .. w
    if infoCache.key ~= key then
        infoCache.key, infoCache.lines = key, buildInfo(d, w - 2 * PAD - 6)
    end

    local h = 2 * PAD + 6
    for _, l in ipairs(infoCache.lines) do h = h + (l.gap or lineH(l.s)) end
    rect(x, y, w, h, C.panel)
    rect(x, y, 4, h, C.accent)
    uiArea(x, y, w, h)

    local ty = y + PAD + 2
    for _, l in ipairs(infoCache.lines) do
        if l.gap then
            ty = ty + l.gap
        else
            text(x + PAD + 6, ty, l.t, l.c, l.s)
            ty = ty + lineH(l.s)
        end
    end
end

-- ── Demo list (button panel above the bar) ────────────────────────────────
local function drawList(barY)
    local rowH, w = 34, 460
    local h = #demos * (rowH + 4) + 2 * PAD + lineH(S_SMALL)
    local x, y = PAD, barY - h - 8
    rect(x, y, w, h, C.panel)
    uiArea(x, y, w, h)
    text(x + PAD, y + PAD, "DEMOS (" .. #demos .. ")", C.section, S_SMALL)
    local ry = y + PAD + lineH(S_SMALL)
    for i, d in ipairs(demos) do
        local label = string.format("%02d  %s", i, d.title)
        if button(x + PAD, ry, w - 2 * PAD, rowH, label, { active = (i == current), disabled = state ~= "idle" }) then
            requestDemo(i)
        end
        ry = ry + rowH + 4
    end
end

-- ── Bottom bar ────────────────────────────────────────────────────────────
local function drawBar(sw, sh)
    local y = sh - BAR_H
    rect(0, y, sw, BAR_H, C.bar)
    rect(0, y, sw, 2, C.accent)
    uiArea(0, y, sw, BAR_H)

    local busy = state ~= "idle"
    local bh, by = BAR_H - 2 * PAD, y + PAD
    local x = PAD
    if button(x, by, 120, bh, "Demos", { active = showList }) then showList = not showList end
    x = x + 120 + PAD
    if button(x, by, 120, bh, "Prev", { disabled = busy }) then requestDemo(current - 1) end
    x = x + 120 + PAD

    local right = sw - PAD
    right = right - 90
    if button(right, by, 90, bh, "Info", { active = showInfo }) then showInfo = not showInfo end
    right = right - PAD - 120
    if button(right, by, 120, bh, "Reload", { disabled = busy or current == 0 }) then requestDemo(current) end
    right = right - PAD - 120
    if button(right, by, 120, bh, "Next", { disabled = busy }) then requestDemo(current + 1) end

    -- Centre: current demo or loading state
    local label, color
    if busy then
        label, color = "Loading " .. demos[pending].title .. " ...", C.loading
    elseif current > 0 then
        label, color = string.format("%d / %d   %s", current, #demos, demos[current].title), C.title
    else
        label, color = "No demo loaded", C.dim
    end
    local cw = right - PAD - x
    local tx = x + math.max(0, (cw - textW(label, S_TEXT)) / 2)
    text(tx, y + (BAR_H - lineH(S_TEXT)) / 2 + 2, label, color, S_TEXT)
    return y
end

-- ── API for other scripts ─────────────────────────────────────────────────
function goToDemo(i)
    requestDemo(tonumber(i) or 1)
end

function showDemoList(visible)
    showList = visible and true or false
end

local function publishState()
    currentDemo = current
    demoCount   = #demos
    browserBusy = state ~= "idle"
end

-- ── Lifecycle ─────────────────────────────────────────────────────────────
function onStart()
    render    = Components:Render()
    input     = Components:Input()
    win       = Components:Window()
    scripting = Components:Scripting()
    loader    = render:getSceneLoader()
    tw        = render:getTextWriter()
    if textW("A", 1.0) == 0 then tw:buildGlyphAtlas(512) end

    current, sceneName, state, pending = 0, nil, "idle", 0
    showList, showInfo = false, true
    infoCache = { key = nil, lines = nil }
    loadCatalog()
    requestDemo(1)
    publishState()
end

function onUpdate()
    if #demos == 0 then return end

    local down = input:isLeftMouseButtonPressed()
    mouse.x, mouse.y = input:getRawMouseX(), input:getRawMouseY()
    mouse.clicked = down and not prevLeft
    mouse.overUI = false
    prevLeft = down

    if state == "idle" then
        if input:isCharFirstEventDown("PageDown") then requestDemo(current + 1) end
        if input:isCharFirstEventDown("PageUp") then requestDemo(current - 1) end
        if input:isCharFirstEventDown("Home") and current > 0 then requestDemo(current) end
    end
    if input:isCharFirstEventDown("F1") then showList = not showList end
    if input:isCharFirstEventDown("F2") then showInfo = not showInfo end

    local sw, sh = win:getWidth(), win:getHeight()
    local barY = drawBar(sw, sh)
    if showList then drawList(barY) end
    if showInfo and state == "idle" then drawInfo(sw) end

    if mouse.overUI and down then input:consumeLeftClick() end

    updateState()
    publishState()
end
