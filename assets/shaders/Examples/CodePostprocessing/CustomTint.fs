#version 330 core

in vec2 TexCoords;

uniform sampler2D sceneTexture;

// Values written every frame from Lua (see Examples/Basics/16_CustomShaderUpdate.lua), each between -1 and 1
uniform float r_factor;
uniform float g_factor;
uniform float a_factor;

out vec4 FragColor;

void main()
{
    vec4 color = texture(sceneTexture, TexCoords);

    // Tint: each factor moves one colour channel between 0.5 and 1.5 times its value
    vec3 tint = vec3(1.0) + 0.5 * vec3(r_factor, g_factor, a_factor);

    FragColor = vec4(color.rgb * tint, 1.0);
}
