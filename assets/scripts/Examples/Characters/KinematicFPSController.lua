-- KinematicFPSController.lua
-- Controlador en primera persona para un cuerpo KINEMATIC (cápsula de personaje).
--
-- CONTROLES
--   W A S D                      moverse
--   Space                        saltar (se recuerda una pulsación justo antes de aterrizar)
--   botón izquierdo + arrastrar  mirar
--   Tab                          mirar libremente sin pulsar nada: el ratón queda capturado (cursor oculto);
--                                Tab o Escape lo devuelven. Con el ratón capturado se ve cómo gira todo el escenario.
--
-- Variables del script (KinematicFPSController.json). Si alguna falta se usa el valor por defecto de aquí:
--   force             aceleración al andar (se aplica a 60 fps; se escala con los fps reales)
--   deceleration      factor de frenado por frame a 60 fps (0 = frena en seco, 1 = no frena)
--   jumpForce         impulso del salto
--   mouseSensibility  grados por píxel de ratón
--   eyeHeight         altura de la cámara sobre el origen de la cápsula (los ojos, no el centro)
--   airControl        fracción del movimiento que se conserva en el aire
--   bobAmount         cabeceo al andar: oscilación de la cámara en unidades (0 = sin cabeceo)
--   bobFrequency      pasos por segundo del cabeceo a velocidad máxima
--   hideBody          true: la cápsula no se dibuja (si no, al moverse se ve desde dentro)
-- Estado visible en el inspector: speed, strafe, yaw, pitch.

local JUMP_BUFFER_TIME = 0.12     -- s: una pulsación de salto se recuerda este tiempo hasta tocar suelo
local MAX_FRAME_DT     = 0.1      -- s: un tirón de frame no dispara la aceleración
local MAX_MOUSE_STEP   = 120      -- píxeles por frame: descarta saltos del ratón (p. ej. al recuperar el foco)
local PITCH_LIMIT      = 89

local input, camera, tw
local jumpBuffer = 0.0
local freeLook   = false          -- mirar sin mantener el botón
local hasCapture = false          -- el motor permite capturar el ratón (Components:Input():setRelativeMouseMode)
local bobPhase = 0.0          -- fase del cabeceo
local bobBlend = 0.0          -- 0..1: cuánto cabeceo se aplica (se funde al empezar/parar)
local yawOnlyBody = false         -- true si se puede dejar el cuerpo solo con el yaw (si no, copia la rotación de la cámara)

local function num(v, default)
    v = tonumber(v)
    if v == nil then return default end
    return v
end

local function clamp(v, lo, hi)
    return math.max(lo, math.min(hi, v))
end

-- Componentes XZ normalizadas de un vector: el movimiento es siempre horizontal, mires hacia donde mires
local function flat(v)
    local l = math.sqrt(v.x * v.x + v.z * v.z)
    if l < 0.0001 then return 0, 0 end
    return v.x / l, v.z / l
end

local function setFreeLook(on)
    freeLook = on
    -- Capturado, el cursor no se sale de la ventana: el giro no se acaba al llegar al borde de la pantalla
    if hasCapture then input:setRelativeMouseMode(on) end
end

function onStart()
    input  = Components:Input()
    camera = Components:Camera():getCamera()
    tw     = ObjectFactory.TextWriter("../assets/fonts/Courier.ttf")
    -- Arrastrar con el botón izquierdo mira/apunta: el recuadro de selección del editor sobraría
    pcall(function() Components:Render():setDragSelectEnabled(false) end)

    -- Un exe anterior no tiene la captura de ratón: Tab sigue activando mirar sin botón, pero con el cursor visible
    hasCapture = pcall(function() return input:isRelativeMouseMode() end)
    freeLook = false

    speed, strafe, yaw, pitch = 0, 0, 0, 0
    jumpBuffer = 0.0
    bobPhase, bobBlend = 0.0, 0.0

    -- Los ojos están dentro de la cápsula: se oculta su malla (el collider sigue funcionando)
    if hideBody ~= false then
        pcall(function() this:setAlphaEnabled(true); this:setAlpha(0.0) end)
    end

    -- El cuerpo (cápsula) solo debe girar en yaw; si el binding no está disponible se usa la rotación completa de la cámara
    yawOnlyBody = pcall(function() return M3.getMatrixRotationForEulerAngles(0, 0, 0):getTranspose() end)
end

-- Cabeceo al andar: devuelve el desplazamiento vertical y lateral de la cámara.
-- Dos subidas y bajadas por ciclo (un paso con cada pie) y un leve balanceo lateral.
local function bobOffset(dt, onGround)
    local amount = num(bobAmount, 0.05)
    if amount <= 0 then
        bobBlend = 0.0
        return 0.0, 0.0
    end
    local f = num(force, 0.15)
    local d = clamp(num(deceleration, 0.1), 0.0, 0.95)
    local top = math.max(f * d / (1.0 - d), 0.0001)         -- velocidad de régimen por frame a 60 fps
    local ratio = clamp(math.sqrt(speed * speed + strafe * strafe) / top, 0.0, 1.0)

    local target = (onGround and ratio > 0.1) and ratio or 0.0
    bobBlend = bobBlend + (target - bobBlend) * math.min(1.0, dt * 10.0)
    bobPhase = bobPhase + dt * num(bobFrequency, 2.0) * 2.0 * math.pi * (0.5 + 0.5 * ratio)

    return math.sin(bobPhase * 2.0) * amount * bobBlend, math.cos(bobPhase) * amount * 0.5 * bobBlend
end

local function look()
    local sensibility = num(mouseSensibility, 0.1)
    local dx, dy = 0, 0
    if (freeLook or input:isLeftMouseButtonPressed()) and input:isMouseMotion() then
        dx = clamp(input:getMouseMotionXRel(), -MAX_MOUSE_STEP, MAX_MOUSE_STEP)
        dy = clamp(input:getMouseMotionYRel(), -MAX_MOUSE_STEP, MAX_MOUSE_STEP)
    end

    yaw   = yaw + dx * sensibility
    pitch = clamp(pitch + dy * sensibility, -PITCH_LIMIT, PITCH_LIMIT)

    camera:setRotationFromEulerAngles(pitch, yaw, 0)

    if yawOnlyBody then
        this:setRotation(M3.getMatrixRotationForEulerAngles(0, yaw, 0):getTranspose())
    else
        this:setRotation(camera:getRotation():getTranspose())
    end
end

local drawHint

function onUpdate()
    drawHint()

    -- Tab alterna mirar libremente; Escape siempre lo suelta
    if input:isCharFirstEventDown("Tab") then
        setFreeLook(not freeLook)
    elseif freeLook and input:isCharFirstEventDown("Escape") then
        setFreeLook(false)
    end

    local dt = clamp(Brakeza:getDeltaTime(), 0.0001, MAX_FRAME_DT)
    local frames60 = dt * 60.0      -- los valores de force/deceleration están pensados "por frame a 60 fps"

    -- La cámara sigue a la cápsula a la altura de los ojos
    local p = this:getPosition()
    local bobY, bobSide = bobOffset(dt, this:onGround())
    local brx, brz = flat(this:AxisRight())
    camera:setPosition(Vertex3D.new(p.x + brx * bobSide, p.y + num(eyeHeight, 0.7) + bobY, p.z + brz * bobSide))

    look()

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

    -- Dirección horizontal: sin la inclinación de la cámara, para que mirar arriba o abajo no cambie la velocidad
    local fx, fz = flat(this:AxisForward())
    local rx, rz = flat(this:AxisRight())
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

-- Pista en pantalla. Se dibuja desde onUpdate: el postUpdate de un script de objeto se congela si el objeto sale de cámara
drawHint = function()
    if tw == nil then return end
    pcall(function()
        tw:writeTextTTFAutoSize(20, 20, freeLook and "Free look ON   (Tab / Esc: release the mouse)"
                                                  or "Tab: free look with the mouse   (or hold the left button and drag)",
            Color.new(0.85, 0.85, 0.9, 1.0), 0.5)
    end)
end

function onEnd()
    -- Soltar el ratón al salir: si no, quedaría capturado en la siguiente demo
    if freeLook then setFreeLook(false) end
    pcall(function() Components:Render():setDragSelectEnabled(true) end)   -- devolver la selección con recuadro
    pcall(function() this:setAlpha(1.0); this:setAlphaEnabled(false) end)
end

function onCollision(with)
end
