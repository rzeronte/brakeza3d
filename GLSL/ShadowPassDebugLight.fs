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

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D depthTexture;

void main()
{
    float depth = texture(depthTexture, TexCoords).r;  // ← Solo vec2, sin la coordenada z
    FragColor = vec4(vec3(depth), 1.0);
}