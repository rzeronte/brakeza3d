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

uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 VP;
uniform vec4 colorFrom;
uniform vec4 colorTo;

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

    // Alpha: valor fijo por partícula (aleatorio al nacer, constante)
    // = Tools::random(MIN_ALPHA, MAX_ALPHA) / 255.0f
    // Comportamiento idéntico a CPU
    particlecolor.a = inAlpha;

    // Billboard: quad que siempre mira a la cámara
    float particleSize = posSize.w;
    vec3  center       = posSize.xyz;
    vec3 worldPos = center
        + CameraRight_worldspace * squareVertices.x * particleSize
        + CameraUp_worldspace    * squareVertices.y * particleSize;

    gl_Position = VP * vec4(worldPos, 1.0);

    // UV: squareVertices.xy ∈ [-0.5, 0.5] → [0, 1]
    UV = squareVertices.xy + vec2(0.5, 0.5);
}
