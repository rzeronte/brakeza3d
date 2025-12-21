function onStart()
    print("Simple Day/Night Cycle - Start")

    CYCLE_DURATION = 30.0   -- 1 minuto = 1 día completo
    current_time = 0.25     -- Comienza en la mañana
    reduction = 1.25
end

function onUpdate()

   local deltaTime = Brakeza:getDeltaTime()

    render = Components:Render()

    -- Avanzar el tiempo
    current_time = current_time + (deltaTime / CYCLE_DURATION)
    if current_time >= 1.0 then current_time = 0.0 end


    -- Calcular intensidad de la luz (0 = noche, 1 = día)
    local sun_angle = current_time * 2.0 * math.pi
    local sun_height = math.sin(sun_angle)
    local intensity = (sun_height + 1.0) / 2.0  -- Remap de -1,1 a 0,1

    -- Aplicar iluminación
    render:setGlobalIlluminationAmbient(
        Vertex3D.new(
            (0.2 + intensity * 0.8) * reduction,
            (0.2 + intensity * 0.8) * reduction,
            (0.3 + intensity * 0.7) * reduction
        )
    )

    render:setGlobalIlluminationDiffuse(
        Vertex3D.new(
            (0.1 + intensity * 0.4) * reduction,  -- Rojo
            (0.1 + intensity * 0.4) * reduction,  -- Verde
            (0.15 + intensity * 0.45) * reduction -- Azul (ligeramente más azul)
        )
    )

    -- Dirección del sol
    local sunDirection = Vertex3D.new(
        math.cos(sun_angle),
        sun_height,
        0.0
    ):getNormalize()

    render:setGlobalIlluminationDirection(sunDirection)
end

function onEnd()
    print("Simple Day/Night Cycle - Detenido")
end