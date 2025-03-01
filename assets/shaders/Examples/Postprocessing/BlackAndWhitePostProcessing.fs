
#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;
uniform float intensity; // 0.0 = color original, 1.0 = completamente en blanco y negro

out vec4 FragColor;

void main()
{
    vec4 color = texture(sceneTexture, TexCoords);
    
    // Fórmula de luminancia ponderada (mejor percepción humana)
    float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
    
    // Interpolación entre el color original y la escala de grises según la intensidad
    vec3 finalColor = mix(color.rgb, vec3(gray), intensity);
    
    FragColor = vec4(finalColor, color.a);
}

