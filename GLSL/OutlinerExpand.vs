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
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float outlineThickness;

void main()
{
    // Pass 1: world-space normal expansion (good for oblique cameras and wall normals).
    mat3 normalMatrix = mat3(transpose(inverse(model)));
    vec3 worldNormal  = normalize(normalMatrix * aNormal);
    vec4 worldPos     = model * vec4(aPos.xyz, 1.0);
    worldPos.xyz     += worldNormal * outlineThickness;
    gl_Position       = projection * view * worldPos;

    // Pass 2: clip-space centroid expansion (guarantees a visible ring for any camera
    // angle, including pure top-down where face normals are parallel to the view ray).
    vec4 clipOrigin   = projection * view * model * vec4(0.0, 0.0, 0.0, 1.0);
    vec2 ndcPos       = gl_Position.xy / gl_Position.w;
    vec2 ndcOrigin    = clipOrigin.xy / clipOrigin.w;
    vec2 dir          = ndcPos - ndcOrigin;
    float len         = length(dir);
    if (len < 0.001) dir = vec2(1.0, 0.0);
    else             dir /= len;

    // Scale factor 0.03 converts world-unit thickness to approximate NDC units.
    // e.g. thickness=0.4 -> 0.012 NDC ~ 11 px on a 1920-wide render.
    gl_Position.xy   += dir * (outlineThickness * 0.03) * gl_Position.w;
}
