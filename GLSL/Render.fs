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

#include "Shared/RenderCommons.glsl"

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform int numLights;
uniform float alpha;
uniform bool enableLights;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir, FragPos, TexCoords);

    if (enableLights) {
        for (int i = 0; i < numLights; i++) {
            float dist = distance(pointLights[i].position.xyz, FragPos);
            if (dist > pointLights[i].radius) continue;
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, TexCoords);
        }

        for (int i = 0; i < numSpotLights; i++) {
            float dist = distance(spotLights[i].position.xyz, FragPos);
            if (dist > spotLights[i].radius) continue;
            result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir, i, TexCoords);
        }
    }

    FragColor = vec4(result, texture(material.diffuse, TexCoords).a * alpha);
}