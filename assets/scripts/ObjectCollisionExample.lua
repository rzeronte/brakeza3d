function onStart()
end

function onUpdate()
end

function onCollision(with)
    if with ~= nil then
        print("Collision with " .. with:getLabel())
    else
        print("Collision with unknow object")
    end
end