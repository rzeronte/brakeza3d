//
// Created by darkhead on 13/2/20.
//

#ifndef BRAKEDA3D_MESH3DANIMATED_H
#define BRAKEDA3D_MESH3DANIMATED_H

#include "Mesh3D.h"
#include "../Physics/Mesh3DBody.h"
#include "../Render/Logging.h"
#include <cstring>

#define NUM_BONES_PER_VERTEX 4
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

struct VertexBoneData
{
    int IDs[NUM_BONES_PER_VERTEX] = {0};
    float Weights[NUM_BONES_PER_VERTEX] = {0};

    void AddBoneData(uint BoneID, float Weight)
    {
        int end = ARRAY_SIZE_IN_ELEMENTS(IDs);
        for (uint i = 0 ; i < end ; i++) {
            if (Weights[i] == 0.0) {
                IDs[i]     = BoneID;
                Weights[i] = Weight;
                return;
            }
        }
        Logging::getInstance()->Log("NUM_BONES_PER_VERTEX reached");
        assert(0);
    }
};

struct BoneInfo
{
    std::string name;
    Vertex3D position;

    aiMatrix4x4 BoneOffset;
    aiMatrix4x4 FinalTransformation;
};

class Mesh3DAnimated : public Mesh3D
{
public:

    Assimp::Importer importer;
    const aiScene* scene;
    int m_NumBones = 0;

    M3 fixedRotation = M3::getMatrixIdentity();

    std::vector< std::vector<VertexBoneData> > meshVerticesBoneData;
    std::vector< std::vector<Vertex3D> > meshVertices;

    int   indexCurrentAnimation = 0;
    float runningTime = 0;
    bool  remove_at_end_animation = false;

    std::map<std::string,uint> boneMapping; // maps a bone name to its index
    std::vector<BoneInfo>      boneInfo;

    aiMatrix4x4 m_GlobalInverseTransform;

    std::string follow_me_point_label;
    aiNode*     follow_me_point_node;
    Object3D*   follow_me_point_object;

    Object3D *getFollowMePointObject() const;

    void onUpdate();
    void updateFrameTransformations();

    bool AssimpLoadAnimation(const std::string &Filename);
    void AssimpProcessNodeAnimation(aiNode *node);
    void AssimpProcessMeshAnimation(int i, aiMesh *mesh);

    bool ReadNodes();

    aiMatrix4x4 BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4>& Transforms);

    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);

    uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);


    int updateForBone(Vertex3D &dest, int meshID, int vertexID,  std::vector<aiMatrix4x4> &Transforms);
    void loadMeshBones(aiMesh *mesh, std::vector<VertexBoneData> &);

    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

    void AIMatrixToVertex( Vertex3D &V, aiMatrix4x4 &m );

    void drawBones(aiNode *node, std::vector<aiMatrix4x4> &Transforms);
    void drawVertexWeights();
    Uint32 processWeigthColor(int weight);

    bool isRemoveAtEndAnimation() const;
    void setRemoveAtEndAnimation(bool removeAtEnds);


    aiNode *getFollowPointNode();
    void    setFollowPointNode(aiNode *followPointOrigin);

    void updateFollowObjectPosition(std::vector<aiMatrix4x4> Transforms);

    const std::string &getFollowPointLabel() const;
    void setFollowPointLabel(const std::string &followPointLabel);

    const M3 &getFixedRotation() const;

    void setFixedRotation(const M3 &fixedRotation);
};


#endif //BRAKEDA3D_MESH3DANIMATED_H
