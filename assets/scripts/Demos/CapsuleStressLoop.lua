function onStart()
    timer = 0
    currentCol = 0
    currentRow = 0
    batchSize  = 5
end

function onUpdate()
    if finished > 0 then
        return
    end

    timer = timer + Brakeza:getDeltaTime()

    if timer >= frequency then
        timer = 0  -- Reiniciar temporizador

        -- Crear varios objetos de golpe (batch)
        for i = 1, batchSize do
            if currentRow >= gridRows then
                finished = 1
                print("Parrilla completada")
                break
            end

            -- Calcular posición en la parrilla
            local x = position.x + (currentCol * spacing)
            local y = position.y
            local z = position.z + (currentRow * spacing)

            -- Crear mesh en la posición calculada
            local pos = Vertex3D.new(x, y, z)
            ObjectFactory.Mesh3D("../assets/models/Capsule.fbx", pos)

            -- Avanzar a la siguiente posición
            currentCol = currentCol + 1

            if currentCol >= gridCols then
                currentCol = 0
                currentRow = currentRow + 1
            end
        end
    end
end