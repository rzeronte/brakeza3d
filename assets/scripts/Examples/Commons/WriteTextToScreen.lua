function onStart()
    textWriter = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
end

function onUpdate()
     textWriter:writeTTFCenterHorizontal(
        15,
        "hello world!",
        Color.new(0, 1, 0, 1),
        1.5
    )
    textWriter:writeTextTTF(100, 100, 100, 100, "hello!", Color.new(1, 0, 0, 1))
end

function onCollision(with)
end