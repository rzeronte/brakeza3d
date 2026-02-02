#version 330 core

in vec2 TexCoords;
uniform sampler2D sceneTexture;
uniform float intensity, et;

out vec4 FragColor;

vec2 heatDistortion(vec2 uv, float time, float intensity) {
    float strength = intensity * 0.01; // Ajusta la intensidad
    uv.x += clamp(sin(uv.y * 50.0 + time) * strength, -1, 1);

    return uv;
}

void main()
{
    vec2 distortedUV = heatDistortion(TexCoords, et, intensity); // Aplicar la distorsión

    // Obtener el color distorsionado
    vec4 color = texture(sceneTexture, distortedUV);

    // Mantener el alpha original de la textura
    FragColor = vec4(color.rgb, color.a);  // Respetar el canal alpha
}
