#version 330 core

in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D depthTexture;

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
    float depthValue = texture(depthTexture, vec2(TexCoords.x, 1.0 - TexCoords.y)).r * intensity;
    FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0);
}
