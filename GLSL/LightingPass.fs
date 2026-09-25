#version 330 core

#include "Shared/RenderCommons.glsl"

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform int numPointLights;
// Emisión por-objeto (G-Buffer COLOR_ATTACHMENT3). hasEmission=false en frames sin Mesh3D
// emisivos: la textura ni se lee (su contenido es de un frame anterior, no se limpia).
uniform sampler2D gEmission;
uniform bool hasEmission;

void main()
{
    // Obtener datos del G-Buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 norm = texture(gNormal, TexCoords).rgb;

    vec3 viewDir = normalize(viewPos - FragPos);

    // gAlbedoSpec.a (leído aquí vía material.diffuse) es la intensidad especular por-objeto -- 1.0
    // por defecto para toda la escena (GLSL/GBuffer.fs), objetos con shader propio (p.ej. WaterRTS)
    // pueden bajarla para atenuar picos de luz direccional muy correlados en superficies grandes.
    // specIntensity=0.0 (agua, ver WaterRTS.fs) anula del todo el término specular -- que es
    // exactamente el que generaba la mancha ligada a la dirección del sol -- pero deja intactos
    // ambient+diffuse (y sombra), así que el agua sigue modelada por N·L en vez de ir sin iluminar.
    float specIntensity = texture(material.diffuse, TexCoords).a;

    vec3 result = CalcDirLight(dirLight, norm, viewDir, FragPos, TexCoords, specIntensity);

    for (int i = 0; i < numPointLights; i++) {
        float dist = distance(pointLights[i].position.xyz, FragPos);
        if (dist > pointLights[i].radius) continue;
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, TexCoords, specIntensity);
    }

    for (int i = 0; i < numSpotLights; i++) {
        float dist = distance(spotLights[i].position.xyz, FragPos);
        if (dist > spotLights[i].radius) continue;
        result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir, i, TexCoords, specIntensity);
    }

    // FragColor.a ya NO viaja acoplado a specIntensity (gAlbedoSpec.a): esta capa se compone sobre
    // el backgroundFBO (grid de referencia incluido) con GL_SRC_ALPHA en
    // ComponentWindow::FlipGlobalToWindow(), así que el alpha real es "¿hay geometría en este
    // píxel?" -- mismo sentinel (0,0,0) que GLSL/GroundCircle.fs -- no "cuánta intensidad
    // especular tiene". Un alpha=1.0 fijo aquí tapaba el fondo (y su grid) en TODO el frame.
    // Emisión sobre el diffuse: intensidad 1 = color de la textura tal cual, sin luces ni sombras.
    if (hasEmission) {
        vec4 e = texture(gEmission, TexCoords);
        result = mix(result, e.rgb, e.a);
    }

    bool hasGeometry = dot(FragPos, FragPos) > 0.001;
    FragColor = vec4(result, hasGeometry ? 1.0 : 0.0);
}