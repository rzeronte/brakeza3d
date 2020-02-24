//
// Created by darkhead on 13/2/20.
//

#include "Mesh3DAnimated.h"
#include "../Brakeza3D.h"


bool Mesh3DAnimated::AssimpLoad(const std::string &Filename)
{
    this->scene = importer.ReadFile( Filename,
                                     aiProcess_CalcTangentSpace       |
                                     aiProcess_Triangulate            |
                                     aiProcess_JoinIdenticalVertices  |
                                     aiProcess_SortByPType
     );

    if( !scene ) {
        //DoTheErrorLogging( importer.GetErrorString());
        Logging::getInstance()->Log("Error import 3D file for ASSIMP");
        exit(-1);
        return false;
    }

    m_GlobalInverseTransform = scene->mRootNode->mTransformation;
    m_GlobalInverseTransform.Inverse();

    meshInfo.resize(scene->mNumMeshes);

    // Count the number of vertices and indices
    uint NumVertices = 0;
    uint NumIndices = 0;

    for (uint i = 0 ; i < meshInfo.size() ; i++) {
        meshInfo[i].MaterialIndex = scene->mMeshes[i]->mMaterialIndex;
        meshInfo[i].NumIndices    = scene->mMeshes[i]->mNumFaces * 3;
        meshInfo[i].BaseVertex    = NumVertices;
        meshInfo[i].BaseIndex     = NumIndices;

        NumVertices += scene->mMeshes[i]->mNumVertices;
        NumIndices  += meshInfo[i].NumIndices;
    }

    this->InitMaterials(scene, Filename);
    std::cout << "Load ASSIMP: mNumMeshes: " << scene->mNumMeshes << std::endl;
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < scene->mNumMeshes; i++) {
        std::vector<VertexBoneData> localMeshBones;
        std::vector<Vertex3D> localMeshVertices;

        localMeshBones.resize(scene->mMeshes[i]->mNumVertices );

        this->LoadBones(i, scene->mMeshes[i], localMeshBones);

        for (unsigned int j = 0 ; j < scene->mMeshes[i]->mNumVertices ; j++) {
            Vertex3D v = Vertex3D();
            v.x = scene->mMeshes[i]->mVertices[j].x;
            v.y = scene->mMeshes[i]->mVertices[j].y;
            v.z = scene->mMeshes[i]->mVertices[j].z;

            const aiVector3D* pTexCoord = scene->mMeshes[i]->HasTextureCoords(0) ? &(scene->mMeshes[i]->mTextureCoords[0][j]) : &Zero3D;
            v.u = pTexCoord->x;
            v.v = pTexCoord->y;

            localMeshVertices.push_back(v);
        }

        this->meshVertices.push_back(localMeshVertices );
        this->meshVerticesBoneData.push_back(localMeshBones );

        std::cout << "Assimp: Mesh id: "<< i <<" with " << scene->mMeshes[i]->mNumVertices << " vertices / " << scene->mMeshes[i]->mNumBones << " bones / " << scene->mMeshes[i]->mNumFaces << " faces" << std::endl;
        for (unsigned int k = 0 ; k < scene->mMeshes[i]->mNumFaces ; k++) {
            const aiFace& Face = scene->mMeshes[i]->mFaces[k];
            Vertex3D V1 = localMeshVertices.at(Face.mIndices[0]);
            Vertex3D V2 = localMeshVertices.at(Face.mIndices[1]);
            Vertex3D V3 = localMeshVertices.at(Face.mIndices[2]);

            this->modelTriangles.push_back( new Triangle(V1, V2, V3, this) );
            if (this->numTextures > 0) {
                this->modelTriangles[k]->setTexture( &this->modelTextures[ scene->mMeshes[i]->mMaterialIndex ] );
            }
        }
    }

    return true;
}

void Mesh3DAnimated::LoadBones(uint MeshIndex, const aiMesh *pMesh, std::vector<VertexBoneData> &Bones)
{
    for (uint i = 0 ; i < pMesh->mNumBones ; i++) {
        uint BoneIndex = 0;
        std::string BoneName(pMesh->mBones[i]->mName.data);

        if (boneMapping.find(BoneName) == boneMapping.end()) {
            BoneIndex = numBones;
            numBones++;
            BoneInfo bi;
            boneInfo.push_back(bi);
        } else {
            BoneIndex = boneMapping[BoneName];
        }

        boneMapping[BoneName] = BoneIndex;
        boneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;

        for (uint j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) {
            uint VertexID = meshInfo[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}

aiMatrix4x4 Mesh3DAnimated::BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4> &Transforms)
{
    aiMatrix4x4 Identity;
    //Identity.InitIdentity();

    float TicksPerSecond = scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f;
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, scene->mAnimations[0]->mDuration);

    ReadNodeHeirarchy(AnimationTime, scene->mRootNode, Identity);

    Transforms.resize(numBones);

    for (uint i = 0 ; i < numBones ; i++) {
        Transforms[i] = boneInfo[i].FinalTransformation;
    }
}

void Mesh3DAnimated::ReadNodeHeirarchy(float AnimationTime, const aiNode *pNode, const aiMatrix4x4 &ParentTransform)
{
    std::string NodeName(pNode->mName.data);

    const aiAnimation* pAnimation = scene->mAnimations[0];

    aiMatrix4x4 NodeTransformation(pNode->mTransformation);

    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        aiMatrix4x4 ScalingM;
        ScalingM.Scaling(aiVector3D(Scaling.x, Scaling.y, Scaling.z), ScalingM);
        //ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        aiMatrix4x4 RotationM = aiMatrix4x4(RotationQ.GetMatrix());

        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        aiMatrix4x4 TranslationM;
        TranslationM.Translation(aiVector3D(Translation.x, Translation.y, Translation.z), TranslationM);
        //TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }

    aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;

    if (boneMapping.find(NodeName) != boneMapping.end()) {
        uint BoneIndex = boneMapping[NodeName];
        boneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * boneInfo[BoneIndex].BoneOffset;
    }

    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

void Mesh3DAnimated::CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}

void Mesh3DAnimated::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void Mesh3DAnimated::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    uint NextScalingIndex = (ScalingIndex + 1);
    assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

const aiNodeAnim *Mesh3DAnimated::FindNodeAnim(const aiAnimation *pAnimation, const std::string NodeName)
{
    for (uint i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return NULL;
}

uint Mesh3DAnimated::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (uint i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);
}

uint Mesh3DAnimated::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    for (uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}

uint Mesh3DAnimated::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}

void Mesh3DAnimated::updateForBone(Vertex3D &V, int meshID, int vertexID, std::vector<aiMatrix4x4> &Transforms)
{
    float u; float v;
    u = V.u;
    v = V.v;

    int BoneIDs[4]; float Weights[4];
    for (int n = 0 ; n < NUM_BONES_PER_VEREX ; n++) {
        BoneIDs[n] = this->meshVerticesBoneData[meshID][vertexID].IDs[n]; // boneID
        Weights[n] = this->meshVerticesBoneData[meshID][vertexID].Weights[n]; // WeightID
        //std::cout << "VertexID: " << vertexID << "n: " << n << ", BoneID: " << BoneIDs[n] << std::endl;
    }

    aiMatrix4x4 BoneTransform = Transforms[BoneIDs[0]] * Weights[0];
    BoneTransform = BoneTransform + Transforms[BoneIDs[1]] * Weights[1];
    BoneTransform = BoneTransform + Transforms[BoneIDs[2]] * Weights[2];
    BoneTransform = BoneTransform + Transforms[BoneIDs[3]] * Weights[3];

    aiVector3t<float> scaling;
    aiVector3t<float> position;
    aiQuaterniont<float> rotation;

    BoneTransform.Decompose(scaling, rotation, position);
    V.getScaled(scaling.x, scaling.y, scaling.z);

    V = this->convertAssimpM3(rotation.GetMatrix()) * V;
    V = V + Vertex3D(position.x, position.y, position.z);
    V.u = u;
    V.v = v;
}

M3 Mesh3DAnimated::convertAssimpM3(aiMatrix3x3 s)
{
    M3 r(
        s.a1, s.a2, s.a3,
        s.b1, s.b2, s.b3,
        s.c1, s.c2, s.c3
    );

    return r;
}

bool Mesh3DAnimated::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Extract the directory part from the file name
    std::string::size_type SlashIndex = Filename.find_last_of("/");
    std::string Dir;

    if (SlashIndex == std::string::npos) {
        Dir = ".";
    } else if (SlashIndex == 0) {
        Dir = "/";
    } else {
        Dir = Filename.substr(0, SlashIndex);
    }

    bool Ret = true;

    // Initialize the materials
    std::cout << "Load ASSIMP: mNumMaterials: " << pScene->mNumMaterials << std::endl;
    for (uint i = 0 ; i < pScene->mNumMaterials ; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];

        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;

            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string p(Path.data);

                if (p.substr(0, 2) == ".\\") {
                    p = p.substr(2, p.size() - 2);
                }

                std::string FullPath = Dir + "/" + p;

                std::cout << "Import texture " << FullPath << " for ASSIMP Mesh" << std::endl;
                Texture *t = new Texture();
                if (t->loadTGA(FullPath.c_str(), 1) ) {
                    this->modelTextures[ this->numTextures ] = *t;
                    this->numTextures++;
                }
            }
        }
    }

    return Ret;
}