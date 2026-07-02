#version 330 core
in vec2 texCoord;
out vec4 pixelColor;
uniform sampler2D image;
void main()
{
    vec4 c = texture(image, texCoord);
    if (c.a < 0.01) discard;
    pixelColor = c;
}
