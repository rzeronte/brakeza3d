/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#version 330 core

// ============================================================
// GPU Particle Fragment Shader
// ============================================================
// particlecolor.rgb = interpolado entre colorFrom y colorTo
//                     según lifeRatio (viene del vertex shader)
// particlecolor.a   = alpha por partícula (valor constante,
//                     aleatorio [MIN_ALPHA..MAX_ALPHA]/255)
//
// Color final = textura × color_partícula
// Comportamiento idéntico a CPU (Particle.fs)
// ============================================================

in vec2 UV;
in vec4 particlecolor;

out vec4 color;

uniform sampler2D myTextureSampler;

void main()
{
    color = texture(myTextureSampler, UV) * particlecolor;
}
