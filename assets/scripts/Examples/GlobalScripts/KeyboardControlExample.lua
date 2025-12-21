function onStart()
end

function onUpdate()
    left = Components:Input():isClickLeft()

    if (left) then
        print("Click Left!")
    end

    if (Components:Input():isCharPressed("A")) then
        print("Key A pressed!")
    end

    if (Components:Input():isKeyEventDown()) then
        print("Key event down!")
    end

    if (Components:Input():isKeyEventUp()) then
        print("Key event up!")
    end
end

