//
// Created by darkhead on 13/2/20.
//

#ifndef BRAKEDA3D_MESH3DANIMATED_H
#define BRAKEDA3D_MESH3DANIMATED_H

#include "Mesh3D.h"
#include <cstring>

#define NUM_BONES_PER_VERTEX 14
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

    std::vector< std::vector<VertexBoneData> > meshVerticesBoneData;
    std::vector< std::vector<Vertex3D> > meshVertices;

    int   indexCurrentAnimation = 0;
    float runningTime = 0;

    std::map<std::string,uint> boneMapping; // maps a bone name to its index
    std::vector<BoneInfo> boneInfo;
    aiMatrix4x4 m_GlobalInverseTransform;

    void onUpdate();

    bool AssimpLoad(const std::string &Filename);

    aiMatrix4x4 BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4>& Transforms);

    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);

    uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

    bool InitMaterials(const aiScene* pScene, const std::string& Filename);

    int updateForBone(Vertex3D &dest, int meshID, int vertexID,  std::vector<aiMatrix4x4> &Transforms);
    void processNode(aiNode *node);
    void processMesh(int i, aiMesh *mesh);
    void loadMeshBones(aiMesh *mesh, std::vector<VertexBoneData> &);

    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

    void AIMatrixToVertex( Vertex3D &V, aiMatrix4x4 &m );

    void drawBones(aiNode *node, std::vector<aiMatrix4x4> &Transforms);
    void drawVertexWeights();
    Uint32 processWeigthColor(int weight);
};


#endif //BRAKEDA3D_MESH3DANIMATED_H
