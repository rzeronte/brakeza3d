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

layout (location = 0) in vec4 aPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 color;

out vec4 fragColor;

void main()
{
    gl_Position = projection * view * model * aPosition;

    gl_PointSize = 2.0f;
    fragColor = vec4(color, 1);
}
