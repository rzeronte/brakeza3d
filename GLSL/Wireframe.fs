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

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 color;

void main()
{
    FragColor = vec4(color, 1.0); // Interior en rojo

    // Dibuja el borde en blanco y el interior en rojo

    //if (gl_FrontFacing)
    //    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Interior en rojo
    //else
    //    FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Borde en blanco
}
