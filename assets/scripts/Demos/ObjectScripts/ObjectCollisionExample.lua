function onStart()
end

function onUpdate()
end

function onCollision(with)
    if with ~= nil then
        print("Script: Collision With: " .. with:getObject():getName())
        print("Script: Collision Source: " .. with:getSource())
        print("Script: Collision BoneIndexMapping: " .. with:getBoneIndexMapping())
    else
        print("Script: Collision with unknow object")
    end
end