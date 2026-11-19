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
flat in vec4  vColor;
flat in float vThickness;
flat in float vWorldY;
out vec4 FragColor;

uniform sampler2D gPosition;
uniform vec2      screenSize;
uniform bool      uFilled;

void main()
{
    // Igual que GroundCircle.fs: si algo de la escena por encima del plano del círculo lo tapa, no se pinta
    vec2 screenUV = gl_FragCoord.xy / screenSize;
    vec3 fragPos  = texture(gPosition, screenUV).rgb;
    if (dot(fragPos, fragPos) > 0.001 && fragPos.y > vWorldY + 2.0) {
        discard;
    }

    vec2  uv   = TexCoord * 2.0 - 1.0;
    float dist = length(uv);

    float alpha;
    if (uFilled) {
        alpha = 1.0 - smoothstep(0.2, 1.0, dist);
    } else {
        float center    = 0.85;
        float halfWidth = vThickness * 0.5;
        float fadeWidth = 0.10;
        float inner     = center - halfWidth;
        float outer     = center + halfWidth;
        alpha = (1.0 - smoothstep(outer, outer + fadeWidth, dist))
              * smoothstep(inner - fadeWidth, inner, dist);
    }

    if (alpha < 0.01) discard;
    FragColor = vec4(vColor.rgb, vColor.a * alpha);
}
