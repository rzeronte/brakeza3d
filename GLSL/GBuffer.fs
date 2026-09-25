#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
// Solo llega a la textura en frames con Mesh3D emisivos (ComponentRender::FlushEmissiveQueue activa
// el 4º draw buffer); el resto del tiempo esta salida se descarta sin coste.
layout (location = 3) out vec4 gEmission;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform float emission;   // 0..1, Mesh3D::emissionIntensity (0 si la emisión está desactivada)

void main()
{
    // Almacenar posición del fragmento
    gPosition = FragPos;

    // Almacenar normal
    gNormal = normalize(Normal);

    // Almacenar color difuso en RGB y especular (intensidad) en A
    vec3 diffuse = texture(texture_diffuse, TexCoords).rgb;
    gAlbedoSpec.rgb = diffuse;

    // Usar el mapa especular si está disponible; tomar el canal rojo como intensidad
    gAlbedoSpec.a = 1.0; //texture(texture_specular, TexCoords).r;

    // Emisión sobre el diffuse: rgb = color emitido, a = intensidad (LightingPass.fs hace el mix)
    gEmission = vec4(diffuse, emission);
}
