#version 330 core

out vec4 FragColor;

in vec3 vColor;

// 1.0 en el picking (RenderColor*, pasada 2 del contorno); < 1.0 en el tinte de submesh
// (ShaderOGLColor::RenderTint, mezclado con alpha sobre la escena).
uniform float alpha;

void main()
{
    FragColor = vec4(vColor, alpha);
}
