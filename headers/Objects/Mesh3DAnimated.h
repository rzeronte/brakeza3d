//
// Created by darkhead on 13/2/20.
//

#ifndef BRAKEDA3D_MESH3DANIMATED_H
#define BRAKEDA3D_MESH3DANIMATED_H

#include "Mesh3D.h"

#define NUM_BONES_PER_VEREX 4

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

struct VertexBoneData
{
    uint IDs[NUM_BONES_PER_VEREX];
    float Weights[NUM_BONES_PER_VEREX];

    void AddBoneData(uint BoneID, float Weight) {
        for (uint i = 0 ; i < ARRAY_SIZE_IN_ELEMENTS(IDs) ; i++) {
            if (Weights[i] == 0.0) {
                IDs[i]     = BoneID;
                Weights[i] = Weight;
                return;
            }
        }
    }
};

struct BoneInfo
{
    aiMatrix4x4 BoneOffset;
    aiMatrix4x4 FinalTransformation;

    BoneInfo()
    {
        //BoneOffset.SetZero();
        //FinalTransformation.SetZero();
    }
};

struct BasicMeshEntry {
    BasicMeshEntry()
    {
        NumIndices = 0;
        BaseVertex = 0;
        BaseIndex = 0;
    }

    unsigned int NumIndices;
    unsigned int BaseVertex;
    unsigned int BaseIndex;
    unsigned int MaterialIndex;
};

class Mesh3DAnimated : public Mesh3D
{
public:
    Assimp::Importer importer;
    const aiScene* scene;

    std::vector< std::vector<VertexBoneData> > meshVerticesBoneData;
    std::vector< std::vector<Vertex3D> > meshVertices;

    uint numBones;
    std::map<std::string,uint> boneMapping; // maps a bone name to its index
    std::vector<BoneInfo> boneInfo;
    aiMatrix4x4 m_GlobalInverseTransform;
    std::vector<BasicMeshEntry> meshInfo;

    bool AssimpLoad(const std::string &Filename);
    void LoadBones(uint, const aiMesh*, std::vector<VertexBoneData>&);
    aiMatrix4x4 BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4>& Transforms);
    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform);
    const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);

    void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
    void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);

    uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
    uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

    void updateForBone(Vertex3D &dest, int meshID, int vertexID,  std::vector<aiMatrix4x4> &Transforms);
    M3 convertAssimpM3(aiMatrix3x3);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
};


#endif //BRAKEDA3D_MESH3DANIMATED_H
