#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceModel; // ocupa locations 3,4,5,6 (4 vec4 por columna)
layout (location = 7) in vec3 aInstanceColor;
layout (location = 8) in ivec4 BoneIDs;
layout (location = 9) in vec4 Weights;

out vec3 vColor;

uniform mat4 model;
uniform vec3 color;
uniform bool useInstancing;

// Picking instanciado para unidades animadas (Fase 2, mismo mecanismo que GLSL/GBuffer.vs Fase 1):
// skinning en shader vía TBO indexado por gl_InstanceID, en vez del feedbackBuffer horneado
// por-instancia -- solo hace falta la posición (el picking no usa normales).
uniform bool useSkinning;
uniform int bonesPerInstance;
uniform samplerBuffer boneMatrices;

layout (std140) uniform CameraBlock {
    mat4 projection;
    mat4 view;
};

mat4 fetchBoneMatrix(int instanceIdx, int boneIdx)
{
    int base = (instanceIdx * bonesPerInstance + boneIdx) * 4;
    return mat4(
        texelFetch(boneMatrices, base + 0),
        texelFetch(boneMatrices, base + 1),
        texelFetch(boneMatrices, base + 2),
        texelFetch(boneMatrices, base + 3)
    );
}

void main()
{
    vec4 skinnedPos = aPos;

    if (useSkinning) {
        mat4 boneTransform = mat4(0.0);
        float totalWeight = 0.0;
        for (int i = 0; i < 4; i++) {
            if (Weights[i] > 0.0) {
                boneTransform += fetchBoneMatrix(gl_InstanceID, BoneIDs[i]) * Weights[i];
                totalWeight += Weights[i];
            }
        }
        boneTransform = totalWeight > 0.0 ? boneTransform / totalWeight : mat4(1.0);
        skinnedPos = boneTransform * aPos;
    }

    mat4 usedModel = useInstancing ? aInstanceModel : model;
    vColor = useInstancing ? aInstanceColor : color;
    gl_Position = projection * view * usedModel * skinnedPos;
}
