function onStart()
end

function onUpdate()
    left = componentsManager:getComponentInput():isClickLeft()

    if (left) then
        print("Click Left!")
    end

    if (componentsManager:getComponentInput():isCharPressed("A")) then
        print("Key A pressed!")
    end

    if (componentsManager:getComponentInput():isKeyEventDown()) then
        print("Key event down!")
    end

    if (componentsManager:getComponentInput():isKeyEventUp()) then
        print("Key event up!")
    end
end

