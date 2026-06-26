#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;

out vec4 FragColor;

void main()
{
    vec4 color = texture(sceneTexture, TexCoords);

    // Column 4 (x: 0.75-1.0), Row bottom (y <= 0.5) -> Orange
    if (TexCoords.x >= 0.75 && TexCoords.y <= 0.5) {
        FragColor = vec4(1.0, 0.5, color.b, 1.0);
    } else {
        FragColor = color;
    }
}
