/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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

    // Biblioteca de animaciones compartida (ver sharedAnimationsFiles). Si el modelo no trae
    // animaciones propias, los clips salen de animScene en vez de scene. animImporter mantiene
    // viva esa escena (solo lectura, compartida entre todas las instancias via sceneCache).
    const aiScene *animScene = nullptr;
    std::shared_ptr<Assimp::Importer> animImporter;

    // Retargeting de las animaciones compartidas. Los modelos tienen huesos de distinta longitud y
    // T-poses distintas (brazos mas o menos levantados), asi que de la biblioteca solo se toma el
    // MOVIMIENTO de cada hueso, no su pose absoluta:
    //   - rotacion: giro en el MUNDO respecto al reposo de la biblioteca, aplicado sobre el reposo
    //     del modelo (Wt = Wl(t) * Wl_reposo^-1 * Wt_reposo); la local sale de dividir por el padre.
    //   - posicion y escala de cada hueso: las del modelo. Solo el hueso raiz (cadera) usa el
    //     desplazamiento animado respecto al reposo de la biblioteca, escalado por rootScale
    //     (proporcion de alturas), sumado al reposo propio del modelo.
    struct RetargetBone {
        int         idx       = -1;          // posicion en retargetWl/retargetWt
        int         parentIdx = -1;          // hueso padre (-1 = hueso raiz)
        bool        inLib     = false;       // el hueso existe en la biblioteca
        bool        isRoot    = false;
        float       rootScale = 1.0f;
        aiVector3D  pos;                     // posicion local de reposo del modelo
        aiVector3D  scale{1.0f, 1.0f, 1.0f};
        aiVector3D  libRestPos;              // posicion de reposo en la biblioteca (solo el raiz)
        aiMatrix3x3 modelRestLocal;          // rotacion local de reposo del modelo
        aiMatrix3x3 modelRestWorld;          // ... en el mundo (incluye ancestros)
        aiMatrix3x3 modelParentWorld;        // mundo de los ancestros del hueso raiz (modelo)
        aiMatrix3x3 libRestWorldInv;         // inversa de la rotacion mundo de reposo en la biblioteca
        aiMatrix3x3 libParentWorld;          // mundo de los ancestros del hueso raiz (biblioteca)
    };
    bool animCopyLocal = false;              // modo de la biblioteca en uso (SharedAnimationLibrary::copyLocalRotations)
    std::unordered_map<const aiNode*, RetargetBone> retargetBones;
    std::vector<aiMatrix3x3> retargetWl;     // rotacion mundo animada en la biblioteca (por frame)
    std::vector<aiMatrix3x3> retargetWt;     // rotacion mundo resultante en el modelo (por frame)

    // Esqueleto precalculado (ver BuildSkeletonCache): recorrido plano sin búsquedas por nombre en cada frame.
    struct SkelNode {
        const aiNode*       node      = nullptr;
        int                 parent    = -1;       // índice en skelNodes (-1 = raíz); siempre anterior al del hijo
        const aiNodeAnim*   anim      = nullptr;  // canal de este nodo en la animación actual (nullptr = sin canal)
        const RetargetBone* rb        = nullptr;  // reposo retargeteado (solo animaciones de biblioteca compartida)
        int                 boneIndex = -1;       // hueso del modelo que es este nodo (-1 = no es hueso)
    };
    std::vector<SkelNode> skelNodes;
    std::vector<aiMatrix4x4> skelGlobal;                          // transformación global de cada nodo en este frame
    std::vector<aiMatrix4x4> skelOffsetStore;                     // offsets de bind copiados en contiguo ([mesh * numBones + hueso]); skelMeshOffsets apunta aquí
    std::vector<std::vector<const aiMatrix4x4*>> skelMeshOffsets; // [mesh][hueso] -> offset de bind (nullptr si el mesh no lo usa)
    const Mesh3DAnimation *skelKeyThis = nullptr;   // la caché guarda punteros a miembros de ESTA instancia
    const aiScene *skelKeyScene = nullptr;
    const aiScene *skelKeyAnimScene = nullptr;
    int skelKeyAnim = -1;
    size_t skelKeyRetarget = 0, skelKeyBones = 0, skelKeyMeshes = 0;
    bool skelValid = false;
    [[nodiscard]] bool SkeletonCacheValid() const;
    void BuildSkeletonCache();
    void ReadNodeHierarchyCached(float AnimationTime);
    aiMatrix4x4 NodeLocalTransform(float AnimationTime, const aiNode *pNode, const aiNodeAnim *pNodeAnim, const RetargetBone *rbPtr);
    void AttachSharedAnimations();

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
    // Ficheros con TODAS las animaciones de cada familia de esqueleto. Un modelo sin animaciones
    // propias usa la primera biblioteca cuyo esqueleto coincide con el suyo (nombres de hueso).
    // Los declara el proyecto (clave "sharedAnimationsFiles" de su JSON). Vacio o inexistentes =
    // desactivado (comportamiento anterior: cada FBX lleva las suyas).
    struct SharedAnimationLibrary {
        std::string file;
        // false (por defecto): las animaciones de la biblioteca se adaptaron a la T-pose de cada
        //   personaje (giro en el mundo respecto al reposo, ver RetargetBone).
        // true: las rotaciones locales de la biblioteca se aplican tal cual (animaciones copiadas
        //   entre armatures con el mismo esqueleto, sin adaptar). Ver "mode" en el JSON del proyecto.
        bool copyLocalRotations = false;
    };
    static std::vector<SharedAnimationLibrary> sharedAnimationsFiles;

    // Parsea en segundo plano (pool de computo) todas las bibliotecas declaradas, para que esten listas cuando
    // empiece la carga de personajes en lugar de parsearse (~segundos cada una) en el momento en que
    // el primer personaje las pide, con el resto de trabajadores bloqueados esperando. Se llama al cargar el proyecto.
    static void PreloadSharedAnimations();

    // Escena de la que salen las animaciones: la biblioteca compartida o la del propio modelo.
    [[nodiscard]] const aiScene* animationsScene() const { return animScene ? animScene : scene; }

    Mesh3DAnimation();
    ~Mesh3DAnimation() override;

    bool AssimpLoadAnimation(const std::string &filename);
    void onUpdate() override;
    void onUpdateCulled() override;
    void postUpdate() override;
    void UpdateFrameTransformations();
    bool AdvanceAnimationClock();
    void ProcessNodeAnimation(aiNode *node);
    void ProcessMeshAnimation(int i, aiMesh *mesh);
    void ReadNodesFromRoot();
    void UpdateBonesFinalTransformations(float TimeInSeconds);
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
    // bake=false: solo calcula las matrices de huesos (boneTransformCachePerMesh); sin la pasada de
    // transform feedback que hornea la pose en feedbackBuffer (ver NeedsBakedPose)
    void UpdateOpenGLBones(bool bake = true);
    // ¿Alguien leerá feedbackBuffer este frame? (fase 3 del instancing, 2026-10-10)
    [[nodiscard]] bool NeedsBakedPose() const;
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
    // Vuelve al primer frame del clip actual aunque ya estuviera sonando (setAnimationByName con el mismo clip no lo reinicia)
    void restartAnimation();
    // Duración real en segundos del clip actual, ya dividida por la velocidad de animación
    [[nodiscard]] float getAnimationDuration() const;
    [[nodiscard]] int getNumAnimations() const              { auto* a = animationsScene(); return a ? static_cast<int>(a->mNumAnimations) : 0; }
    [[nodiscard]] std::string getAnimationName(int i) const { auto* a = animationsScene(); return (a && i >= 0 && i < static_cast<int>(a->mNumAnimations)) ? a->mAnimations[i]->mName.C_Str() : ""; }
    [[nodiscard]] float getCurrentAnimationMaxTime() const;
    [[nodiscard]] const std::vector<BonesMappingColliders> *getBoneMappingColliders() const;

    // Instancing de unidades animadas (Fase 1): matrices de huesos de este submesh ya calculadas
    // este frame por UpdateOpenGLBones() (glm, listas para subir a un TBO), reutilizadas por
    // ComponentRender::EnqueueOpaque sin recalcular nada.
    [[nodiscard]] const std::vector<glm::mat4>& getBoneTransformCache(size_t meshIdx) const { return boneTransformCachePerMesh[meshIdx]; }
    // Vacío hasta la primera actualización de huesos: comprobar antes de getBoneTransformCache
    // desde pasadas que incluyen objetos que quizá nunca se actualizaron (shadow pass, fuera de cámara)
    [[nodiscard]] size_t getBoneTransformCacheCount() const { return boneTransformCachePerMesh.size(); }
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

