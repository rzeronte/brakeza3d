#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

#define NR_POINT_LIGHTS 64

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec4 position;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float constant;
    float linear;
    float quadratic;
    float padding;
};

struct SpotLight {
    vec4 position;
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

uniform sampler2D gPosition;
uniform sampler2D gNormal;

uniform Material material; // Two sampler2D

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform mat4 dirLightMatrix;
uniform sampler2D dirLightShadowMap;

uniform int numPointLights;
uniform int numSpotLights;

uniform sampler2DArray shadowMapArray;
uniform int numShadowMaps;

uniform bool debugShadowMapping;
uniform float shadowIntensity;
uniform bool enableDirectionalLightShadowMapping;

layout (std140) uniform PointLightsBlock                      { PointLight pointLights[NR_POINT_LIGHTS]; };
layout (std140) uniform SpotLightsBlock                       { SpotLight spotLights[NR_POINT_LIGHTS]; };
layout (std140) uniform SpotLightsShadowMapDepthTexturesBlock { mat4 spotLightsMatrix[NR_POINT_LIGHTS]; };

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 fragPos);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int lightIndex);

float ShadowCalculationDir(vec3 fragPos, mat4 lightSpaceMatrix, vec3 normal, vec3 lightDir);
float ShadowCalculationSpot(vec3 fragPos, int lightIndex, vec3 normal, vec3 lightDir);

void main()
{
    // Obtener datos del G-Buffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 norm = texture(gNormal, TexCoords).rgb;

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir, FragPos);

    for (int i = 0; i < numPointLights; i++) {
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
    }

    for (int i = 0; i < numSpotLights; i++) {
        result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir, i);
    }

    //result += CalcSpotLight(spotLight, norm, FragPos, viewDir, -1);

    FragColor = vec4(result, texture(material.diffuse, TexCoords).a * 1);
    //FragColor = vec4(FragPos * 0.1, 1.0);
    //FragColor = vec4(norm * 1.5 + 0.5, 1.0);
}

// Calcula las sombras para la LUZ DIRECCIONAL usando sampler2D
float ShadowCalculationDir(vec3 fragPos, mat4 lightSpaceMatrix, vec3 normal, vec3 lightDir)
{
    // Transformar fragmento al espacio de luz
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);

    // Realizar perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transformar a rango [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    // Si está fuera del frustum de la luz, no hay sombra
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    // Profundidad actual del fragmento
    float currentDepth = projCoords.z;

    // Bias dinámico basado en el ángulo entre normal y luz
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

    // PCF (Percentage Closer Filtering) para suavizar sombras
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(dirLightShadowMap, 0);

    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            float pcfDepth = texture(dirLightShadowMap, projCoords.xy + offset).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    return shadow;
}

// Calcula las sombras para SPOT LIGHTS usando sampler2DArray
float ShadowCalculationSpot(vec3 fragPos, int lightIndex, vec3 normal, vec3 lightDir)
{
    // Si no hay shadow map para esta luz o el índice es inválido
    if (lightIndex < 0 || lightIndex >= numShadowMaps) {
        return 0.0; // Sin sombra
    }

    // Transformar fragmento al espacio de luz
    vec4 fragPosLightSpace = spotLightsMatrix[lightIndex] * vec4(fragPos, 1.0);

    // Realizar perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // Transformar a rango [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    // Si está fuera del frustum de la luz, no hay sombra
    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
        projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    // Profundidad actual del fragmento
    float currentDepth = projCoords.z;

    // Bias dinámico basado en el ángulo entre normal y luz
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

    // PCF (Percentage Closer Filtering) para suavizar sombras
    float shadow = 0.0;
    vec2 texelSize = 1.0 / vec2(textureSize(shadowMapArray, 0).xy);

    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(x, y) * texelSize;
            float pcfDepth = texture(shadowMapArray, vec3(projCoords.xy + offset, float(lightIndex))).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }

    shadow /= 9.0;

    return shadow;
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    // Shadow calculation - usando dirLightShadowMap
    float shadow = 0;
    if (enableDirectionalLightShadowMapping) {
        shadow = ShadowCalculationDir(fragPos, dirLightMatrix, normal, lightDir);
    }

    if (debugShadowMapping) {
        return mix(vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), shadow * shadowIntensity);
    }

    return (ambient + (1.0 - shadow * shadowIntensity) * (diffuse + specular));
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results
    vec3 ambient = light.ambient.xyz * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse.xyz * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular.xyz * spec * vec3(texture(material.specular, TexCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int lightIndex)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction.xyz));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient.xyz * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse.xyz * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular.xyz * spec * vec3(texture(material.specular, TexCoords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    // Shadow calculation
    float shadow = ShadowCalculationSpot(fragPos, lightIndex, normal, lightDir);

    if (debugShadowMapping) {
        return mix(vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), shadow * shadowIntensity);
    }

    return (ambient + (1.0 - shadow * shadowIntensity) * (diffuse + specular));  // ✅ SUMA, no resta
}