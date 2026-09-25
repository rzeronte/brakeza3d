#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceModel; // ocupa locations 3,4,5,6 (4 vec4 por columna)

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform bool useInstancing;

void main()
{
    mat4 usedModel = useInstancing ? aInstanceModel : model;
    gl_Position = lightSpaceMatrix * usedModel * aPos;
}