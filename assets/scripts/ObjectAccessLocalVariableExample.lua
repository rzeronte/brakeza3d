function onStart()
end

function onUpdate()
    scale = this:getScale()
    if scale > 1 and state == 1 then
        print("Increasing scale > 1 times: " .. count)
        state = 0
        count = count + 1
    end
    if scale <= 1 then
        state = 1
    end

    offset.x = offset.x + 1
    offset.y = offset.y + 1
    offset.z = offset.z + 1
end