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

function onStart()
    scripting = Components:Scripting()
    target, waited, ready, phase = 1, 0.0, false, "demos"
    local d = tonumber(Brakeza:getCliParam("demoTourDwell"))
    if d ~= nil and d > 0 then dwell = d end
    print("[DemoTour] start, " .. dwell .. " s per demo")
end

function onUpdate()
    if phase == "done" then return end
    if total == 0 then total = tonumber(scripting:getGlobalScriptVar("DemoBrowser", "demoCount")) or 0 end
    if total == 0 then return end

    if phase == "list" then
        waited = waited + Brakeza:getDeltaTime()
        if waited >= HOLD then
            print("[DemoTour] DONE " .. total .. " demos")
            phase = "done"
            Brakeza:requestExit(0)
        end
        return
    end

    local busy    = scripting:getGlobalScriptVar("DemoBrowser", "browserBusy")
    local current = tonumber(scripting:getGlobalScriptVar("DemoBrowser", "currentDemo")) or 0
    if busy or current ~= target then waited = 0.0; return end

    waited = waited + Brakeza:getDeltaTime()
    if not ready and waited >= dwell then
        print(string.format("[DemoTour] ready %02d (%d/%d)", target, target, total))
        ready = true
    end
    if ready and waited >= dwell + HOLD then
        ready, waited = false, 0.0
        if target >= total then
            scripting:callGlobalScriptFunction("DemoBrowser", "showDemoList", true)
            print("[DemoTour] ready list")
            phase = "list"
            return
        end
        target = target + 1
        scripting:callGlobalScriptFunction("DemoBrowser", "goToDemo", target)
    end
end
