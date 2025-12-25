function onStart()
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf", 2)
    current = 0
    menuOptions = {"> Option #1", "> Option #2", "> Option #3", "> Option #4"}
    baseOffsetX = 300
    baseOffsetY = 300
    enabled = true
end

function onUpdate()

    input = Components:Input()

    if input:isCharPressed("ESCAPE") and input:isKeyEventDown() then
        enabled = not enabled
    end

    if not enabled then
        return
    end

    local lineSpacing = 50 -- Espaciado entre las líneas del menú

    for i, option in ipairs(menuOptions) do
        local offsetY = baseOffsetY + (i - 1) * lineSpacing
        local color = Color.new(1, 1, 1, 1) -- Blanco por defecto

        if i - 1 == current then
            color = Color.new(0, 1, 0, 1) -- Verde si está seleccionada
        end

        textWriter:writeTextTTFAutoSize(baseOffsetX, offsetY, option, color, 1.5)
    end

    if input:isCharPressed("UP") and input:isKeyEventDown() then
        current = current - 1
    end

    if input:isCharPressed("DOWN") and input:isKeyEventDown() then
        current = current + 1
    end

    if current < 0 then
        current = #menuOptions - 1
    elseif current >= #menuOptions then
        current = 0
    end
end

function onCollision(with)
end

