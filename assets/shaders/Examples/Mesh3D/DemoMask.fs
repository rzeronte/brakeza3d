#version 330 core

uniform sampler2D diffuseTexture;
uniform sampler2D noiseTexture;
uniform float t;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
    // Obtener color base
    vec3 baseColor = texture(diffuseTexture, TexCoords).rgb;

    // Obtener la nube como máscara de disolución
    float noiseValue = texture(noiseTexture, TexCoords).r;

    // Controlar la disolución con el tiempo
    float dissolveThreshold = sin(t) * 0.5 + 0.5; // Oscila entre 0 y 1

    // Comparar con la textura de nubes para determinar transparencia
    float alpha = smoothstep(dissolveThreshold - 0.1, dissolveThreshold, noiseValue);

    // Efecto de borde brillante en la zona de disolución
    vec3 edgeColor = vec3(1.0, 0.5, 0.0); // Color naranja brillante
    float edge = smoothstep(dissolveThreshold, dissolveThreshold + 0.05, noiseValue);
    vec3 finalColor = mix(edgeColor, baseColor, edge);

    FragColor = vec4(finalColor, alpha);
}
