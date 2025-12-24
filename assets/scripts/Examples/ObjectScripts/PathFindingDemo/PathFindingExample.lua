function onStart()
    eye = Mesh3D.create(Vertex3D.new(0, 0, 1), "../assets/models/eye.fbx")
    eye:buildGrid3D(10, 10, 10)
    Brakeza:addObject3D(eye, 'modelo')

    eye:fillGrid3DFromGeometry()
    eye:getGrid3D():setTravel(0, 0, 0, 5, 5, 5) -- Configuramos un viaje inicial hasta el centro
end

function onUpdate()
    local time = Brakeza:getExecutionTime()

    -- Oscilación entre 0 y 10
    local oscX = (math.cos(time) + 1) * 5  -- Oscila entre [0, 10] en X
    local oscY = (math.cos(time * 1.2) + 1) * 5  -- Oscila entre [0, 10] en Y
    local oscZ = (math.cos(time * 1.5) + 1) * 5  -- Oscila entre [0, 10] en Z

    -- Actualizamos el destino del viaje dinámicamente a modo de ejemplo
    eye:getGrid3D():setTravel(0, 0, 0, math.floor(oscX), math.floor(oscY), math.floor(oscZ))

    -- Obtener el path calculado mediante A*
    path = eye:getGrid3D():makeTravelCubesGrid()

    -- Ejemplo: Iteramos sobre el array path y pintamos los índices de cada CubeGrid3D
    for i, cube in ipairs(path) do
        print("Cube " .. i .. ": X = " .. cube.posX .. ", Y = " .. cube.posY .. ", Z = " .. cube.posZ)
    end
end

function onCollision(with)
end
