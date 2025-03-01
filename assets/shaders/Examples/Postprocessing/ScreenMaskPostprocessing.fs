#version 330 core

in vec2 TexCoords;

uniform sampler2D noise;
uniform sampler2D sceneTexture;

uniform float et; // Tiempo acumulado

out vec4 FragColor;

void main()
{
    // Normalizar dt para que oscile entre 0.0 y 1.0
    float timeFactor = mod(et, 1);

    // Obtener la máscara de ruido
    float mask = texture(noise, TexCoords).r;
    vec4 sceneColor = texture(sceneTexture, TexCoords);

    // Aplicar la máscara con transición suave
    float alpha = smoothstep(timeFactor - 0.1, timeFactor + 0.1, mask);

    // Aplicar el efecto al color final
    FragColor = vec4(sceneColor.rgb, sceneColor.a * alpha);
}
