#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 tf_Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float executionTime;

void main()
{
    // Parámetros de la oscilación (onda)
    float frequency = 3.0;  // Número de oscilaciones por segundo
    float amplitude = 0.002;  // Desplazamiento máximo en el eje Y

    // Oscilación en el eje Y (usando seno para mover hacia arriba y hacia abajo)
    // Desplazamos el vértice en función de su posición en el eje X para crear una onda
    float offsetY = amplitude * sin(executionTime + aPos.x * frequency);

    // Aplicamos la oscilación solo al eje Y
    vec3 animatedPos = aPos.xyz;
    animatedPos.y += offsetY;

    // Guardamos la posición transformada en el espacio del modelo
    FragPos = vec3(model * vec4(animatedPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    // Calculamos la posición final en espacio de clip
    gl_Position = projection * view * vec4(FragPos, 1.0);

    // Guardamos la posición en el espacio local (antes de cualquier transformación)
    tf_Position = vec4(animatedPos, 1.0);  // Aquí guardamos la posición local
}
