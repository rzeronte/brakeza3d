-- DemoBrowser.lua (global script of TutorialsProject)
-- On-screen browser for the demo scenes of the project: a bottom bar to go to the previous/next demo,
-- a list with every demo and an info panel (what the demo shows, its controls, what to look at in the
-- code, and the files it uses).
--
-- The demo catalog (titles, categories, descriptions, controls) lives in demos.json, next to this script.
-- Each demo has a "category"; the list groups the demos by category (in columns) and PgDn/PgUp walk them in that
-- order. The files/scripts/shaders shown in the info panel are read from each scene JSON, so they never go stale.
--
-- Switching demos, all from Lua while the scripts are running:
--   1. unloadScene(<current>)      objects, scene scripts and scene shaders of the current demo
--   2. removeAllObjects()          plus anything its scripts spawned at runtime (cubes, projectiles...)
--   3. wait two frames             the engine frees removed objects one frame later
--   4. loadSceneAdditive(path, scripts, shaders, camera, renderSettings, startScripts=true)
--      startScripts: the scene scripts get onStart() right away (otherwise only Play calls it)
--
-- The browser starts in TOUR mode: a short playlist of the most striking demos ("tour" in demos.json, by scene
-- name). PgDn/PgUp walk the tour; F3 (or the Tour/All button) switches to ALL, which walks every demo in
-- catalog order. F4 (Auto button) advances by itself every AUTO_SECONDS seconds, like a slide show.
--
-- Keys: PageDown / PageUp next / previous, Home reload, F1 demo list, F2 info panel, F3 tour/all, F4 auto.
-- They are chosen not to clash with the demos (letters, numbers and arrows are used by them).
--
-- From other scripts: scripting:callGlobalScriptFunction("DemoBrowser", "goToDemo", i) (or
-- "showDemoList", true/false) and the
-- globals currentDemo / demoCount / browserBusy (scripting:getGlobalScriptVar("DemoBrowser", ...)).

local CATALOG = "../assets/scripts/Browser/demos.json"

local render, input, win, scripting, loader, tw

local demos      = {}        -- demos.json entries + facts read from the scene JSON
local categories = {}        -- { {name = "Graphics", items = {demo indices}}, ... } in browsing order
local current    = 0         -- loaded demo (0 = none)
local sceneName  = nil       -- Scene name of the loaded demo (file name without .json)
local state      = "idle"    -- idle | clearing | loading
local stateFrames = 0
local pending    = 0         -- demo being loaded
local showList   = false
local showInfo   = true
local mode       = "tour"    -- tour | all: which demos Prev/Next walk
local tourList   = {}        -- demo indices of the tour, in tour order
local autoplay   = false
local idleTime   = 0.0       -- seconds the current demo has been running (for autoplay)
local AUTO_SECONDS = 20.0
local prevLeft   = false
local infoCache  = { key = nil, lines = nil }

-- ── Style ─────────────────────────────────────────────────────────────────
-- The UI is designed for a 1009 px high window and scales with the real window height (applyScale), so in a
-- small window the bar, the buttons, the panels and the text shrink instead of covering the demo.
local DESIGN_H = 1009
local U        = 1.0         -- current scale factor
local S_TITLE, S_TEXT, S_SMALL = 0.62, 0.46, 0.40
local BAR_H   = 56
local PAD     = 10
local BTN_W, BTN_W_S = 120, 90
local GAP_S, GAP_M = 8, 10

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
    return math.floor(40 * scale + 6 * U)
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
    local tourNames = (cat and cat.tour) or {}
    demos = {}
    for _, e in ipairs((cat and cat.demos) or {}) do
        if e.scene ~= nil then
            e.title    = e.title or stem(e.scene)
            e.summary  = e.summary or ""
            e.controls = e.controls or {}
            e.learn    = e.learn or {}
            e.category = e.category or "Other"
            e.facts    = readSceneFacts(e.scene)
            demos[#demos + 1] = e
        end
    end

    -- Agrupar por categoría (orden de primera aparición) y numerar las demos en ese orden, así Prev/Next y la
    -- lista recorren lo mismo aunque en demos.json dos demos de una categoría no estén seguidas
    local order, byName = {}, {}
    for _, e in ipairs(demos) do
        if byName[e.category] == nil then
            byName[e.category] = { name = e.category, demos = {} }
            order[#order + 1] = byName[e.category]
        end
        local list = byName[e.category].demos
        list[#list + 1] = e
    end
    demos, categories = {}, {}
    for _, cat in ipairs(order) do
        local entry = { name = cat.name, items = {} }
        for _, e in ipairs(cat.demos) do
            demos[#demos + 1] = e
            entry.items[#entry.items + 1] = #demos
        end
        categories[#categories + 1] = entry
    end

    -- Tour: demos.json lists scene names; here they become demo indices (names that do not exist are ignored)
    local byStem = {}
    for i, e in ipairs(demos) do byStem[stem(e.scene)] = i end
    tourList = {}
    for _, name in ipairs(tourNames) do
        local i = byStem[name]
        if i ~= nil then
            tourList[#tourList + 1] = i
            demos[i].inTour = true
        end
    end
    if #tourList == 0 then mode = "all" end
    print("[DemoBrowser] " .. #demos .. " demos in " .. #categories .. " categories, tour of " .. #tourList .. " (" .. CATALOG .. ")")
end

-- ── Switching demos ───────────────────────────────────────────────────────
-- Al descargar una escena sus scripts se descartan SIN llamar a su onEnd (los de objeto, tampoco, al eliminarlos). Lo que
-- una demo haya cambiado en el motor y no se recarga con la escena se restablece aquí, o pasaría a la demo siguiente:
-- el teclado de la cámara apagado (lección CameraFollow), el ratón capturado (primera persona), el paneo del audio,
-- la música. La iluminación y la cámara sí se recargan con cada escena (ADS y camera del JSON).
local function resetEngineState()
    pcall(function()
        local inp = Components:Input()
        inp:setEnabled(true)
        inp:setKeyboardEnabled(true)
        inp:setMouseEnabled(true)
        inp:setRelativeMouseMode(false)      -- exe antiguo sin este binding: se ignora
    end)
    pcall(function() Components:Render():setDragSelectEnabled(true) end)   -- recuadro de selección (lo apagan las demos de cámara)
    pcall(function() Components:Sound():setAmbiencePanning(false) end)
    pcall(function() Components:Sound():setAmbienceVolume(100) end)
    pcall(function() Components:Sound():StopMusic() end)
    pcall(function() Components:Window():setClearColor(0.0, 0.0, 0.0, 0.0) end)   -- fondo por defecto (negro)
    pcall(function() Components:Collisions():setEnableDebugMode(false) end)      -- wireframe de la fisica (lo enciende la demo de los cubos)
end

local function clearWorld()
    resetEngineState()
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

-- Position of the current demo inside the tour (0 = not in it)
local function tourPosition()
    for k, i in ipairs(tourList) do
        if i == current then return k end
    end
    return 0
end

-- Demo that is `delta` steps from the current one in the active mode
local function neighbour(delta)
    if mode == "tour" and #tourList > 0 then
        local pos = tourPosition()
        if pos == 0 then return delta > 0 and tourList[1] or tourList[#tourList] end
        return tourList[((pos - 1 + delta) % #tourList) + 1]
    end
    return ((current - 1 + delta) % #demos) + 1
end

local function step(delta)
    requestDemo(neighbour(delta))
end

local function toggleMode()
    if #tourList == 0 then return end
    mode = (mode == "tour") and "all" or "tour"
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
    out[#out + 1] = { gap = GAP_S }
    out[#out + 1] = { t = title, c = C.section, s = S_SMALL }
end

local function buildInfo(d, maxW)
    local out = {}
    out[#out + 1] = { t = string.upper(d.category), c = C.accent, s = S_SMALL }
    wrap(d.title, S_TITLE, maxW, out, C.title)
    out[#out + 1] = { gap = math.floor(4 * U) }
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

    out[#out + 1] = { gap = GAP_M }
    wrap("PgDn/PgUp next/previous - F3 tour/all - F4 auto - Home reload - F1 list - F2 hide this panel", S_SMALL, maxW, out, C.dim)
    return out
end

local function drawInfo(sw)
    local d = demos[current]
    if d == nil then return end
    local w = math.floor(math.min(540 * U, sw * 0.36))
    local x = sw - w - math.floor(16 * U)
    local y = math.floor(16 * U)
    local key = current .. ":" .. w .. ":" .. S_SMALL
    if infoCache.key ~= key then
        infoCache.key, infoCache.lines = key, buildInfo(d, w - 2 * PAD - 6)
    end

    local h = 2 * PAD + 6
    for _, l in ipairs(infoCache.lines) do h = h + (l.gap or lineH(l.s)) end
    rect(x, y, w, h, C.panel)
    rect(x, y, math.max(2, math.floor(4 * U)), h, C.accent)
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
-- Grouped by category. Each category goes whole into a column; a new column starts when the next
-- category does not fit in the height available above the bar.
local LIST_ROW_H, LIST_ROW_GAP, LIST_COL_W = 30, 3, 360

-- Recomputes every pixel size from the window height. Called once per frame.
local function applyScale(sh)
    U = math.max(0.55, math.min(1.5, sh / DESIGN_H))
    S_TITLE, S_TEXT, S_SMALL = 0.62 * U, 0.46 * U, 0.40 * U
    BAR_H, PAD = math.floor(56 * U), math.max(4, math.floor(10 * U))
    BTN_W, BTN_W_S = math.floor(120 * U), math.floor(90 * U)
    GAP_S, GAP_M = math.max(2, math.floor(8 * U)), math.max(2, math.floor(10 * U))
    LIST_ROW_H, LIST_ROW_GAP, LIST_COL_W = math.floor(30 * U), math.max(1, math.floor(3 * U)), math.floor(360 * U)
end

local function drawList(barY)
    local maxH   = barY - 2 * PAD - GAP_S
    local headH  = lineH(S_SMALL) + 2
    local titleH = lineH(S_SMALL)

    -- Reparto de categorías en columnas
    local cols, used = { {} }, { titleH + PAD }
    for _, cat in ipairs(categories) do
        local blockH = headH + #cat.items * (LIST_ROW_H + LIST_ROW_GAP) + GAP_S
        local c = #cols
        if used[c] + blockH > maxH and #cols[c] > 0 then
            cols[#cols + 1] = {}
            used[#cols] = PAD
            c = #cols
        end
        cols[c][#cols[c] + 1] = cat
        used[c] = used[c] + blockH
    end

    local h = PAD
    for _, u in ipairs(used) do h = math.max(h, u + PAD) end
    local w = #cols * (LIST_COL_W + PAD) + PAD
    local x, y = PAD, barY - h - GAP_S
    rect(x, y, w, h, C.panel)
    uiArea(x, y, w, h)
    text(x + PAD, y + PAD, "DEMOS (" .. #demos .. ")   -   " .. #categories .. " categories   -   * = in the tour", C.dim, S_SMALL)

    for ci, catList in ipairs(cols) do
        local cx = x + PAD + (ci - 1) * (LIST_COL_W + PAD)
        local ry = y + ((ci == 1) and (PAD + titleH) or PAD)
        for _, cat in ipairs(catList) do
            text(cx, ry, string.upper(cat.name), C.section, S_SMALL)
            ry = ry + headH
            for _, i in ipairs(cat.items) do
                local label = string.format("%02d  %s%s", i, demos[i].title, demos[i].inTour and "  *" or "")
                if button(cx, ry, LIST_COL_W, LIST_ROW_H, label, { active = (i == current), disabled = state ~= "idle" }) then
                    requestDemo(i)
                end
                ry = ry + LIST_ROW_H + LIST_ROW_GAP
            end
            ry = ry + GAP_S
        end
    end
end

-- ── Bottom bar ────────────────────────────────────────────────────────────
local function drawBar(sw, sh)
    local y = sh - BAR_H
    rect(0, y, sw, BAR_H, C.bar)
    rect(0, y, sw, math.max(1, math.floor(2 * U)), C.accent)
    uiArea(0, y, sw, BAR_H)

    local busy = state ~= "idle"
    local bh, by = BAR_H - 2 * PAD, y + PAD
    local x = PAD
    if button(x, by, BTN_W, bh, "Demos", { active = showList }) then showList = not showList end
    x = x + BTN_W + PAD
    if button(x, by, BTN_W_S, bh, mode == "tour" and "Tour" or "All", { active = mode == "tour", disabled = #tourList == 0 }) then toggleMode() end
    x = x + BTN_W_S + PAD
    if button(x, by, BTN_W, bh, "Prev", { disabled = busy }) then step(-1) end
    x = x + BTN_W + PAD

    local right = sw - PAD
    right = right - BTN_W_S
    if button(right, by, BTN_W_S, bh, "Info", { active = showInfo }) then showInfo = not showInfo end
    right = right - PAD - BTN_W
    if button(right, by, BTN_W, bh, "Reload", { disabled = busy or current == 0 }) then requestDemo(current) end
    right = right - PAD - BTN_W
    if button(right, by, BTN_W, bh, "Next", { disabled = busy }) then step(1) end
    right = right - PAD - BTN_W_S
    if button(right, by, BTN_W_S, bh, "Auto", { active = autoplay }) then autoplay = not autoplay; idleTime = 0.0 end

    -- Centre: current demo or loading state
    local label, color
    if busy then
        label, color = "Loading " .. demos[pending].title .. " ...", C.loading
    elseif current > 0 then
        local pos = tourPosition()
        if mode == "tour" and pos > 0 then
            label = string.format("Tour %d / %d   -   %s", pos, #tourList, demos[current].title)
        else
            label = string.format("%d / %d   %s  -  %s", current, #demos, demos[current].category, demos[current].title)
        end
        color = C.title
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
    mode, autoplay, idleTime = "tour", false, 0.0
    infoCache = { key = nil, lines = nil }
    loadCatalog()
    requestDemo((mode == "tour" and #tourList > 0) and tourList[1] or 1)
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
        if input:isCharFirstEventDown("PageDown") then step(1) end
        if input:isCharFirstEventDown("PageUp") then step(-1) end
        if input:isCharFirstEventDown("Home") and current > 0 then requestDemo(current) end
    end
    if input:isCharFirstEventDown("F1") then showList = not showList end
    if input:isCharFirstEventDown("F2") then showInfo = not showInfo end
    if input:isCharFirstEventDown("F3") then toggleMode() end
    if input:isCharFirstEventDown("F4") then autoplay = not autoplay; idleTime = 0.0 end

    -- Slide show: pasa a la siguiente demo cuando la actual lleva AUTO_SECONDS en marcha
    if state == "idle" and current > 0 then
        idleTime = idleTime + math.min(Brakeza:getDeltaTime(), 0.1)
        if autoplay and idleTime >= AUTO_SECONDS then
            idleTime = 0.0
            step(1)
        end
    else
        idleTime = 0.0
    end

    local sw, sh = win:getWidth(), win:getHeight()
    applyScale(sh)
    local barY = drawBar(sw, sh)
    if showList then drawList(barY) end
    if showInfo and state == "idle" then drawInfo(sw) end

    if mouse.overUI and down then input:consumeLeftClick() end

    updateState()
    publishState()
end
