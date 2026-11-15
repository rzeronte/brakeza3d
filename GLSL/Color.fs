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

in vec3 vColor;

// 1.0 en el picking (RenderColor*, pasada 2 del contorno); < 1.0 en el tinte de submesh
// (ShaderOGLColor::RenderTint, mezclado con alpha sobre la escena).
uniform float alpha;

void main()
{
    FragColor = vec4(vColor, alpha);
}
