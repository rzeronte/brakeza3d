#version 330 core

#define MAX_BONES 100

layout (location = 0) in vec4 aPos;
layout (location = 1) in ivec4 BoneIDs;
layout (location = 2) in vec4 Weights;
layout (location = 3) in vec3 aNormal;

uniform mat4 gBones[MAX_BONES];

out vec4 tf_Position;
out vec3 tf_Normal;

void main()
{
    mat4 BoneTransform = mat4(0.0);

    float totalWeight = 0.0;

    for (int i = 0; i < 4; i++){
        if (Weights[i] > 0.0) {
            BoneTransform += gBones[BoneIDs[i]] * Weights[i];
            totalWeight += Weights[i];
        }
    }

    if (totalWeight > 0.0) {
        BoneTransform /= totalWeight;
    } else {
        BoneTransform = mat4(1.0);
    }

    tf_Position = BoneTransform * aPos;
    tf_Normal = mat3(transpose(inverse(mat3(BoneTransform))))* aNormal;
}