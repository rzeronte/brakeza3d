#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;

out vec4 FragColor;

void main()
{
    vec4 color = texture(sceneTexture, TexCoords);

    // Column 3 (x: 0.5-0.75), Row top (y > 0.5) -> Blue
    if (TexCoords.x >= 0.5 && TexCoords.x < 0.75 && TexCoords.y > 0.5) {
        FragColor = vec4(color.r, color.g, 1.0, 1.0);
    } else {
        FragColor = color;
    }
}
