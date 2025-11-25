//
// Created by Eduardo on 30/03/2025.
//

#ifndef BRAKEZA3D_ASSIMPANIMATIONSERVICE_H
#define BRAKEZA3D_ASSIMPANIMATIONSERVICE_H

#include "../3D/Mesh3DAnimation.h"

class AssimpAnimationService
{
    int indexCurrentAnimation = 0;
    float runningTime = 0;
    bool removeAtAnimationEnd = false;
    float animation_speed = 1;
    bool animation_ends = false;
    int numBones = 0;
    bool loop = true;

    Assimp::Importer importer;
    const aiScene *scene = nullptr;

    aiMatrix4x4 globalInverseTransform;

    std::vector<std::vector<VertexBoneData>> meshVerticesBoneData;  // mesh[] > vertex[] > VertexBoneData
    std::vector<std::vector<Vertex3D>> meshVertices;                // mesh[] > vertices

    std::map<std::string, unsigned int> boneMapping;                // maps a bone's name to its index
    std::vector<BoneInfo> boneInfo;                                 // Bone info and final transformation

public:
    AssimpAnimationService();
    ~AssimpAnimationService();

    void UpdateFrameTransformations();
    bool AssimpLoadAnimation(const std::string &filename, std::vector<Mesh3DData> &meshes);
    void ProcessNodeAnimation(aiNode *node, std::vector<Mesh3DData> &meshes);
    void ProcessMeshAnimation(int i, aiMesh *mesh, std::vector<Mesh3DData> &meshes);
    void ReadNodesFromRoot(std::vector<Mesh3DData> &meshes);
    void UpdateBonesFinalTransformations(float TimeInSeconds);
    void ReadNodeHierarchy(float AnimationTime, const aiNode *pNode, const aiMatrix4x4 &ParentTransform);
    static const aiNodeAnim *FindNodeAnim(const aiAnimation *pAnimation, const std::string& NodeName);
    static unsigned int FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim);
    static unsigned int FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim);
    static unsigned int FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim);
    void updateForBone(Vertex3D &dest, int meshID, int vertexID);
    void LoadMeshVertex(int meshId, aiMesh *mesh, std::vector<Vertex3D> &meshVertex, std::vector<Vertex3D> &meshNormals);
    void LoadMeshBones(int meshId, aiMesh *mesh, std::vector<VertexBoneData> &meshVertexBoneData);
    static void CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);
    static void CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);
    static void CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);
    void drawBones(Object3D *o, aiNode *node, Vertex3D *lastBonePosition = nullptr);
    void setRemoveAtEndAnimation(bool removeAtEnds);
    [[nodiscard]] bool isRemoveAtEndAnimation() const;
    void setAnimationSpeed(float animationSpeed);
    void setIndexCurrentAnimation(int indexCurrentAnimation);
    bool isAnimationEnds() const;
    float getCurrentAnimationMaxTime() const;
    void CheckIfEndAnimation();
    void FillAnimationBoneDataOGLBuffers(std::vector<Mesh3DData> &meshes);
    void UpdateOpenGLBones(std::vector<Mesh3DData> &meshes);
    void setAnimationByName(const std::string& name);
    bool isLoop() const;
    void setLoop(bool loop);
};


#endif //BRAKEZA3D_ASSIMPANIMATIONSERVICE_H
