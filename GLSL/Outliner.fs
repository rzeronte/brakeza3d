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
in vec2 texCoord;
out vec4 pixelColor;
uniform sampler2D image;
void main()
{
    vec4 c = texture(image, texCoord);
    if (c.a < 0.01) discard;
    pixelColor = c;
}
