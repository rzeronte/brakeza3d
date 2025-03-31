//
// Created by Eduardo on 30/03/2025.
//

#ifndef BRAKEZA3D_ASSIMPANIMATIONSERVICE_H
#define BRAKEZA3D_ASSIMPANIMATIONSERVICE_H


#include "../Objects/Mesh3DAnimation.h"

class AssimpAnimationService {

private:
    Assimp::Importer importer;
    const aiScene *scene;
    int numBones;
    bool loop;

    aiMatrix4x4 globalInverseTransform;

    std::vector<std::vector<VertexBoneData>> meshVerticesBoneData;  // mesh[] > vertex[] > VertexBoneData
    std::vector<std::vector<Vertex3D>> meshVertices;                // mesh[] > vertices

    std::map<std::string, unsigned int> boneMapping;                // maps a bone's name to its index
    std::vector<BoneInfo> boneInfo;                                 // Bone info and final transformation

    int indexCurrentAnimation;
    float runningTime;
    bool remove_at_end_animation;
    float animation_speed;
    bool animation_ends;

    //std::vector<meshData> meshes;

public:
    AssimpAnimationService();

    void UpdateFrameTransformations();

    ~AssimpAnimationService();

    bool AssimpLoadAnimation(const std::string &filename, std::vector<meshData> &meshes);

    void ProcessNodeAnimation(aiNode *node, std::vector<meshData> &meshes);

    void ProcessMeshAnimation(int i, aiMesh *mesh, std::vector<meshData> &meshes);

    void ReadNodesFromRoot(std::vector<meshData> &meshes);

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

    void FillAnimationBoneDataOGLBuffers(std::vector<meshData> &meshes);

    void UpdateOpenGLBones(std::vector<meshData> &meshes);

    void setAnimationByName(const std::string& name);

    bool isLoop() const;

    void setLoop(bool loop);

};


#endif //BRAKEZA3D_ASSIMPANIMATIONSERVICE_H
