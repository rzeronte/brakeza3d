-- DemoBrowserTour.lua (TutorialsProject, only with --set demoBrowserTour=true)
-- Automatic check of DemoBrowser: loads every demo in order through the browser and holds on each
-- one. A Lua error in any demo stops the scripts, so a run that does not reach "[DemoTour] DONE"
-- points at the failing demo. At the end it opens the demo list.
--
-- The engine screenshot does not include the UI, so tools/run_demo_browser_tour.ps1 captures the
-- window from Windows every time this script prints "[DemoTour] ready <NN>".

local scripting
local dwell   = 4.0      -- seconds before "ready" (the demo settles)
local HOLD    = 2.5      -- seconds after "ready" (time for the external capture)
local target  = 1
local waited  = 0.0
local total   = 0
local ready   = false
local phase   = "demos"  -- demos | list | done
local order   = nil      -- --set demoTourTourOnly=true: only the tour playlist, as catalog positions (see TOUR_ORDER)
local pos     = 1
-- "tour" of demos.json (ShowRoom, Particles, Lights, PostProcessing, ShaderShowcase, Swarm, RainCubes, Projectile,
-- ThirdPerson, TwinStick) as positions in the full catalog
local TOUR_ORDER = { 17, 21, 18, 19, 20, 22, 23, 24, 13, 15, 25, 26, 27, 33, 34, 29, 30, 31, 32, 35, 28 }
local record  = false    -- --set demoTourRecord=true: tools/record_demo_tour.ps1 drives the pace (see readAck)

-- In record mode the recorder presses the keys of each demo and writes the number of the demo it has
-- finished in logs/demo_tour_ack.txt; the tour only moves on when it sees it (or after RECORD_TIMEOUT).
local RECORD_TIMEOUT = 240.0
local function readAck()
    local f = io.open("../logs/demo_tour_ack.txt", "r")
    if not f then return 0 end
    local n = tonumber(f:read("*l")) or 0
    f:close()
    return n
end

function onStart()
    scripting = Components:Scripting()
    order = Brakeza:getCliParam("demoTourTourOnly") == true and TOUR_ORDER or nil
    pos = 1
    target, waited, ready, phase = order and order[1] or 1, 0.0, false, "demos"
    local d = tonumber(Brakeza:getCliParam("demoTourDwell"))
    if d ~= nil and d > 0 then dwell = d end
    record = Brakeza:getCliParam("demoTourRecord") == true
    -- Scripts started AGAIN in mid-tour (a demo that stops and plays them from the editor): carry on with the demo
    -- that follows the last one the recorder finished, instead of restarting the tour from the first one
    local acked = record and order and readAck() or 0
    if acked > 0 then
        for i, n in ipairs(order) do
            if n == acked then pos = math.min(i + 1, #order); target = order[pos]; break end
        end
    end
    print("[DemoTour] start, " .. dwell .. " s per demo" .. (record and " (record mode)" or ""))
end

function onUpdate()
    if phase == "done" then return end
    if total == 0 then total = tonumber(scripting:getGlobalScriptVar("DemoBrowser", "demoCount")) or 0 end
    if total == 0 then return end
    local last = order and #order or total

    if phase == "list" then
        waited = waited + Brakeza:getDeltaTime()
        if waited >= (record and 9.0 or HOLD) then
            print("[DemoTour] DONE " .. last .. " demos")
            phase = "done"
            Brakeza:requestExit(0)
        end
        return
    end

    local busy    = scripting:getGlobalScriptVar("DemoBrowser", "browserBusy")
    local current = tonumber(scripting:getGlobalScriptVar("DemoBrowser", "currentDemo")) or 0
    if busy then waited = 0.0; return end
    if current ~= target then
        -- the browser starts in TOUR mode on the first tour demo: go to demo `target` of the full catalog
        waited = waited + Brakeza:getDeltaTime()
        if waited > 1.0 then
            waited = 0.0
            scripting:callGlobalScriptFunction("DemoBrowser", "goToDemo", target)
        end
        return
    end

    waited = waited + Brakeza:getDeltaTime()
    if not ready and waited >= dwell then
        print(string.format("[DemoTour] ready %02d (%d/%d)", target, order and pos or target, last))
        ready = true
    end
    local release = waited >= dwell + HOLD
    if record then release = ready and (readAck() == target or waited >= dwell + RECORD_TIMEOUT) end
    if ready and release then
        ready, waited = false, 0.0
        if (order and pos >= #order) or (not order and target >= total) then
            scripting:callGlobalScriptFunction("DemoBrowser", "showDemoList", true)
            print("[DemoTour] ready list")
            phase = "list"
            return
        end
        pos = pos + 1
        target = order and order[pos] or (target + 1)
        scripting:callGlobalScriptFunction("DemoBrowser", "goToDemo", target)
    end
end
