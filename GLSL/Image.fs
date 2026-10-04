#version 330 core

in vec2 TexCoords;

uniform sampler2D image;
uniform float alpha;
uniform vec4 tintColor;

out vec4 color;

void main()
{
    // tintColor.a también cuenta: el texto de los widgets lleva ahí su transparencia (fundidos vía
    // UIManager::globalAlpha, p.ej. números de daño o tooltips). Las imágenes pasan siempre a=1.
    color = vec4(tintColor.rgb, alpha * tintColor.a) * texture(image, TexCoords);
}