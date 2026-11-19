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

// Círculos de suelo en una sola pasada (instancing): un quad unidad en XZ y, por instancia, centro+radio, color y grosor.
// Mismo resultado que GroundCircle.vs/.fs dibujando uno a uno (la posición lleva el mismo +0.05 en Y).
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec4 iPosRadius;   // x, y, z, radio
layout (location = 3) in vec4 iColor;       // r, g, b, a
layout (location = 4) in float iThickness;

uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
flat out vec4  vColor;
flat out float vThickness;
flat out float vWorldY;

void main()
{
    vec3 center = vec3(iPosRadius.x, iPosRadius.y + 0.05, iPosRadius.z);
    gl_Position = projection * view * vec4(center + aPos * iPosRadius.w, 1.0);
    TexCoord   = aUV;
    vColor     = iColor;
    vThickness = iThickness;
    vWorldY    = center.y;
}
