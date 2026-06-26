local MAX_ENTRIES = 12
local ENTRY_TTL   = 4.0

local log     = {}
local render  = nil
local tw      = nil
local hitCount = 0

local C_HEAD  = Color.new(1.0,  0.4,  0.4,  1.0)
local C_ENTRY = Color.new(1.0,  1.0,  0.7,  1.0)
local C_OLD   = Color.new(0.55, 0.55, 0.55, 0.75)

function onStart()
    render = Components:Render()
    tw     = render:getTextWriter()
end

function onUpdate()
    local dt = Brakeza:getDeltaTime()
    local alive = {}
    for _, e in ipairs(log) do
        e.ttl = e.ttl - dt
        if e.ttl > 0 then
            table.insert(alive, e)
        end
    end
    log = alive

    drawLog()
end

function onCollision(info)
    hitCount = hitCount + 1

    local name = "unknown"
    local ok, obj = pcall(function() return info:getObject() end)
    if ok and obj ~= nil then
        local ok2, n = pcall(function() return obj:getName() end)
        if ok2 then name = n end
    end

    local entry = string.format("#%d  %s", hitCount, name)
    table.insert(log, 1, { text = entry, ttl = ENTRY_TTL })
    if #log > MAX_ENTRIES then
        table.remove(log)
    end
end

function drawLog()
    local ww = Components:Window():getWidthRender()
    local x  = ww - 320
    local y  = 18

    tw:writeTextTTFAutoSize(x, y, "== Collision Log ==", C_HEAD, 0.50)
    y = y + 22

    for _, e in ipairs(log) do
        local ratio = e.ttl / ENTRY_TTL
        local c = (ratio > 0.4) and C_ENTRY or C_OLD
        tw:writeTextTTFAutoSize(x, y, e.text, c, 0.45)
        y = y + 18
    end
end
