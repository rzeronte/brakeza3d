function onStart()
    print("Simple Day/Night Cycle - Start")

    CYCLE_DURATION = 30.0   -- 30 segundos = 1 día completo
    current_time = 0.25     -- Comienza en la mañana

    render = Components:Render()

    -- Guardar los valores originales de iluminación
    orig_ambient = render:getGlobalIlluminationAmbient()
    orig_diffuse = render:getGlobalIlluminationDiffuse()
    orig_direction = render:getGlobalIlluminationDirection()
end

function onUpdate()

    local deltaTime = Brakeza:getDeltaTime()

    render = Components:Render()

    -- Avanzar el tiempo (0.0 = medianoche, 0.5 = mediodía)
    current_time = current_time + (deltaTime / CYCLE_DURATION)
    if current_time >= 1.0 then current_time = 0.0 end

    -- Ángulo del sol: fase ajustada para que el pico sea en t=0.5 (mediodía)
    local sun_angle = (current_time - 0.25) * 2.0 * math.pi
    local sun_height = math.sin(sun_angle)

    -- Intensidad: solo positiva cuando el sol está sobre el horizonte
    local intensity = math.max(0.0, sun_height)

    -- Escalar ambient y diffuse originales por la intensidad del ciclo
    local night_factor = 0.1

    render:setGlobalIlluminationAmbient(
        Vertex3D.new(
            orig_ambient.x * (night_factor + intensity * (1.0 - night_factor)),
            orig_ambient.y * (night_factor + intensity * (1.0 - night_factor)),
            orig_ambient.z * (night_factor + intensity * (1.0 - night_factor))
        )
    )

    render:setGlobalIlluminationDiffuse(
        Vertex3D.new(
            orig_diffuse.x * intensity,
            orig_diffuse.y * intensity,
            orig_diffuse.z * intensity
        )
    )

    -- Dirección del sol: arco inclinado en 3D (este → cenital → oeste)
    local tilt = 0.4
    local sunDirection = Vertex3D.new(
        math.cos(sun_angle),
        -sun_height,
        math.sin(sun_angle) * tilt
    ):getNormalize()

    render:setGlobalIlluminationDirection(sunDirection)
end

function onEnd()
    print("Simple Day/Night Cycle - Detenido")

    -- Restaurar los valores originales
    render = Components:Render()
    render:setGlobalIlluminationAmbient(orig_ambient)
    render:setGlobalIlluminationDiffuse(orig_diffuse)
    render:setGlobalIlluminationDirection(orig_direction)
end
