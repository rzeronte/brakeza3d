#version 330 core

uniform sampler2D diffuse;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
    //FragColor = vec4(texture(diffuse, TexCoords).rgb, 1);
}
