#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;

out vec4 FragColor;

void main()
{
    vec4 color = texture(sceneTexture, TexCoords);

    // Column 4 (x: 0.75-1.0), Row top (y > 0.5) -> Yellow
    if (TexCoords.x >= 0.75 && TexCoords.y > 0.5) {
        FragColor = vec4(1.0, 1.0, color.b, 1.0);
    } else {
        FragColor = color;
    }
}
