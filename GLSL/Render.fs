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
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, TexCoords);
        }

        for (int i = 0; i < numSpotLights; i++) {
            result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir, i, TexCoords);
        }
    }

    FragColor = vec4(result, texture(material.diffuse, TexCoords).a * alpha);
}