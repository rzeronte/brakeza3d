#version 330 core

in vec2 TexCoords;
in vec2 invScreenSize;

out vec4 FragColor;

uniform sampler2D sceneTexture;
uniform sampler2D depthTexture;

uniform float focalDistance;
uniform float focalRange;
uniform int blurRadius;
uniform float far_plane;
uniform float intensity;

float near_plane = 0.1f;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane));
}

void main()
{
    float depthValue = texture(depthTexture, TexCoords).r * intensity;
    float depth = LinearizeDepth(depthValue) / far_plane;

    // Calcular la diferencia de profundidad con respecto al plano de enfoque
    float depthDifference = abs(depth - focalDistance);

    // Calcular la cantidad de desenfoque en función de la diferencia de profundidad y el tamaño del desenfoque
    int blurAmount = int((depthDifference - focalRange) * blurRadius);
    blurAmount = clamp(blurAmount, 0, 6);

    // Inicializar la suma de colores y el peso total
    vec4 colorSum = vec4(0.0);
    float weightSum = 0.0;

    // Iterar sobre el área de desenfoque
    for (int dy = -blurAmount; dy <= blurAmount; ++dy) {
        for (int dx = -blurAmount; dx <= blurAmount; ++dx) {
            // Calcular las coordenadas del píxel vecino
            vec2 offsetTexCoord = TexCoords + vec2(float(dx), float(dy)) * invScreenSize;

            // Obtener la profundidad del píxel vecino desde el buffer de profundidad
            float thisDepth = texture(depthTexture, offsetTexCoord).r * intensity;
            thisDepth = LinearizeDepth(thisDepth) / far_plane;

            // Calcular la diferencia de profundidad y el peso del píxel
            float depthDifference = abs(depth - thisDepth);
            float weight = 1.0 / (1.0 + depthDifference * depthDifference * blurAmount);

            // Obtener el color del píxel vecino desde la textura de video
            vec4 neighborColor = texture(sceneTexture, offsetTexCoord);

            // Acumular el color ponderado y el peso
            colorSum += weight * neighborColor;
            weightSum += weight;
        }
    }

    // Normalizar el color acumulado por el peso total
    if (weightSum > 0.0) {
        colorSum /= weightSum;
    }

    // Asignar el color resultante al fragmento de salida
    FragColor = colorSum;
}