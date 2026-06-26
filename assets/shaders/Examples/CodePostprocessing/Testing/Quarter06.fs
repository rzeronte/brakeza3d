#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;

out vec4 FragColor;

void main()
{
    vec4 color = texture(sceneTexture, TexCoords);

    // Column 2 (x: 0.25-0.5), Row bottom (y <= 0.5) -> Magenta
    if (TexCoords.x >= 0.25 && TexCoords.x < 0.5 && TexCoords.y <= 0.5) {
        FragColor = vec4(1.0, color.g, 1.0, 1.0);
    } else {
        FragColor = color;
    }
}
