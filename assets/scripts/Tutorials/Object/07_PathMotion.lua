-- ============================================================================
-- Tutorial 07: Path Motion
-- Type: Object Script (attach to any Object3D)
-- ============================================================================
-- Moves the object along a figure-eight around a center point, without input.
-- Used by the Swarm demo: the red cube is the swarm's predator, and moving it
-- through the flock shows the boids steering away from it.
-- ============================================================================

local t = 0

function onStart()
    t = 0
end

function onUpdate()
    t = t + Brakeza:getDeltaTime() * speed

    -- Figure-eight (Lissajous curve): x follows sin(t), z follows sin(2t)
    local x = centerX + radius * math.sin(t)
    local y = centerY + height * math.sin(t * 0.5)
    local z = centerZ + radius * 0.6 * math.sin(t * 2)

    this:setPosition(Vertex3D.new(x, y, z))
end
