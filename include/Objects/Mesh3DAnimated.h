#ifndef BRAKEDA3D_MESH3DANIMATED_H
#define BRAKEDA3D_MESH3DANIMATED_H

#include "Mesh3D.h"
#include "../Physics/Mesh3DBody.h"
#include "../Render/Logging.h"
#include <cstring>
#include <cassert>

#define NUM_BONES_PER_VERTEX 6
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

struct VertexBoneData {
    int IDs[NUM_BONES_PER_VERTEX] = {0};
    float Weights[NUM_BONES_PER_VERTEX] = {0};

    void AddBoneData(unsigned int BoneID, float Weight) {
        int end = ARRAY_SIZE_IN_ELEMENTS(IDs);
        for (unsigned int i = 0; i < end; i++) {
            if (Weights[i] == 0.0) {
                IDs[i] = BoneID;
                Weights[i] = Weight;
                return;
            }
        }
        Logging::Log("NUM_BONES_PER_VERTEX reached", "Mesh3DAnimated");
        assert(0);
    }
};

struct BoneInfo {
    std::string name;
    Vertex3D position;

    aiMatrix4x4 BoneOffset;
    aiMatrix4x4 FinalTransformation;
};

class Mesh3DAnimated : public Mesh3D {
public:
    Assimp::Importer importer;
    const aiScene *scene;
    int m_NumBones = 0;

    M3 fixedRotation = M3::getMatrixIdentity();

    std::vector<std::vector<VertexBoneData> > meshVerticesBoneData;
    std::vector<std::vector<Vertex3D> > meshVertices;

    int indexCurrentAnimation = 0;
    float runningTime = 0;
    bool remove_at_end_animation = false;
    float animation_speed = 1;
    bool animation_ends = false;

    std::map<std::string, unsigned int> boneMapping; // maps a bone name to its index
    std::vector<BoneInfo> boneInfo;

    aiMatrix4x4 m_GlobalInverseTransform;

    std::string follow_me_point_label;
    aiNode *follow_me_point_node;
    Object3D *follow_me_point_object;

    Object3D *getFollowMePointObject() const;

    void onUpdate();

    void updateFrameTransformations();

    bool AssimpLoadAnimation(const std::string &Filename);

    void AssimpProcessNodeAnimation(aiNode *node);

    void AssimpProcessMeshAnimation(int i, aiMesh *mesh);

    void ReadNodes();

    void BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4> &Transforms);

    void ReadNodeHeirarchy(float AnimationTime, const aiNode *pNode, const aiMatrix4x4 &ParentTransform);

    static const aiNodeAnim *FindNodeAnim(const aiAnimation *pAnimation, const std::string& NodeName);

    static unsigned int FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim);

    static unsigned int FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim);

    static unsigned int FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim);


    int updateForBone(Vertex3D &dest, int meshID, int vertexID, std::vector<aiMatrix4x4> &Transforms);

    void loadMeshBones(aiMesh *mesh, std::vector<VertexBoneData> &);

    static void CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);

    static void CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);

    static void CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);

    static void AIMatrixToVertex(Vertex3D &V, aiMatrix4x4 &m);

    void drawBones(aiNode *node, std::vector<aiMatrix4x4> &Transforms);

    bool isRemoveAtEndAnimation() const;

    void setRemoveAtEndAnimation(bool removeAtEnds);


    aiNode *getFollowPointNode() const;

    void setFollowPointNode(aiNode *followPointOrigin);

    void updateFollowObjectPosition(std::vector<aiMatrix4x4> Transforms);

    const std::string &getFollowPointLabel() const;

    void setFollowPointLabel(const std::string &followPointLabel);

    const M3 &getFixedRotation() const;

    void setFixedRotation(const M3 &fixedRotation);

    bool isAnimationEnds() const;
};


#endif //BRAKEDA3D_MESH3DANIMATED_H
