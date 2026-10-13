-- ============================================================================
-- RainCubesDemo
-- Type: Object Script (attach to any object: it is the spawn point)
-- ============================================================================
-- Every `frequency` seconds it creates a cube with a rigid body at the position of the object that owns the script.
-- The cubes fall, pile up and collide. Only the newest `maxCubes` of each emitter are kept (the oldest is removed),
-- otherwise the physics world would grow without limit.
--
-- Variables: frequency (s between cubes), maxCubes (cubes kept per emitter, 30 if the variable is missing).
-- Physics from a script: object:setCollisionsEnabled(true), setMass, SetupRigidBodyCollider(shape).
-- ============================================================================

local spawned = {}

function onStart()
	timer = 0
	spawned = {}
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

		-- keep only the newest maxCubes
		spawned[#spawned + 1] = mesh
		local limit = tonumber(maxCubes) or 30
		while #spawned > limit do
			local oldest = table.remove(spawned, 1)
			pcall(function() oldest:setRemoved(true) end)
		end
	end
end

function onCollision(with)
end


