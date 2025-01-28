function onStart()
    textWriter = TextWriter.create("../assets/fonts/Courier.ttf")
end

function onUpdate()
     textWriter:writeTTFCenterHorizontal(
        15,
        "si bueno no",
        Color.new(0, 1, 0, 1),
        1.5
    )
    textWriter:writeTextTTF(100, 100, 100, 100, "Hola", Color.new(1, 0, 0, 1))-- código a ejecutar en cada frame.
end

function onCollision(with)
end