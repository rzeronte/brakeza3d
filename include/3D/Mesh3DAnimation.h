#ifndef BRAKEDA3D_MESH3DANIMATED_H
#define BRAKEDA3D_MESH3DANIMATED_H

#include "Mesh3D.h"
#include "../Misc/Logging.h"
#include <cstring>
#include <cassert>
#include <assimp/Importer.hpp>

#define NUM_BONES_PER_VERTEX 4
#define MAX_BONES 100

struct VertexBoneData {
    int IDs[NUM_BONES_PER_VERTEX];
    float Weights[NUM_BONES_PER_VERTEX] = {0};

    void AddBoneData(int boneId, float weight) {
        unsigned int end = std::size(IDs);
        for (unsigned int i = 0; i < end; i++) {
            if (Weights[i] == 0.0) {
                IDs[i] = boneId;
                Weights[i] = weight;
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

class Mesh3DAnimation : public Mesh3D
{
    int numBones = 0;
    int indexCurrentAnimation = 0;
    int boneColliderIndex = 0;
    float runningTime = 0;
    float animation_speed = 1;
    bool loop = true;
    bool boneColliderEnabled = false;
    bool removeOnAnimationEnd = false;
    bool finished = false;

    const aiScene *scene = nullptr;
    Assimp::Importer importer;

    aiMatrix4x4 globalInverseTransform;

    std::vector<std::vector<VertexBoneData>> meshVerticesBoneData;  // mesh[] > vertex[] > VertexBoneData
    std::vector<std::vector<Vertex3D>> meshVertices;                // mesh[] > vertices

    std::map<std::string, unsigned int> boneMapping;                // maps a bone's name to its index
    std::vector<BoneInfo> boneInfo;                                 // Bone info and final transformation
    std::vector<BonesMappingColliders> boneMappingColliders;
public:
    Mesh3DAnimation();
    ~Mesh3DAnimation() override;

    bool AssimpLoadAnimation(const std::string &filename);
    void onUpdate() override;
    void UpdateFrameTransformations();
    void ProcessNodeAnimation(aiNode *node);
    void ProcessMeshAnimation(int i, aiMesh *mesh);
    void ReadNodesFromRoot();
    void UpdateBonesFinalTransformations(float TimeInSeconds);
    void ReadNodeHierarchy(float AnimationTime, const aiNode *pNode, const aiMatrix4x4 &ParentTransform);
    void UpdateForBone(Vertex3D &dest, int meshID, int vertexID);
    void LoadMeshBones(int meshId, aiMesh *mesh, std::vector<VertexBoneData> &meshVertexBoneData);
    void DrawBones(aiNode *node, Vertex3D *lastBonePosition = nullptr);
    void setRemoveAtEndAnimation(bool removeAtEnds);
    void DrawPropertiesGUI() override;
    void setAnimationSpeed(float value);
    void setIndexCurrentAnimation(int indexCurrentAnimation);
    void CheckIfEndAnimation();
    void FillAnimationBoneDataOGLBuffers();
    void UpdateOpenGLBones();
    void UpdateBoundingBox() override;
    void setAnimationByName(const std::string& name);
    void setLoop(bool value);
    void createBonesMappingColliders(const std::string &name);
    void createBoneGhostBody(int bmIndex, unsigned int boneId, const BoneCollisionShape &shape, BoneColliderInfo &ci);
    void removeBonesColliderMapping(const std::string &name);
    void ResolveCollision(CollisionInfo with) override;
    void ShadowMappingPass() override;
    void SetMappingBoneColliderInfo(const std::string& mappingName, unsigned int boneId, bool enabled, BoneCollisionShape shape);
    void UpdateBoneColliders();

    BonesMappingColliders *getBonesMappingByName(const std::string& name, int &index);
    int &BoneColliderIndexPointer()                         { return boneColliderIndex; };
    ObjectType getTypeObject() const override               { return ObjectType::Mesh3DAnimation; }
    GUIType::Sheet getIcon() override                       { return IconObject::MESH_3D_ANIMATION; }
    [[nodiscard]] bool isRemoveAtEndAnimation() const       { return removeOnAnimationEnd; }

    [[nodiscard]] bool isLoop() const                       { return loop; }
    [[nodiscard]] bool isAnimationEnds() const              { return finished; }
    [[nodiscard]] float getCurrentAnimationMaxTime() const;
    [[nodiscard]] const std::vector<BonesMappingColliders> *getBoneMappingColliders() const;

    static void CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);
    static void CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);
    static void CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);
    static void LoadMeshVertex(int meshId, aiMesh *mesh, std::vector<Vertex3D> &meshVertex, std::vector<Vertex3D> &meshNormals);
    static const aiNodeAnim *FindNodeAnim(const aiAnimation *pAnimation, const std::string& NodeName);
    static unsigned int FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim);
    static unsigned int FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim);
    static unsigned int FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim);

    friend class Mesh3DAnimationSerializer;
    friend class Mesh3DAnimationDrawerGUI;
};

#endif //BRAKEDA3D_MESH3DANIMATED_H

