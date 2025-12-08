//
// Created by Eduardo on 30/03/2025.
//

#include "../../include/Render/AssimpAnimationService.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Brakeza.h"
#include "../../include/Render/Transforms.h"
#include <assimp/postprocess.h>

AssimpAnimationService::AssimpAnimationService()
{
}

void AssimpAnimationService::UpdateOpenGLBones(std::vector<Mesh3DData> &meshes)
{
    std::vector transformations(MAX_BONES, glm::mat4(0));

    for (int i = 0; i < (int) boneInfo.size(); i++) {
        transformations[i] = Tools::aiMat4toGLMMat4(boneInfo[i].FinalTransformation);
    }

    for (auto &m: meshes) {
        if (m.vertices.empty()) continue;
        ComponentsManager::get()->Render()->getShaders()->shaderOGLBonesTransforms->render(
            m,
            transformations,
            ComponentsManager::get()->Window()->getSceneFramebuffer()
        );
    }
}

void AssimpAnimationService::UpdateFrameTransformations()
{
    if (!scene->HasAnimations()) return;

    CheckIfEndAnimation();

    if (isRemoveAtEndAnimation() && isAnimationEnds()) {
        //getParent()->setRemoved(true);
        return;
    }

    UpdateBonesFinalTransformations(runningTime);
}

void AssimpAnimationService::CheckIfEndAnimation()
{
    auto timeIncrement = Brakeza::get()->getDeltaTime() * animation_speed;
    animation_ends = false;
    auto maxAnimationTime = getCurrentAnimationMaxTime();

    if (runningTime + timeIncrement >= maxAnimationTime) {
        animation_ends = true;
        if (loop) {
            runningTime = 0.000;
        } else {
            runningTime = maxAnimationTime - 0.01f;
        }

        return;
    }

    runningTime += timeIncrement;
}

float AssimpAnimationService::getCurrentAnimationMaxTime() const
{
    return (float) (
            scene->mAnimations[indexCurrentAnimation]->mDuration / scene->mAnimations[indexCurrentAnimation]->mTicksPerSecond
    );
}

bool AssimpAnimationService::AssimpLoadAnimation(const std::string &filename, std::vector<Mesh3DData> &meshes)
{
    Logging::Message("[AssimpAnimationService] Load animation from %s", filename.c_str());

    if (!Tools::FileExists(filename.c_str())) {
        Logging::Message("[AssimpLoadAnimation] ERROR: File not found %s", filename.c_str());
        return false;
    }

    scene = importer.ReadFile(
        filename,
        aiProcess_Triangulate |
        aiProcess_SortByPType |
        aiProcess_GenNormals |
        aiProcess_OptimizeMeshes |
        aiProcess_FlipUVs
    );

    if (!scene) {
        Logging::Message("Error import 3D file for ASSIMP");
        exit(-1);
    }

    ReadNodesFromRoot(meshes);

    ComponentsManager::get()->Render()->FillOGLBuffers(meshes);

    FillAnimationBoneDataOGLBuffers(meshes);

    return true;
}

void AssimpAnimationService::ReadNodesFromRoot(std::vector<Mesh3DData> &meshes)
{
    // Transformación raíz
    globalInverseTransform = scene->mRootNode->mTransformation;
    globalInverseTransform.Inverse();
    meshes.resize(scene->mNumMeshes);

    // Para cada malla preparamos sus array de "Vertices" y "VertexBoneData"
    meshVertices.resize(scene->mNumMeshes);
    meshVerticesBoneData.resize(scene->mNumMeshes);

    // Procesamos los nodos desde el nodo raíz
    ProcessNodeAnimation(scene->mRootNode, meshes);
}

void AssimpAnimationService::ProcessNodeAnimation(aiNode *node, std::vector<Mesh3DData> &meshes)
{
    // Procesamos las mallas del nodo dado
    for (unsigned int x = 0; x < node->mNumMeshes; x++) {
        int idMesh = (int) node->mMeshes[x];
        this->ProcessMeshAnimation(idMesh, scene->mMeshes[idMesh], meshes);
    }

    // Continuamos recursivamente por los hijos del nodo dado
    for (unsigned int j = 0; j < node->mNumChildren; j++) {
        ProcessNodeAnimation(node->mChildren[j], meshes);
    }
}

void AssimpAnimationService::ProcessMeshAnimation(int i, aiMesh *mesh, std::vector<Mesh3DData> &meshes)
{
    if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
        Logging::Message("Skip mesh non triangle");
        return;
    }

    Logging::Message("[AssimpAnimationService] Loading mesh animation with materialIndex: %d", mesh->mMaterialIndex);

    std::vector<VertexBoneData> localMeshBones(mesh->mNumVertices);
    std::vector<Vertex3D> localMeshVertices(mesh->mNumVertices);
    std::vector<Vertex3D> localMeshNormals(mesh->mNumVertices);

    LoadMeshBones(i, mesh, localMeshBones);
    LoadMeshVertex(i, mesh, localMeshVertices, localMeshNormals);

    meshVertices[i] = localMeshVertices;
    meshVerticesBoneData[i] = localMeshBones;

    for (unsigned int k = 0; k < mesh->mNumFaces; k++) {
        const aiFace &Face = mesh->mFaces[k];

        if (Face.mNumIndices < 3) continue;

        auto index1 = Face.mIndices[0];
        auto index2 = Face.mIndices[1];
        auto index3 = Face.mIndices[2];

        Vertex3D V1 = meshVertices[i][index1];
        Vertex3D V2 = meshVertices[i][index2];
        Vertex3D V3 = meshVertices[i][index3];

        //meshes[i].modelTriangles.push_back(new Triangle(V1, V2, V3, this));

        Vertex3D normalV1 = localMeshNormals[index1];
        Vertex3D normalV2 = localMeshNormals[index2];
        Vertex3D normalV3 = localMeshNormals[index3];

        meshes[i].vertices.emplace_back(V1.x, V1.y, V1.z, 1.0f);
        meshes[i].vertices.emplace_back(V2.x, V2.y, V2.z, 1.0f);
        meshes[i].vertices.emplace_back(V3.x, V3.y, V3.z, 1.0f);

        meshes[i].uvs.emplace_back(V1.u, V1.v);
        meshes[i].uvs.emplace_back(V2.u, V2.v);
        meshes[i].uvs.emplace_back(V3.u, V3.v);

        meshes[i].normals.emplace_back(normalV1.x, normalV1.y, normalV1.z);
        meshes[i].normals.emplace_back(normalV2.x, normalV2.y, normalV2.z);
        meshes[i].normals.emplace_back(normalV3.x, normalV3.y, normalV3.z);
    }
}

void AssimpAnimationService::LoadMeshBones(int meshId, aiMesh *mesh, std::vector<VertexBoneData> &meshVertexBoneData)
{
    for (int i = 0; i < (int) mesh->mNumBones; i++) {
        int BoneIndex;
        std::string BoneName(mesh->mBones[i]->mName.data);

        if (boneMapping.find(BoneName) == boneMapping.end()) {
            BoneIndex = numBones;
            numBones++;
            BoneInfo bi = BoneInfo();
            boneInfo.push_back(bi);

            boneMapping[BoneName] = BoneIndex;

            boneInfo[BoneIndex].BoneOffset = mesh->mBones[i]->mOffsetMatrix;
            boneInfo[BoneIndex].name = mesh->mBones[i]->mName.C_Str();
        } else {
            BoneIndex = (int) boneMapping[BoneName];
        }

        for (int j = 0; j < (int) mesh->mBones[i]->mNumWeights; j++) {
            unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = mesh->mBones[i]->mWeights[j].mWeight;

            meshVertexBoneData[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}

void AssimpAnimationService::LoadMeshVertex(int meshId, aiMesh *mesh, std::vector<Vertex3D> &meshVertex, std::vector<Vertex3D> &meshNormal)
{
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
        aiVector3t vf = mesh->mVertices[j];
        Vertex3D v(vf.x, vf.y, vf.z);

        const aiVector3D *pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][j]) : &Zero3D;
        v.u = pTexCoord->x;
        v.v = pTexCoord->y;

        meshVertex[j] = v;
        aiVector3t n = mesh->mNormals[j];
        meshNormal[j] = Vertex3D::fromAssimp(n);
    }
}

void AssimpAnimationService::UpdateBonesFinalTransformations(float TimeInSeconds)
{
    aiMatrix4x4 Identity = aiMatrix4x4();

    float TicksPerSecond = scene->mAnimations[indexCurrentAnimation]->mTicksPerSecond != 0
                           ? scene->mAnimations[indexCurrentAnimation]->mTicksPerSecond : 0.25f;
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    auto AnimationTime = (float) fmod(TimeInTicks, scene->mAnimations[indexCurrentAnimation]->mDuration);

    ReadNodeHierarchy(AnimationTime, scene->mRootNode, Identity);
}

void AssimpAnimationService::ReadNodeHierarchy(float AnimationTime, const aiNode *pNode, const aiMatrix4x4 &ParentTransform)
{
    std::string NodeName(pNode->mName.data);
    const aiAnimation *pAnimation = scene->mAnimations[indexCurrentAnimation];

    aiMatrix4x4 NodeTransformation;
    NodeTransformation = pNode->mTransformation;

    const aiNodeAnim *pNodeAnim = FindNodeAnim(pAnimation, NodeName);

    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        aiMatrix4x4 ScalingM = aiMatrix4x4();
        aiMatrix4x4::Scaling(Scaling, ScalingM);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        aiMatrix4x4 RotationM = aiMatrix4x4(RotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        aiMatrix4x4 TranslationM;
        aiMatrix4x4::Translation(Translation, TranslationM);

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;

    if (boneMapping.find(NodeName) != boneMapping.end()) {
        unsigned int BoneIndex = boneMapping[NodeName];
        boneInfo[BoneIndex].FinalTransformation = globalInverseTransform * GlobalTransformation * boneInfo[BoneIndex].BoneOffset;
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
        ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

const aiNodeAnim *AssimpAnimationService::FindNodeAnim(const aiAnimation *pAnimation, const std::string& NodeName)
{
    for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
        const aiNodeAnim *pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return nullptr;
}

unsigned int AssimpAnimationService::FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

unsigned int AssimpAnimationService::FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

void AssimpAnimationService::updateForBone(Vertex3D &V, int meshID, int vertexID)
{
    if (numBones == 0) return;

    glm::mat4 BoneTransform(0);
    for (int n = 0; n < NUM_BONES_PER_VERTEX; n++) {
        auto boneData = meshVerticesBoneData[meshID][vertexID];
        int boneId = boneData.IDs[n];
        float weight = boneData.Weights[n];
        //Logging::Message("ID: %d, Weight: %f, vertexID: %d", boneId, weight, vertexID);
        BoneTransform += Tools::aiMat4toGLMMat4(boneInfo[boneId].FinalTransformation) * weight;
    }

    V = Vertex3D::fromGLM(BoneTransform * V.toGLM4());
}

unsigned int AssimpAnimationService::FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

void AssimpAnimationService::CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    unsigned int NextRotationIndex = (RotationIndex + 1);
    //assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
    float Factor = (AnimationTime - (float) pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion &StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion &EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;

    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);

    Out = Out.Normalize();
}

void AssimpAnimationService::CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    unsigned int NextPositionIndex = (PositionIndex + 1);

    auto DeltaTime = (float) (pNodeAnim->mPositionKeys[NextPositionIndex].mTime -
                              pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float) pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D &Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D &End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void AssimpAnimationService::CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    unsigned int NextScalingIndex = (ScalingIndex + 1);
    //assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    auto DeltaTime = (float) (pNodeAnim->mScalingKeys[NextScalingIndex].mTime -
                              pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float) pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D &Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D &End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

void AssimpAnimationService::drawBones(Object3D *o, aiNode *node, Vertex3D *lastBonePosition)
{
    std::vector<aiMatrix4x4> Transforms;
    Transforms.resize(numBones);
    for (int i = 0; i < numBones; i++) {
        Transforms[i] = boneInfo[i].FinalTransformation;
    }

    int idCurrentNode;

    if (boneMapping.find(node->mName.C_Str()) != boneMapping.end()) {
        idCurrentNode = boneMapping[node->mName.C_Str()];

        aiMatrix4x4 mOffset = boneInfo[idCurrentNode].BoneOffset;
        aiMatrix4x4 mT = Transforms[idCurrentNode];
        aiVector3D aBonePosition;

        aBonePosition = mOffset.Inverse() * aBonePosition;
        aBonePosition = mT * aBonePosition;

        Vertex3D bonePosition = Vertex3D::fromAssimp(aBonePosition);
        Transforms::objectSpace(bonePosition, bonePosition, o);

        if (lastBonePosition) {
            auto render = ComponentsManager::get()->Render();
            auto window = ComponentsManager::get()->Window();
            render->getShaders()->shaderOGLLine3D->render(
                *lastBonePosition,
                bonePosition,
                window->getForegroundFramebuffer(),
                Color::yellow()
            );
        }

        lastBonePosition = &bonePosition;
    }

    for (int j = 0; j < (int) node->mNumChildren; j++) {
        drawBones(o, node->mChildren[j], lastBonePosition);
    }
}

bool AssimpAnimationService::isRemoveAtEndAnimation() const
{
    return removeAtAnimationEnd;
}

void AssimpAnimationService::setRemoveAtEndAnimation(bool removeAtEnds)
{
    removeAtAnimationEnd = removeAtEnds;
}

void AssimpAnimationService::setAnimationSpeed(float animationSpeed) {
    animation_speed = animationSpeed;
}

AssimpAnimationService::~AssimpAnimationService()
{
    meshVerticesBoneData.clear();
    meshVertices.clear();
    boneMapping.clear();
    boneInfo.clear();
}

void AssimpAnimationService::setIndexCurrentAnimation(int indexCurrentAnimation)
{
    AssimpAnimationService::indexCurrentAnimation = indexCurrentAnimation;
}

bool AssimpAnimationService::isAnimationEnds() const
{
    return animation_ends;
}

void AssimpAnimationService::FillAnimationBoneDataOGLBuffers(std::vector<Mesh3DData> &meshes)
{
    for (int i = 0; i < meshes.size(); i++) {
        if (meshes[i].vertices.empty()) continue;

        if (!glIsBuffer(meshes[i].vertexBoneDataBuffer)) {
            glGenBuffers(1, &meshes[i].vertexBoneDataBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, meshes[i].vertexBoneDataBuffer);
            glBufferData(GL_ARRAY_BUFFER, meshVerticesBoneData[i].size() * sizeof(VertexBoneData), meshVerticesBoneData[i].data(), GL_DYNAMIC_DRAW);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, meshes[i].vertexBoneDataBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, meshVerticesBoneData[i].size() * sizeof(VertexBoneData), meshVerticesBoneData[i].data());
        }
    }
}

void AssimpAnimationService::setAnimationByName(const std::string& name)
{
    for (int i = 0; i < (int) scene->mNumAnimations; i++) {
        if (name == scene->mAnimations[i]->mName.C_Str()) {
            setIndexCurrentAnimation(i);
        }
    }
}

bool AssimpAnimationService::isLoop() const {
    return loop;
}

void AssimpAnimationService::setLoop(bool loop) {
    AssimpAnimationService::loop = loop;
}
