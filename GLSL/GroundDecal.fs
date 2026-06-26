#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D decalTexture;
uniform vec4 tintColor;

void main()
{
    vec4 tex = texture(decalTexture, TexCoord);
    if (tex.a < 0.01) discard;
    FragColor = tex * tintColor;
}
