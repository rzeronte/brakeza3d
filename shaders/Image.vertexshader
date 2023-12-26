#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;
uniform int inverse;

void main()
{
    vec2 UV = vec2(vertex.z, vertex.w);
    if (inverse > 0){
        UV = vec2(vertex.z, -vertex.w);
    }
    TexCoords = UV;
    gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);
}