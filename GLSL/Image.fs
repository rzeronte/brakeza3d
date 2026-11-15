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

uniform sampler2D image;
uniform float alpha;
uniform vec4 tintColor;

out vec4 color;

void main()
{
    // tintColor.a también cuenta: el texto de los widgets lleva ahí su transparencia (fundidos vía
    // UIManager::globalAlpha, p.ej. números de daño o tooltips). Las imágenes pasan siempre a=1.
    color = vec4(tintColor.rgb, alpha * tintColor.a) * texture(image, TexCoords);
}