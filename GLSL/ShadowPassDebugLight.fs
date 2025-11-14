#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2DArray shadowMapArray;
uniform int layer;

int nearPlane = 0;
int farPlane = 20;
float intensity = 1.0;


void main()
{
    float depth = texture(shadowMapArray, vec3(TexCoords, layer)).r;
    FragColor = vec4(vec3(depth), 1.0);

}