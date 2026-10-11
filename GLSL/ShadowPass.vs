#version 330 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceModel; // ocupa locations 3,4,5,6 (4 vec4 por columna)
layout (location = 7) in ivec4 BoneIDs;
layout (location = 8) in vec4 Weights;

uniform mat4 lightSpaceMatrix;
uniform mat4 model;
uniform bool useInstancing;

// Skinning de unidades animadas con las matrices de huesos de cada instancia en un TBO, indexado por
// gl_InstanceID -- misma matemática que GBuffer.vs (2026-10-10). Antes la sombra dibujaba el
// feedbackBuffer horneado de cada unidad, único por instancia: nunca se agrupaba (~2000 draw calls
// por frame con 200 unidades). Aquí solo hace falta la posición (pase de profundidad).
uniform bool useSkinning;
uniform int bonesPerInstance;
uniform samplerBuffer boneMatrices;

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
    vec4 pos = aPos;
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
        pos = boneTransform * aPos;
    }

    mat4 usedModel = useInstancing ? aInstanceModel : model;
    gl_Position = lightSpaceMatrix * usedModel * pos;
}
