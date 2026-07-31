-- Prueba del UIManager
local testVisible = true

function onUpdate()
    if not testVisible then return end

    local render = Components:Render()
    local y = 80
    y = render:drawWidgetAtPos("unitCard", 10, y, {
        avatar  = { path = "../assets/images/rts/avatar.png" },
        name    = { text = "Test Archer", color = Color.new(0.65, 0.28, 0, 1) },
        state   = { text = "MOVING", color = Color.new(0, 0.52, 0.12, 1) },
        faction = { text = "PLAYER", color = Color.new(0.2, 0.88, 0.25, 1) },
        hp      = { value = 65, max = 100 },
        resources = { list = { "gold", "wood" } }
    })

    -- Segundo widget apilado
    y = render:drawWidgetAtPos("buildingCard", 10, y, {
        avatar  = { path = "../assets/images/rts/avatar.png" },
        bldType = { text = "TOWN HALL" },
        faction = { text = "ENEMY", color = Color.new(0.9, 0.18, 0.18, 1) },
        hp      = { value = 200, max = 200 }
    })
end

function onStart() end
function onEnd() end