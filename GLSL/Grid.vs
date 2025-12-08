#version 330 core

layout (location = 0) in vec4 vertex;

uniform mat4 model;
uniform mat4 projection;

out vec3 nearPoint;
out vec3 farPoint;

vec3 unprojectPoint(float x, float y, float z, mat4 model, mat4 projection) {
    mat4 viewInv = inverse(model);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint = viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
    vec2 pos = vertex.xy * 2.0 - 1.0;

    nearPoint = unprojectPoint(pos.x, pos.y, 0.0, model, projection);
    farPoint = unprojectPoint(pos.x, pos.y, 1.0, model, projection);

    gl_Position = vec4(pos, 0.0, 1.0);
}