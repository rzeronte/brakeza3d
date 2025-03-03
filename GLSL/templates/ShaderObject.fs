#version 330 core

//uniform sampler2D diffuse;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
    //FragColor = texture(diffuse, TexCoords);
    FragColor = vec4(0, 0, 0, 0);
}
