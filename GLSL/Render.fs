#version 330 core

#define NR_POINT_LIGHTS 64

out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

in vec3 Normal;
in vec2 TexCoords;

uniform Material material;
uniform float alpha;

void main()
{
    FragColor = vec4(vec3(texture(material.diffuse, TexCoords)), alpha);
}
