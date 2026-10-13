-- KinematicThirdPersonController.lua
-- Controlador en tercera persona para un cuerpo KINEMATIC (cápsula de personaje) con cámara que lo sigue por detrás.
--
-- CONTROLES
--   W A S D                      moverse (relativo a hacia dónde mira el personaje)
--   Space                        saltar (se recuerda una pulsación justo antes de aterrizar)
--   botón izquierdo + arrastrar  girar el personaje y la cámara (movimiento horizontal del ratón)
--
-- Variables del script (KinematicThirdPersonController.json). Si alguna falta se usa el valor por defecto de aquí:
--   force             aceleración al andar (se aplica a 60 fps; se escala con los fps reales)
--   deceleration      factor de frenado por frame a 60 fps (0 = frena en seco, 1 = no frena)
--   jumpForce         impulso del salto
--   mouseSensibility  grados por píxel de ratón
--   airControl        fracción del movimiento que se conserva en el aire
--   cameraOffset      posición de la cámara respecto al personaje: x lateral, y altura, z distancia
-- Estado visible en el inspector: speed, strafe, velocity.

local JUMP_BUFFER_TIME = 0.12     -- s: una pulsación de salto se recuerda este tiempo hasta tocar suelo
local MAX_FRAME_DT     = 0.1      -- s: un tirón de frame no dispara la aceleración
local MAX_MOUSE_STEP   = 120      -- píxeles por frame: descarta saltos del ratón (p. ej. al recuperar el foco)

local input, camera
local jumpBuffer = 0.0
local yaw = 0.0                 -- hacia dónde mira el personaje (y la cámara, que lo sigue por detrás)

local function num(v, default)
    v = tonumber(v)
    if v == nil then return default end
    return v
end

local function clamp(v, lo, hi)
    return math.max(lo, math.min(hi, v))
end

-- Componentes XZ normalizadas de un vector: el movimiento es siempre horizontal
local function flat(v)
    local l = math.sqrt(v.x * v.x + v.z * v.z)
    if l < 0.0001 then return 0, 0 end
    return v.x / l, v.z / l
end

function onStart()
    input  = Components:Input()
    camera = Components:Camera():getCamera()
    speed, strafe = 0, 0
    jumpBuffer = 0.0
    yaw = 0.0
    this:setRotation(M3:getMatrixRotationForEulerAngles(0, yaw, 0):getTranspose())
end

-- Giro: arrastrar con el botón izquierdo gira el personaje alrededor del eje vertical (Y).
-- Se guarda el yaw acumulado y el cuerpo se orienta igual que en el controlador en primera persona
-- (la rotación del objeto es la traspuesta de la matriz de Euler).
local function turn()
    if not (input:isLeftMouseButtonPressed() and input:isMouseMotion()) then return end
    local dx = clamp(input:getMouseMotionXRel(), -MAX_MOUSE_STEP, MAX_MOUSE_STEP)
    yaw = yaw + dx * num(mouseSensibility, 0.1)
    this:setRotation(M3:getMatrixRotationForEulerAngles(0, yaw, 0):getTranspose())
end

-- La cámara se coloca detrás del personaje (cameraOffset: x lateral, y altura, z distancia) y mira a
-- su misma dirección, inclinada hacia abajo lo justo para tener al personaje en el centro.
-- (fx, fz) apunta hacia ATRÁS del personaje: W avanza en sentido contrario.
local function followCamera(pos, fx, fz, rx, rz)
    local off = cameraOffset
    local ox, oy, oz = 0, 2.5, 5
    if off ~= nil then ox, oy, oz = off.x, off.y, off.z end

    camera:setPosition(Vertex3D.new(pos.x + fx * oz + rx * ox, pos.y + oy, pos.z + fz * oz + rz * ox))

    local aimHeight = 0.5          -- punto del personaje al que apunta la cámara, sobre su origen
    local pitch = math.deg(math.atan2(math.max(oy - aimHeight, 0.0), math.max(oz, 0.1)))
    camera:setRotationFromEulerAngles(pitch, yaw, 0)
end

function onUpdate()
    local dt = clamp(Brakeza:getDeltaTime(), 0.0001, MAX_FRAME_DT)
    local frames60 = dt * 60.0      -- los valores de force/deceleration están pensados "por frame a 60 fps"

    turn()

    local pos = this:getPosition()
    local fx, fz = flat(this:AxisForward())
    local rx, rz = flat(this:AxisRight())

    followCamera(pos, fx, fz, rx, rz)

    -- Entrada de movimiento: ejes -1/0/1 (W adelante = negativo, como siempre en este controlador)
    local inForward, inStrafe = 0, 0
    if input:isCharPressed("W") then inForward = inForward - 1 end
    if input:isCharPressed("S") then inForward = inForward + 1 end
    if input:isCharPressed("A") then inStrafe  = inStrafe  - 1 end
    if input:isCharPressed("D") then inStrafe  = inStrafe  + 1 end

    -- En diagonal la suma de los dos ejes sería ×1,41: se normaliza
    if inForward ~= 0 and inStrafe ~= 0 then
        inForward = inForward * 0.70710678
        inStrafe  = inStrafe  * 0.70710678
    end

    -- Aceleración y frenado independientes de los fps
    local force = num(force, 0.15)
    local decel = clamp(num(deceleration, 0.1), 0.0, 1.0)
    speed  = (speed  + inForward * force * frames60) * (decel ^ frames60)
    strafe = (strafe + inStrafe  * force * frames60) * (decel ^ frames60)

    local walk = Vertex3D.new(fx * speed + rx * strafe, 0, fz * speed + rz * strafe)

    local onGround = this:onGround()
    if onGround then
        this:setWalkingDirection(walk)
    else
        this:setWalkingDirection(walk:getScaled(clamp(num(airControl, 0.25), 0.0, 1.0)))
    end

    -- Salto con buffer: pulsar un poco antes de aterrizar también salta
    if input:isCharFirstEventDown("SPACE") then
        jumpBuffer = JUMP_BUFFER_TIME
    else
        jumpBuffer = math.max(0.0, jumpBuffer - dt)
    end
    if jumpBuffer > 0.0 and onGround then
        this:Jump(Vertex3D.new(0, num(jumpForce, 10), 0))
        jumpBuffer = 0.0
    end
end

function onCollision(with)
end
