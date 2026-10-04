-- UIWidgetsDemo: minimal UIManager example (scene UITest, TutorialsProject).
-- Uses only the example widgets in assets/ui/:
--   exampleNestedWidget  panel that embeds one exampleCard   (data keys "card_<element>")
--   exampleArrayWidget   panel with a list of exampleCard     (count in "unitList", rows "<i>_<element>")
--   exampleCard          a single card, drawn at an explicit position with drawWidgetAtPos
-- Widgets with posX/posY in their JSON are placed by drawWidget; drawWidgetAtPos overrides it.

local AVATAR      = "../assets/images/me.png"
local ICON_MOUSE  = "../assets/icons/mouse.png"
local ICON_APP    = "../assets/icons/application.png"

local UNITS = {
    { name = "Scout",   subtitle = "Moving",    avatar = AVATAR,     maxHp = 80  },
    { name = "Builder", subtitle = "Working",   avatar = ICON_APP,   maxHp = 120 },
    { name = "Pointer", subtitle = "Idle",      avatar = ICON_MOUSE, maxHp = 60  },
}

local elapsed = 0.0

-- Animated health (0..maxHp) so the progress bars change colour ok -> mid -> low
local function animatedHp(maxHp, phase)
    local k = 0.5 + 0.5 * math.sin(elapsed * 0.8 + phase)
    return math.floor(maxHp * k + 0.5)
end

function onStart()
    elapsed = 0.0
end

function onUpdate()
    elapsed = elapsed + Brakeza:getDeltaTime()
    local render = Components:Render()

    -- 1) Nested widget: the embedded card reads the keys prefixed with its element id ("card_")
    render:drawWidget("exampleNestedWidget", {
        title         = { text = "NESTED WIDGET" },
        card_avatar   = { path = AVATAR },
        card_name     = { text = "Hero", color = Color.new(1.0, 0.8, 0.3, 1) },
        card_subtitle = { text = "Leader" },
        card_hpBar    = { value = animatedHp(100, 0.0), max = 100 },
    })

    -- 2) Array widget: "unitList" says how many rows; row i reads the keys prefixed with "<i>_"
    local data = {
        title    = { text = "ARRAY WIDGET" },
        unitList = { count = #UNITS },
    }
    for i, u in ipairs(UNITS) do
        local p = (i - 1) .. "_"
        local hp = animatedHp(u.maxHp, i * 1.7)
        data[p .. "avatar"]   = { path = u.avatar }
        data[p .. "name"]     = { text = u.name }
        data[p .. "subtitle"] = { text = u.subtitle }
        data[p .. "hpBar"]    = { value = hp, max = u.maxHp }
    end
    render:drawWidget("exampleArrayWidget", data)

    -- 3) Single card at an explicit pixel position (top-right area of the window)
    local win = Components:Window()
    render:drawWidgetAtPos("exampleCard", win:getWidth() - win:getWidth() * 0.1, 20, {
        avatar   = { path = ICON_APP },
        name     = { text = "Standalone" },
        subtitle = { text = "drawWidgetAtPos" },
        hpBar    = { value = animatedHp(50, 3.0), max = 50 },
    })
end

function onEnd() end
