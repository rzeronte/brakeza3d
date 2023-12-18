#version 330 core

in vec4 fragColor;
in float drawParticle;

out vec4 finalColor;

void main()
{
    if (drawParticle < 1) discard;

    finalColor = fragColor;
}