#version 330 core

#define MAX_BONES 100

layout (location = 0) in vec4 aPos;
layout (location = 1) in ivec4 BoneIDs;
layout (location = 2) in vec4 Weights;

uniform mat4 gBones[MAX_BONES];

out vec4 tf_Position;

void main()
{
	mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];

    tf_Position = BoneTransform * aPos;
}
