#ifndef BRAKEDA3D_MESH3DANIMATED_H
#define BRAKEDA3D_MESH3DANIMATED_H

#include "Mesh3D.h"
#include "../Misc/Logging.h"
#include <cassert>
#include <unordered_map>
#include <assimp/Importer.hpp>

#define NUM_BONES_PER_VERTEX 6
#define MAX_BONES 100

struct AnimationData;

struct VertexBoneData {
    int IDs[NUM_BONES_PER_VERTEX] = {0};
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
        LOG_MESSAGE("NUM_BONES_PER_VERTEX reached");
        assert(0);
    }
};

struct BoneInfo {
    std::string name;
    Vertex3D position;

    // Offset (bind pose) de este hueso, UNO POR CADA MESH que lo usa. Un personaje con
    // varios submeshes (cuerpo/manos/pies/casco...) puede tener el mismo hueso con un
    // offset distinto por submesh, porque cada mesh tiene su propia relación con el
    // esqueleto en el momento del bind. Guardar un único offset compartido (como antes)
    // hace que el segundo mesh en usar un hueso "herede" el offset del primero -> malla
    // deformada de forma catastrófica en cuanto ese hueso rota lejos de su pose de reposo.
    std::unordered_map<int, aiMatrix4x4> BoneOffsetByMesh;  // meshId -> offset

    // Transform del hueso en el mundo (globalInverseTransform * GlobalTransformation),
    // SIN offset de ningún mesh aplicado. Es el mismo para todos los meshes -- correcto
    // usarlo directamente para "donde esta este hueso" (debug draw, bone colliders,
    // getBoneWorldPosition/Rotation). Para skinning real hace falta combinarlo con el
    // offset del mesh concreto (ver meshBoneFinalTransforms).
    aiMatrix4x4 WorldTransform;
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
    std::unordered_map<std::string, const aiNodeAnim*> nodeAnimCache;
    bool boneColliderEnabled = false;
    bool removeOnAnimationEnd = false;
    bool finished = false;

    const aiScene *scene = nullptr;
    std::shared_ptr<Assimp::Importer> sharedImporter;

    // Mantiene vivo el AnimationData cacheado (y sus buffers de GPU compartidos de pose de
    // reposo, ver AnimationMeshEntry) mientras esta instancia exista -- mismo mecanismo que
    // Mesh3D::sharedModel, ver Mesh3D::sharedStaticGeometry.
    std::shared_ptr<AnimationData> sharedAnimModel;

    aiMatrix4x4 globalInverseTransform;

    std::vector<std::vector<VertexBoneData>> meshVerticesBoneData;  // mesh[] > vertex[] > VertexBoneData
    std::vector<std::vector<Vertex3D>> meshVertices;                // mesh[] > vertices

    std::map<std::string, unsigned int> boneMapping;                // maps a bone's name to its index
    std::vector<BoneInfo> boneInfo;                                 // Bone info and final transformation
    std::vector<BonesMappingColliders> boneMappingColliders;

    // [meshId][boneIndex] = WorldTransform * BoneOffsetByMesh[meshId] (identidad si ese
    // mesh no usa ese hueso). Recalculado cada vez que cambian las FinalTransformation
    // (una vez por frame animado). Es lo que de verdad se usa para deformar cada submesh,
    // tanto en GPU (UpdateOpenGLBones) como en CPU (UpdateForBone / bounding box).
    std::vector<std::vector<aiMatrix4x4>> meshBoneFinalTransforms;
    std::vector<std::vector<glm::mat4>> boneTransformCachePerMesh;
public:
    Mesh3DAnimation();
    ~Mesh3DAnimation() override;

    bool AssimpLoadAnimation(const std::string &filename);
    void onUpdate() override;
    void postUpdate() override;
    void UpdateFrameTransformations();
    void ProcessNodeAnimation(aiNode *node);
    void ProcessMeshAnimation(int i, aiMesh *mesh);
    void ReadNodesFromRoot();
    void UpdateBonesFinalTransformations(float TimeInSeconds);
    void ReadNodeHierarchy(float AnimationTime, const aiNode *pNode, const aiMatrix4x4 &ParentTransform);
    void ComputeMeshBoneFinalTransforms();
    void UpdateForBone(Vertex3D &dest, int meshID, int vertexID);
    void LoadMeshBones(int meshId, aiMesh *mesh, std::vector<VertexBoneData> &meshVertexBoneData, bool skipWeights = false);
    void DrawBones(aiNode *node, Vertex3D *lastBonePosition = nullptr);
    void setRemoveAtEndAnimation(bool removeAtEnds);
    void DrawPropertiesGUI() override;
    void setAnimationSpeed(float value);
    void setIndexCurrentAnimation(int indexCurrentAnimation);
    void CheckIfEndAnimation();
    void FillOGLBuffers() override;
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
    [[nodiscard]] int getNumAnimations() const              { return scene ? static_cast<int>(scene->mNumAnimations) : 0; }
    [[nodiscard]] std::string getAnimationName(int i) const { return (scene && i >= 0 && i < static_cast<int>(scene->mNumAnimations)) ? scene->mAnimations[i]->mName.C_Str() : ""; }
    [[nodiscard]] float getCurrentAnimationMaxTime() const;
    [[nodiscard]] const std::vector<BonesMappingColliders> *getBoneMappingColliders() const;

    // Instancing de unidades animadas (Fase 1): matrices de huesos de este submesh ya calculadas
    // este frame por UpdateOpenGLBones() (glm, listas para subir a un TBO), reutilizadas por
    // ComponentRender::EnqueueOpaque sin recalcular nada.
    [[nodiscard]] const std::vector<glm::mat4>& getBoneTransformCache(size_t meshIdx) const { return boneTransformCachePerMesh[meshIdx]; }
    [[nodiscard]] Vertex3D getBoneWorldPosition(const std::string& boneName) const;
    [[nodiscard]] M3 getBoneWorldRotation(const std::string& boneName) const;
    [[nodiscard]] std::vector<std::string> getBoneNames() const;

    static void CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);
    static void CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);
    static void CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim);
    static void LoadMeshVertex(int meshId, aiMesh *mesh, std::vector<Vertex3D> &meshVertex, std::vector<Vertex3D> &meshNormals);
    const aiNodeAnim *FindNodeAnim(const aiAnimation *pAnimation, const std::string& NodeName);
    static unsigned int FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim);
    static unsigned int FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim);
    static unsigned int FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim);

    friend class Mesh3DAnimationSerializer;
    friend class Mesh3DAnimationDrawerGUI;
};

#endif //BRAKEDA3D_MESH3DANIMATED_H

