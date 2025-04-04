#ifndef BRAKEDA3D_MESH3DANIMATED_H
#define BRAKEDA3D_MESH3DANIMATED_H

#include "Mesh3D.h"
#include "../Render/Logging.h"
#include <cstring>
#include <cassert>

#define NUM_BONES_PER_VERTEX 4
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define MAX_BONES 100

struct VertexBoneData {
    unsigned int IDs[NUM_BONES_PER_VERTEX];
    float Weights[NUM_BONES_PER_VERTEX] = {0};

    void AddBoneData(unsigned int BoneID, float Weight) {
        int end = ARRAY_SIZE_IN_ELEMENTS(IDs);
        for (unsigned int i = 0; i < end; i++) {
            if (Weights[i] == 0.0) {
                IDs[i] = (int) BoneID;
                Weights[i] = Weight;
                return;
            }
        }
        Logging::Message("NUM_BONES_PER_VERTEX reached");
        assert(0);
    }
};

struct BoneInfo {
    std::string name;
    Vertex3D position;

    aiMatrix4x4 BoneOffset;
    aiMatrix4x4 FinalTransformation;
};

enum BoneCollisionShape {
    BONE_SPHERE = 0,
    BONE_CUBE = 1,
    BONE_CAPSULE = 2
};

struct BoneColliderInfo {
    unsigned int boneId;
    bool enabled = false;
    BoneCollisionShape shape = BoneCollisionShape::BONE_SPHERE;
    btPairCachingGhostObject *ghostObject = nullptr;
    btConvexHullShape *convexHullShape = nullptr;
    Vertex3D size;
    Vertex3D position;
    std::string name;
};

struct BonesMappingColliders {
    std::string nameMapping;
    bool enabled;
    std::vector<BoneColliderInfo> boneColliderInfo;
};

class Mesh3DAnimation : public Mesh3D {
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

    bool boneColliderEnabled;

    std::vector<BonesMappingColliders> boneMappingColliders;

    int indexCurrentAnimation;
    float runningTime;
    bool remove_at_end_animation;
    float animation_speed;
    bool animation_ends;
public:
    Mesh3DAnimation();

    ~Mesh3DAnimation() override;

    void onUpdate() override;

    void UpdateFrameTransformations();

    bool AssimpLoadAnimation(const std::string &filename);

    void ProcessNodeAnimation(aiNode *node);

    void ProcessMeshAnimation(int i, aiMesh *mesh);

    void ReadNodesFromRoot();

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

    void drawBones(aiNode *node, Vertex3D *lastBonePosition = nullptr);

    void setRemoveAtEndAnimation(bool removeAtEnds);

    [[nodiscard]] bool isRemoveAtEndAnimation() const;

    const char *getTypeObject() override;

    const char *getTypeIcon() override;

    void drawImGuiProperties() override;

    cJSON *getJSON() override;

    static void createFromJSON(cJSON *object);

    static void setPropertiesFromJSON(cJSON *object, Mesh3DAnimation *o);

    static Mesh3DAnimation* create(Vertex3D position, const std::string& animationFile);

    void setAnimationSpeed(float animationSpeed);

    void setIndexCurrentAnimation(int indexCurrentAnimation);

    bool isAnimationEnds() const;

    float getCurrentAnimationMaxTime() const;

    void CheckIfEndAnimation();

    void FillAnimationBoneDataOGLBuffers();

    void UpdateOpenGLBones();

    void updateBoundingBox();

    void setAnimationByName(const std::string& name);

    bool isLoop() const;

    void setLoop(bool loop);

    void createBonesMappingColliders(std::string name);

    void SetMappingBoneColliderInfo(
        const std::string& mappingName,
        unsigned int boneId,
        bool enabled,
        BoneCollisionShape shape
     );

    void UpdateBoneColliders();

    BonesMappingColliders *getBonesMappingByName(const std::string& name);

    const std::vector<BonesMappingColliders> *getBoneMappingColliders() const;

    int boneColliderIndex;

    void createBoneGhostBody(unsigned int boneId, const BoneCollisionShape &shape, BoneColliderInfo &ci);

    void removeBonesColliderMapping(const std::string &name);
};


#endif //BRAKEDA3D_MESH3DANIMATED_H

