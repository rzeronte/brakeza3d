#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture; // Textura base
uniform float progress; // Control de fade (0 = escena original, 1 = color final)
uniform vec3 color; // Color final

out vec4 FragColor;

void main()
{
    // Obtener el color base de la escena (RGB)
    vec3 sceneColor = texture(sceneTexture, TexCoords).rgb;

    // Obtener el alpha original de la textura
    float alpha = texture(sceneTexture, TexCoords).a;

    // Interpolar entre la textura y el color deseado en función de progress
    vec3 finalColor = mix(sceneColor, color, progress);

    // Salida final (mantener alpha original)
    FragColor = vec4(finalColor, alpha);
}
