#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D image;
uniform vec2 offset;
uniform float alpha;

float intensity = 0.75f;

void main()
{
    vec2 center = vec2(0.5f, 0.5f);
    vec2 offsetToCenter = vec2(0.40f, 0.40f); // center screen

    // Ajusta la velocidad de cambio con smoothstep para hacer el movimiento más suave al principio
    vec2 offsetInput = offset * intensity;

    vec2 st = TexCoords / 1.25;
    st += center - offsetToCenter + offsetInput;

    FragColor = vec4(vec3(1), alpha) * texture(image, st);
}
