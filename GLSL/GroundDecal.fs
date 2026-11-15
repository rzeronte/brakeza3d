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

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D decalTexture;
uniform vec4 tintColor;

void main()
{
    vec4 tex = texture(decalTexture, TexCoord);
    if (tex.a < 0.01) discard;
    FragColor = tex * tintColor;
}
