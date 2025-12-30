// ============================================================
// renderCommons.glsl - Código común de rendering
// ============================================================

#define NR_POINT_LIGHTS 64

// ============================================================
// ESTRUCTURAS
// ============================================================

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

// ============================================================
// UNIFORMS COMUNES
// ============================================================

uniform Material material;
uniform vec3 viewPos;
uniform DirLight dirLight;

uniform mat4 dirLightMatrix;
uniform sampler2D dirLightShadowMap;
uniform bool enableDirectionalLightShadowMapping;

uniform sampler2DArray shadowMapArray;
uniform int numShadowMaps;

uniform bool debugShadowMapping;
uniform float shadowIntensity;

uniform int numSpotLights;

// ============================================================
// UNIFORM BLOCKS
// ============================================================

layout (std140) uniform PointLightsBlock {
    PointLight pointLights[NR_POINT_LIGHTS];
};

layout (std140) uniform SpotLightsBlock {
    SpotLight spotLights[NR_POINT_LIGHTS];
};

layout (std140) uniform SpotLightsShadowMapDepthTexturesBlock {
    mat4 spotLightsMatrix[NR_POINT_LIGHTS];
};

// ============================================================
// FUNCIONES DE SHADOW CALCULATION
// ============================================================

float ShadowCalculationDir(vec3 fragPos, mat4 lightSpaceMatrix, vec3 normal, vec3 lightDir)
{
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
    projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

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

float ShadowCalculationSpot(vec3 fragPos, int lightIndex, vec3 normal, vec3 lightDir)
{
    if (lightIndex < 0 || lightIndex >= numShadowMaps) {
        return 0.0;
    }

    vec4 fragPosLightSpace = spotLightsMatrix[lightIndex] * vec4(fragPos, 1.0);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
    projCoords.y < 0.0 || projCoords.y > 1.0) {
        return 0.0;
    }

    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);

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

// ============================================================
// FUNCIONES DE CÁLCULO DE ILUMINACIÓN
// ============================================================

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 fragPos, vec2 texCoords)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, texCoords));

    float shadow = 0.0;
    if (enableDirectionalLightShadowMapping) {
        shadow = ShadowCalculationDir(fragPos, dirLightMatrix, normal, lightDir);
    }

    if (debugShadowMapping) {
        return mix(vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), shadow * shadowIntensity);
    }

    return (ambient + (1.0 - shadow * shadowIntensity) * (diffuse + specular));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec2 texCoords)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));

    vec3 ambient = light.ambient.xyz * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = light.diffuse.xyz * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular.xyz * spec * vec3(texture(material.specular, texCoords));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir,
int lightIndex, vec2 texCoords)
{
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));

    float theta = dot(lightDir, normalize(-light.direction.xyz));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient = light.ambient.xyz * vec3(texture(material.diffuse, texCoords));
    vec3 diffuse = light.diffuse.xyz * diff * vec3(texture(material.diffuse, texCoords));
    vec3 specular = light.specular.xyz * spec * vec3(texture(material.specular, texCoords));

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    float shadow = ShadowCalculationSpot(fragPos, lightIndex, normal, lightDir);

    if (debugShadowMapping) {
        return mix(vec3(1.0, 0.0, 0.0), vec3(0.0, 0.0, 1.0), shadow * shadowIntensity);
    }

    return (ambient + (1.0 - shadow * shadowIntensity) * (diffuse + specular));
}