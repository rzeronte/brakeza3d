#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;

out vec4 FragColor;

void main()
{
    vec4 color = texture(sceneTexture, TexCoords);

    // Column 1 (x: 0.0-0.25), Row top (y > 0.5) -> Red
    if (TexCoords.x < 0.25 && TexCoords.y > 0.5) {
        FragColor = vec4(1.0, color.g, color.b, 1.0);
    } else {
        FragColor = color;
    }
}
