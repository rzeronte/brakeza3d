function onStart()
    Components:Camera():setEnabled(true)			-- needed
    Components:Input():setEnabled(true)				-- needed

    Components:Input():setMouseEnabled(true)		-- mouse lookup on
    Components:Input():setKeyboardEnabled(false)	-- keyboard moving camera off
end

function onUpdate()
end

function onCollision(with)
end




