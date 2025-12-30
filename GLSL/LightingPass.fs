#version 330 core

#include "Shared/RenderCommons.glsl"

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform int numPointLights;

void main()
{
    // Obtener datos del G-Buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 norm = texture(gNormal, TexCoords).rgb;

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir, FragPos, TexCoords);

    for (int i = 0; i < numPointLights; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, TexCoords);
    }

    for (int i = 0; i < numSpotLights; i++) {
        result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir, i, TexCoords);
    }

    FragColor = vec4(result, texture(material.diffuse, TexCoords).a);
}