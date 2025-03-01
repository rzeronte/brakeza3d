#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float executionTime;

void main()
{
    // Factor de oscilación: ajusta la frecuencia y amplitud
    float frequency = 2.0; // Número de oscilaciones por segundo
    float amplitude = 1.0; // Desplazamiento máximo

    // Calculamos la variación senoidal
    float offset = amplitude * sin(executionTime * frequency + aPos.x);

    // Aplicamos la oscilación en el eje Y
    vec3 animatedPos = aPos + vec3(0.0, offset, 0.0);

    FragPos = vec3(model * vec4(animatedPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}