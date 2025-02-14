#version 330 core

uniform sampler2D diffuse;
uniform float alpha;

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    FragColor = texture(diffuse, TexCoords);
}
