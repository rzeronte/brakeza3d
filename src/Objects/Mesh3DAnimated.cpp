//
// Created by darkhead on 13/2/20.
//

#include "../../headers/Objects/Mesh3DAnimated.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/Render/Transforms.h"

bool Mesh3DAnimated::AssimpLoad(const std::string &Filename)
{
    this->scene = importer.ReadFile( Filename,aiProcess_CalcTangentSpace       |
                                              aiProcess_Triangulate            |
                                              aiProcess_JoinIdenticalVertices  |
                                              aiProcess_SortByPType );

    if( !scene ) {
        Logging::getInstance()->Log("Error import 3D file for ASSIMP");
        exit(-1);
        return false;
    }

    m_GlobalInverseTransform = scene->mRootNode->mTransformation;
    m_GlobalInverseTransform.Inverse();

    this->meshVertices.resize(scene->mNumMeshes);
    this->meshVerticesBoneData.resize(scene->mNumMeshes);

    this->processNode(scene->mRootNode);
    this->InitMaterials(scene, Filename);

    return true;
}


void Mesh3DAnimated::processMesh(int idMesh, aiMesh *mesh)
{
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    std::vector<VertexBoneData> localMeshBones(mesh->mNumVertices);
    std::vector<Vertex3D> localMeshVertices(mesh->mNumVertices);

    if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
        Logging::getInstance()->Log("Skip mesh non triangle");
        return;
    }

    std::cout << std::endl;

    this->loadMeshBones(mesh, localMeshBones);

    for (unsigned int j = 0 ; j < mesh->mNumVertices ; j++) {

        aiVector3t vf = mesh->mVertices[j];

        Vertex3D v( vf.x, vf.y, vf.z );

        const aiVector3D* pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][j]) : &Zero3D;
        v.u = pTexCoord->x;
        v.v = pTexCoord->y;

        localMeshVertices[j] = v;
    }

    this->meshVertices[ idMesh ] = localMeshVertices;
    this->meshVerticesBoneData[ idMesh ] = localMeshBones;

    std::cout << "Assimp: Mesh (" << mesh->mName.C_Str() << ") | id: "<< idMesh <<" with " << mesh->mNumVertices << " vertices / " << mesh->mNumBones << " bones / " << mesh->mNumFaces << " faces";

    for (unsigned int k = 0 ; k < mesh->mNumFaces ; k++) {
        const aiFace& Face = mesh->mFaces[k];

        if (Face.mNumIndices < 3) continue;

        Vertex3D V1 = localMeshVertices.at(Face.mIndices[0]);
        Vertex3D V2 = localMeshVertices.at(Face.mIndices[1]);
        Vertex3D V3 = localMeshVertices.at(Face.mIndices[2]);

        this->modelTriangles.push_back( new Triangle(V1, V2, V3, this) );

        if (this->numTextures > 0) {
            this->modelTriangles[k]->setTexture( &this->modelTextures[ mesh->mMaterialIndex ] );
        }
    }
}

void Mesh3DAnimated::loadMeshBones(aiMesh *mesh, std::vector<VertexBoneData> &meshVertexBoneData)
{
    for (int i = 0 ; i < mesh->mNumBones ; i++) {
        uint BoneIndex = 0;
        std::string BoneName(mesh->mBones[i]->mName.data);

        if (boneMapping.find(BoneName) == boneMapping.end()) {
            BoneIndex = m_NumBones;
            m_NumBones++;
            BoneInfo bi;
            boneInfo.push_back(bi);

            boneMapping[BoneName] = BoneIndex;

            boneInfo[BoneIndex].BoneOffset = mesh->mBones[i]->mOffsetMatrix;
            boneInfo[BoneIndex].name = mesh->mBones[i]->mName.C_Str();

        } else {
            BoneIndex = boneMapping[BoneName];
        }

        Logging::getInstance()->Log(std::string("Loading Bone info for BoneIndex: ") + std::to_string(BoneIndex) + ": " + mesh->mBones[i]->mName.C_Str());

        for (int j = 0 ; j < mesh->mBones[i]->mNumWeights ; j++) {
            unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = mesh->mBones[i]->mWeights[j].mWeight;

            //Logging::getInstance()->Log(std::to_string(j) + " - AddBoneData for VertexID: " + std::to_string(VertexID) + " in idMesh: " + std::string(mesh->mName.C_Str()) + ", BoneIndex: " + std::to_string(BoneIndex) + ", Weight: " + std::to_string(Weight));
            meshVertexBoneData[ VertexID ].AddBoneData(BoneIndex, Weight);
        }
    }
}

aiMatrix4x4 Mesh3DAnimated::BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4> &Transforms)
{
    aiMatrix4x4 Identity = aiMatrix4x4();

    float TicksPerSecond = scene->mAnimations[ indexCurrentAnimation ]->mTicksPerSecond != 0 ? scene->mAnimations[ indexCurrentAnimation ]->mTicksPerSecond : 0.25f;
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, scene->mAnimations[ indexCurrentAnimation ]->mDuration);

    ReadNodeHeirarchy(AnimationTime, scene->mRootNode, Identity);

    Transforms.resize( this->m_NumBones );

    for (uint i = 0 ; i < this->m_NumBones ; i++) {
        Transforms[i] = boneInfo[i].FinalTransformation;

        if (EngineSetup::getInstance()->DRAW_ANIMATION_BONES) {
            aiMatrix4x4 mOffset = boneInfo[i].BoneOffset;

            Vertex3D bonePosition;
            this->AIMatrixToVertex(bonePosition, mOffset.Inverse());
            this->AIMatrixToVertex(bonePosition, Transforms[i] );

            Transforms::objectSpace(bonePosition, bonePosition, this);

            Drawable::drawVertex(bonePosition, Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera(), Color::red());
        }
    }
}

void Mesh3DAnimated::ReadNodeHeirarchy(float AnimationTime, const aiNode *pNode, const aiMatrix4x4 &ParentTransform)
{
    std::string NodeName(pNode->mName.data);
    const aiAnimation* pAnimation = scene->mAnimations[ this->indexCurrentAnimation ];

    aiMatrix4x4 NodeTransformation;
    NodeTransformation = pNode->mTransformation;

    const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

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


    if ( boneMapping.find(NodeName) != boneMapping.end() ) {
        uint BoneIndex = boneMapping[NodeName];
        boneInfo[ BoneIndex ].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * boneInfo[BoneIndex].BoneOffset;
    }

    for (unsigned int i = 0 ; i < pNode->mNumChildren ; i++) {
        ReadNodeHeirarchy( AnimationTime, pNode->mChildren[i], GlobalTransformation );
    }
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

//    assert(0);
}

uint Mesh3DAnimated::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    for (uint i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    //assert(0);

    return 0;
}

void Mesh3DAnimated::updateForBone(Vertex3D &V, int meshID, int vertexID, std::vector<aiMatrix4x4> &Transforms)
{
    int   BoneIDs[NUM_BONES_PER_VERTEX] = {0};
    float Weights[NUM_BONES_PER_VERTEX] = {0};

    for (int n = 0 ; n < NUM_BONES_PER_VERTEX ; n++) {
        BoneIDs[n] = this->meshVerticesBoneData[meshID][vertexID].IDs[n];     // boneID
        Weights[n] = this->meshVerticesBoneData[meshID][vertexID].Weights[n]; // WeightID
    }

    aiMatrix4x4 BoneTransform;
    BoneTransform = BoneTransform * 0;
    for (int n = 0 ; n < NUM_BONES_PER_VERTEX ; n++) {
        int   boneId = BoneIDs[n];
        float weight = Weights[n];

        BoneTransform = BoneTransform + Transforms[boneId] * weight;
    }

    this->AIMatrixToVertex(V, BoneTransform);
}

uint Mesh3DAnimated::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (uint i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

void Mesh3DAnimated::onUpdate()
{
    if (this->scene->mNumAnimations <= 0) return;

    // Update running time
    this->runningTime += Brakeza3D::get()->getDeltaTime();
    if (runningTime >= this->scene->mAnimations[ this->indexCurrentAnimation ]->mDuration) {
        runningTime = 0.000;
    }

    // update geometry
    this->modelTriangles.clear();

    std::vector<aiMatrix4x4> Transforms;
    this->BoneTransform(runningTime, Transforms);

    for (int i = 0; i < this->scene->mNumMeshes; i++) {

        // Apply bone transforms and create triangle
        for (unsigned int k = 0 ; k < this->scene->mMeshes[i]->mNumFaces ; k++) {

            if (this->meshVertices[i].size() == 0) continue;

            const aiFace& Face = this->scene->mMeshes[i]->mFaces[k];

            Vertex3D V1 = this->meshVertices[i].at(Face.mIndices[0]);
            Vertex3D V2 = this->meshVertices[i].at(Face.mIndices[1]);
            Vertex3D V3 = this->meshVertices[i].at(Face.mIndices[2]);

            this->updateForBone(V1, i, Face.mIndices[0], Transforms);
            this->updateForBone(V2, i, Face.mIndices[1], Transforms);
            this->updateForBone(V3, i, Face.mIndices[2], Transforms);

            this->modelTriangles.push_back( new Triangle(V1, V2, V3, this) );
            if (this->numTextures > 0) {
                this->modelTriangles[k]->setTexture( &this->modelTextures[ this->scene->mMeshes[i]->mMaterialIndex ] );
            }
        }
    }
}

void Mesh3DAnimated::processNode(aiNode *node)
{
    for (int x = 0; x < node->mNumMeshes; x++) {
        int idMesh = node->mMeshes[x];
        this->processMesh( idMesh, scene->mMeshes[ idMesh ] );
    }

    for (int j = 0; j < node->mNumChildren; j++) {
        processNode( node->mChildren[j] );
    }
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

void Mesh3DAnimated::AIMatrixToVertex(Vertex3D &V, aiMatrix4x4 &m)
{
    aiVector3D aiv(V.x, V.y, V.z);
    aiv = m * aiv;

    V.x = aiv.x;
    V.y = aiv.y;
    V.z = aiv.z;
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
    //assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
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
    //assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
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
    //assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
    float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
    const aiVector3D& End   = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}