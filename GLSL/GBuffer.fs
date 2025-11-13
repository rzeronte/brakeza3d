#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main()
{
    // Almacenar posición del fragmento
    gPosition = FragPos;

    // Almacenar normal
    gNormal = normalize(Normal);

    // Almacenar color difuso en RGB y especular (intensidad) en A
    gAlbedoSpec.rgb = texture(texture_diffuse, TexCoords).rgb;

    // Usar el mapa especular si está disponible; tomar el canal rojo como intensidad
    gAlbedoSpec.a = texture(texture_specular, TexCoords).r;
}