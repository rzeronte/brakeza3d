#version 330 core

// ============================================================
// GPU Particle Vertex Shader (instanced billboard)
// ============================================================
// Simula partículas en CPU (OCParticle[]), renderiza con
// glDrawArraysInstanced + glVertexAttribDivisor(1).
// Cada instancia = una partícula.
//
// Atributos por instancia (divisor = 1):
//   loc 0: squareVertices   — quad del billboard [-0.5, 0.5]
//   loc 1: posSize          — .xyz = centro mundo, .w = tamaño partícula
//   loc 2: inTimeToLive     — vida máxima (OCParticle.timeToLive)
//   loc 3: inTimeLiving     — edad actual (OCParticle.timeLiving)
//   loc 4: inEnabled        — 1 = viva, 0 = muerta (OCParticle.active)
//   loc 5: inAlpha          — alpha por partícula, aleatorio [MIN_ALPHA..MAX_ALPHA]/255
//                             (OCParticle.force, constante durante su vida)
//
// Uniforms (iguales para todas las partículas):
//   colorFrom / colorTo     — colores interpolados desde ParticlesContext
//   CameraRight/Up          — vectores cámara para billboard
//   VP                      — proyección × vista
// ============================================================

layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec4 posSize;
layout(location = 2) in float inTimeToLive;
layout(location = 3) in float inTimeLiving;
layout(location = 4) in float inEnabled;
layout(location = 5) in float inAlpha;
layout(location = 6) in float inAngle;   // rotación en el plano de la pantalla (OCParticle.rotation.x, radianes)

uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP;
uniform vec4 colorFrom;
uniform vec4 colorTo;
uniform float sizeStart;   // multiplicador del tamaño al nacer (ParticlesContext::SIZE_START)
uniform float sizeEnd;     // multiplicador del tamaño al morir (ParticlesContext::SIZE_END)
// 0 = billboard (original). Distinto de 0: el quad se queda en el plano generado por flatRight/flatUp (no mira a la cámara)
uniform int  orientation;
uniform vec3 flatRight;
uniform vec3 flatUp;
uniform vec3 cameraPos;   // posición de la cámara en el mundo (solo VERTICAL_BILLBOARD)
// Balanceo de las partículas planas (orientation != 0); ver Particle.vs. flutterAngle = 0 → sin balanceo
uniform float flutterAngle;
uniform float flutterSpeed;

out vec2 UV;
out vec4 particlecolor;

void main()
{
    // Partículas muertas → fuera del viewport (no se rasterizan)
    if (inEnabled < 0.5) {
        gl_Position  = vec4(2.0, 2.0, 2.0, 1.0);
        UV           = vec2(0.0);
        particlecolor = vec4(0.0);
        return;
    }

    // lifeRatio = 1.0 al nacer, 0.0 al morir
    float lifeRatio = clamp(1.0 - inTimeLiving / inTimeToLive, 0.0, 1.0);

    // RGB: fade de colorFrom → colorTo durante la vida
    // Fórmula: mix(colorTo, colorFrom, t) = colorFrom·t + colorTo·(1-t)
    // Es la MISMA que CPU: colorFrom·lifeRatio + colorTo·(1-lifeRatio)
    particlecolor.rgb = mix(colorTo.rgb, colorFrom.rgb, lifeRatio);

    // Alpha: aleatorio al nacer (MIN_ALPHA..MAX_ALPHA / 255) y se desvanece con la vida,
    // igual que en CPU (ParticleEmitter::Draw: a = alpha de nacimiento * lifeRatio)
    particlecolor.a = inAlpha * lifeRatio;

    // Billboard: quad que siempre mira a la cámara
    float particleSize = posSize.w * mix(sizeEnd, sizeStart, lifeRatio);
    vec3  center       = posSize.xyz;
    // Giro del quad sobre sí mismo antes de orientarlo a la cámara (UV sin girar: gira la textura)
    float c = cos(inAngle), s = sin(inAngle);
    vec2 corner = vec2(c * squareVertices.x - s * squareVertices.y,
                       s * squareVertices.x + c * squareVertices.y);
    vec3 axisRight = CameraRight_worldspace;
    vec3 axisUp    = CameraUp_worldspace;
    if (orientation == 3) {
        // VERTICAL_BILLBOARD: el eje largo del quad sigue la vertical del mundo proyectada sobre el plano de la pantalla y el
        // ancho va perpendicular: siempre mira a la cámara (nunca de canto, ni con vista cenital). Se acorta con la
        // perspectiva (vista cenital = casi un punto, como una gota que cae hacia la cámara), con un mínimo para que no desaparezca
        // dirección REAL de esta gota a la cámara (no la del eje de la cámara): con perspectiva la trayectoria en pantalla
        // se abre desde el punto de fuga y la raya tiene que seguirla, como un proyectil
        vec3 toCam = normalize(cameraPos - center);
        vec3 pr    = vec3(0.0, 1.0, 0.0) - toCam * toCam.y;
        float len  = length(pr);
        vec3 vUp   = len > 1e-4 ? pr / len : CameraUp_worldspace;
        axisRight  = normalize(cross(toCam, vUp));
        axisUp     = vUp * (0.5 + 0.5 * len);
    } else if (orientation != 0) {
        axisRight = flatRight;
        axisUp    = flatUp;
    }
    vec3 offset = axisRight * corner.x + axisUp * corner.y;
    if (orientation != 0 && orientation != 3 && flutterAngle > 0.0) {
        // Fase = ángulo de la partícula: continuo y propio de cada una (nace aleatorio, gira a su velocidad). No usar
        // gl_InstanceID: en la ruta CPU el orden del buffer cambia cada frame (SortParticles) y la fase saltaría
        float amp = radians(flutterAngle);
        float a1 = sin(inAngle * flutterSpeed) * amp;
        float a2 = sin(inAngle * flutterSpeed * 0.77 + 1.7) * amp;
        vec3 n = normalize(cross(flatRight, flatUp));
        offset = axisRight * corner.x * cos(a1) + axisUp * corner.y * cos(a2)
               + n * (corner.x * sin(a1) + corner.y * sin(a2));
    }
    vec3 worldPos = center + offset * particleSize;

    gl_Position = VP * vec4(worldPos, 1.0);

    // UV: squareVertices.xy ∈ [-0.5, 0.5] → [0, 1]
    UV = squareVertices.xy + vec2(0.5, 0.5);
}
