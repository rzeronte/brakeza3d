function onStart()
	timer = 0
end

function onUpdate()
	timer = timer + Brakeza:getDeltaTime()
	if timer >= frequency then
		timer = 0																-- reset timer
		mesh = ObjectFactory.Mesh3D("../assets/models/basic/cube.fbx", this:getPosition())
		mesh:setCollisionsEnabled(true)
		mesh:setMass(10.0)
		mesh:setLinearDamping(0)
		mesh:setAngularDamping(0)
		mesh:SetupRigidBodyCollider(CollisionShape.TRIANGLE_MESH_SHAPE); 			-- RigidBody
	end
end

function onCollision(with)
end


