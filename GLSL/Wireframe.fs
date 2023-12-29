#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
    // Dibuja el borde en blanco y el interior en rojo
    if (gl_FrontFacing)
    FragColor = vec4(1.0, 0.0, 0.0, 1.0); // Interior en rojo
    else
    FragColor = vec4(1.0, 1.0, 1.0, 1.0); // Borde en blanco
}
