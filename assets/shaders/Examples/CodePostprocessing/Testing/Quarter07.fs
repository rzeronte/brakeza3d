#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;

out vec4 FragColor;

void main()
{
    vec4 color = texture(sceneTexture, TexCoords);

    // Column 3 (x: 0.5-0.75), Row bottom (y <= 0.5) -> White
    if (TexCoords.x >= 0.5 && TexCoords.x < 0.75 && TexCoords.y <= 0.5) {
        FragColor = vec4(0.5 - color.r, 0.5 - color.g, 0.5 - color.b, 1.0);
    } else {
        FragColor = color;
    }
}
