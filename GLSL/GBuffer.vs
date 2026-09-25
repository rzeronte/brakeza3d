#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceModel; // ocupa locations 3,4,5,6 (4 vec4 por columna)
layout (location = 7) in ivec4 BoneIDs;
layout (location = 8) in vec4 Weights;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform vec3 drawOffset;
uniform bool useInstancing;

// Fase de instancing con skinning: en vez de leer un feedbackBuffer horneado por-instancia
// (transform feedback, ver GLSL/BonesTransforms.vs), el propio vertex shader del G-Buffer aplica
// el skinning leyendo las matrices de huesos de esta instancia desde un TBO (samplerBuffer),
// indexado por gl_InstanceID -- misma matemática que BonesTransforms.vs, solo movida aquí para
// poder instanciar unidades animadas igual que ya se hace con Mesh3D estático.
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
    vec3 skinnedNormal = aNormal;

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
        skinnedNormal = mat3(transpose(inverse(mat3(boneTransform)))) * aNormal;
    }

    mat4 usedModel = useInstancing ? aInstanceModel : model;
    vec4 worldPos = usedModel * skinnedPos;
    FragPos = worldPos.xyz + drawOffset;
    Normal = mat3(transpose(inverse(usedModel))) * skinnedNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}