#include "../../headers/Objects/Mesh3DAnimated.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/Render/Transforms.h"


void Mesh3DAnimated::onUpdate() {
    if (this->scene != nullptr) {
        this->updateFrameTransformations();
    }

    Mesh3D::onUpdate();
}

void Mesh3DAnimated::updateFrameTransformations() {
    if (!this->scene->HasAnimations()) return;

    if (this->isFollowCamera()) {
        this->setRotation(this->getRotation() * this->getFixedRotation());
    }

    // Update running time
    this->animation_ends = false;
    this->runningTime += Brakeza3D::get()->getDeltaTime() * this->animation_speed;
    float maxTime = this->scene->mAnimations[this->indexCurrentAnimation]->mDuration /
                    scene->mAnimations[indexCurrentAnimation]->mTicksPerSecond;

    if (runningTime >= maxTime) {
        if (this->isRemoveAtEndAnimation()) {
            this->getParent()->setRemoved(true);
            return;
        }

        this->animation_ends = true;
        this->runningTime = 0.000;
    }

    std::vector<aiMatrix4x4> Transforms;
    this->BoneTransform(runningTime, Transforms);

    int numModelTriangles = 0;
    for (int i = 0; i < this->scene->mNumMeshes; i++) {

        if (this->meshVertices[i].empty()) continue;

        // Apply bone transforms and create triangle
        for (unsigned int k = 0; k < this->scene->mMeshes[i]->mNumFaces; k++) {

            const aiFace &Face = this->scene->mMeshes[i]->mFaces[k];

            Vertex3D V1 = this->meshVertices[i].at(Face.mIndices[0]);
            Vertex3D V2 = this->meshVertices[i].at(Face.mIndices[1]);
            Vertex3D V3 = this->meshVertices[i].at(Face.mIndices[2]);

            this->updateForBone(V1, i, Face.mIndices[0], Transforms);
            this->updateForBone(V2, i, Face.mIndices[1], Transforms);
            this->updateForBone(V3, i, Face.mIndices[2], Transforms);

            auto *T = this->modelTriangles[numModelTriangles];

            T->A = V1;
            T->B = V2;
            T->C = V3;

            if (this->numTextures > 0) {
                T->setTexture(&this->modelTextures[this->scene->mMeshes[i]->mMaterialIndex]);
            }

            numModelTriangles++;
        }
    }

    // Update FollowPoint position
    if (this->getFollowPointNode() != nullptr) {
        this->updateFollowObjectPosition(Transforms);
    }

    if (EngineSetup::getInstance()->DRAW_ANIMATION_BONES) {
        this->drawBones(scene->mRootNode, Transforms);
    }
}

bool Mesh3DAnimated::AssimpLoadAnimation(const std::string &Filename) {
    Logging::Log("AssimpLoad for " + Filename, "Mesh3DAnimated");

    this->scene = importer.ReadFile(Filename, aiProcess_Triangulate |
                                              aiProcess_JoinIdenticalVertices |
                                              aiProcess_SortByPType |
                                              aiProcess_FlipUVs
    );

    if (!scene) {
        Logging::Log("Error import 3D file for ASSIMP", "Mesh3DAnimated");
        exit(-1);
        return false;
    }

    this->AssimpInitMaterials(scene, Filename);
    this->ReadNodes();

    return true;
}

void Mesh3DAnimated::ReadNodes() {
    m_GlobalInverseTransform = scene->mRootNode->mTransformation;
    m_GlobalInverseTransform.Inverse();

    this->meshVertices.resize(scene->mNumMeshes);
    this->meshVerticesBoneData.resize(scene->mNumMeshes);

    this->AssimpProcessNodeAnimation(scene->mRootNode);
    this->updateBoundingBox();
}

void Mesh3DAnimated::AssimpProcessMeshAnimation(int i, aiMesh *mesh) {
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    std::vector<VertexBoneData> localMeshBones(mesh->mNumVertices);
    std::vector<Vertex3D> localMeshVertices(mesh->mNumVertices);

    if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
        Logging::Log("Skip mesh non triangle", "Mesh3DAnimated");
        return;
    }

    std::cout << "Loading mesh animation with materialIndex: " << mesh->mMaterialIndex << std::endl;
    this->loadMeshBones(mesh, localMeshBones);

    for (unsigned int j = 0; j < mesh->mNumVertices; j++) {

        aiVector3t vf = mesh->mVertices[j];

        Vertex3D v(vf.x, vf.y, vf.z);

        const aiVector3D *pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][j]) : &Zero3D;
        v.u = pTexCoord->x;
        v.v = pTexCoord->y;

        localMeshVertices[j] = v;
    }

    this->meshVertices[i] = localMeshVertices;
    this->meshVerticesBoneData[i] = localMeshBones;

    for (unsigned int k = 0; k < mesh->mNumFaces; k++) {
        const aiFace &Face = mesh->mFaces[k];

        if (Face.mNumIndices < 3) continue;

        Vertex3D V1 = localMeshVertices.at(Face.mIndices[0]);
        Vertex3D V2 = localMeshVertices.at(Face.mIndices[1]);
        Vertex3D V3 = localMeshVertices.at(Face.mIndices[2]);

        this->modelTriangles.push_back(new Triangle(V1, V2, V3, this));

        if (this->numTextures > 0) {
            if (&this->modelTextures[mesh->mMaterialIndex] != nullptr) {
                this->modelTriangles[k]->setTexture(&this->modelTextures[mesh->mMaterialIndex]);
            }
        }
    }
}

void Mesh3DAnimated::loadMeshBones(aiMesh *mesh, std::vector<VertexBoneData> &meshVertexBoneData) {
    for (int i = 0; i < mesh->mNumBones; i++) {
        int BoneIndex;
        std::string BoneName(mesh->mBones[i]->mName.data);

        if (boneMapping.find(BoneName) == boneMapping.end()) {
            BoneIndex = m_NumBones;
            m_NumBones++;
            BoneInfo bi = BoneInfo();
            boneInfo.push_back(bi);

            boneMapping[BoneName] = BoneIndex;

            boneInfo[BoneIndex].BoneOffset = mesh->mBones[i]->mOffsetMatrix;
            boneInfo[BoneIndex].name = mesh->mBones[i]->mName.C_Str();
        } else {
            BoneIndex = (int) boneMapping[BoneName];
        }

        //Logging::Log(std::string("Loading Bone info for BoneIndex: ") + std::to_string(BoneIndex) + ": " + mesh->mBones[i]->mName.C_Str());

        for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
            unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = mesh->mBones[i]->mWeights[j].mWeight;

            //Logging::Log(std::to_string(j) + " - AddBoneData for VertexID: " + std::to_string(VertexID) + " in idMesh: " + std::string(mesh->mName.C_Str()) + ", BoneIndex: " + std::to_string(BoneIndex) + ", Weight: " + std::to_string(Weight));
            meshVertexBoneData[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}

void Mesh3DAnimated::BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4> &Transforms) {
    aiMatrix4x4 Identity = aiMatrix4x4();

    float TicksPerSecond = scene->mAnimations[indexCurrentAnimation]->mTicksPerSecond != 0
                           ? scene->mAnimations[indexCurrentAnimation]->mTicksPerSecond : 0.25f;
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, scene->mAnimations[indexCurrentAnimation]->mDuration);

    ReadNodeHeirarchy(AnimationTime, scene->mRootNode, Identity);

    Transforms.resize(this->m_NumBones);

    for (uint i = 0; i < this->m_NumBones; i++) {
        Transforms[i] = boneInfo[i].FinalTransformation;
    }
}

void Mesh3DAnimated::ReadNodeHeirarchy(float AnimationTime, const aiNode *pNode, const aiMatrix4x4 &ParentTransform) {
    std::string NodeName(pNode->mName.data);
    const aiAnimation *pAnimation = scene->mAnimations[this->indexCurrentAnimation];

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
        uint BoneIndex = boneMapping[NodeName];
        boneInfo[BoneIndex].FinalTransformation =
                m_GlobalInverseTransform * GlobalTransformation * boneInfo[BoneIndex].BoneOffset;
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

const aiNodeAnim *Mesh3DAnimated::FindNodeAnim(const aiAnimation *pAnimation, const std::string& NodeName) {
    for (uint i = 0; i < pAnimation->mNumChannels; i++) {
        const aiNodeAnim *pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return nullptr;
}

uint Mesh3DAnimated::FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim) {
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

//    assert(0);
    return 0;
}

uint Mesh3DAnimated::FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim) {
    for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    //assert(0);

    return 0;
}

int Mesh3DAnimated::updateForBone(Vertex3D &V, int meshID, int vertexID, std::vector<aiMatrix4x4> &Transforms) {
    int numVertexBones = 0;

    if (this->m_NumBones == 0) return 0;

    int BoneIDs[NUM_BONES_PER_VERTEX] = {-1};
    float Weights[NUM_BONES_PER_VERTEX] = {-1};

    for (int n = 0; n < NUM_BONES_PER_VERTEX; n++) {
        BoneIDs[n] = this->meshVerticesBoneData[meshID][vertexID].IDs[n];     // boneID
        Weights[n] = this->meshVerticesBoneData[meshID][vertexID].Weights[n]; // WeightID
        if (BoneIDs[n] > 0) {
            numVertexBones++;
        }
    }

    aiMatrix4x4 BoneTransform;
    BoneTransform = BoneTransform * 0;
    for (int n = 0; n < NUM_BONES_PER_VERTEX; n++) {
        int boneId = BoneIDs[n];
        float weight = Weights[n];

        if (boneId != -1) {
            BoneTransform = BoneTransform + Transforms[boneId] * weight;
        }
    }

    Mesh3DAnimated::AIMatrixToVertex(V, BoneTransform);

    return numVertexBones;
}

uint Mesh3DAnimated::FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim) {
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

void Mesh3DAnimated::AssimpProcessNodeAnimation(aiNode *node) {
    if (std::string(node->mName.C_Str()) == this->getFollowPointLabel()) {
        this->setFollowPointNode(node);
        this->follow_me_point_object = new Object3D();
        this->follow_me_point_object->setPosition(this->getPosition());
        this->follow_me_point_object->setRotation(M3::getMatrixIdentity());
    }

    for (unsigned int x = 0; x < node->mNumMeshes; x++) {
        int idMesh = node->mMeshes[x];
        this->AssimpProcessMeshAnimation(idMesh, scene->mMeshes[idMesh]);
    }

    for (unsigned int j = 0; j < node->mNumChildren; j++) {
        AssimpProcessNodeAnimation(node->mChildren[j]);
    }
}

void Mesh3DAnimated::AIMatrixToVertex(Vertex3D &V, aiMatrix4x4 &m) {
    aiVector3D aiv(V.x, V.y, V.z);
    aiv = m * aiv;

    V.x = aiv.x;
    V.y = aiv.y;
    V.z = aiv.z;
}

void Mesh3DAnimated::CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime, const aiNodeAnim *pNodeAnim) {
    // we need at least two values to interpolate...
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
    uint NextRotationIndex = (RotationIndex + 1);
    //assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
    float Factor = (AnimationTime - (float) pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion &StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion &EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;

    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);

    Out = Out.Normalize();
}

void Mesh3DAnimated::CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim) {
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    uint NextPositionIndex = (PositionIndex + 1);
    //assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    auto DeltaTime = (float) (pNodeAnim->mPositionKeys[NextPositionIndex].mTime -
                               pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float) pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    //assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D &Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D &End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}


void Mesh3DAnimated::CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim) {
    if (pNodeAnim->mNumScalingKeys == 1) {
        Out = pNodeAnim->mScalingKeys[0].mValue;
        return;
    }

    uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
    uint NextScalingIndex = (ScalingIndex + 1);
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

void Mesh3DAnimated::drawBones(aiNode *node, std::vector<aiMatrix4x4> &Transforms) {
    Uint32 colorPoints = Color::red();

    int idCurrentNode;

    if (boneMapping.find(node->mName.C_Str()) != boneMapping.end()) {
        idCurrentNode = boneMapping[node->mName.C_Str()];

        Vertex3D bonePosition;
        aiMatrix4x4 mOffset = boneInfo[idCurrentNode].BoneOffset;
        aiMatrix4x4 mT = Transforms[idCurrentNode];

        this->AIMatrixToVertex(bonePosition, mOffset.Inverse());
        this->AIMatrixToVertex(bonePosition, mT);
        Transforms::objectSpace(bonePosition, bonePosition, this);

        Drawable::drawVertex(bonePosition, Brakeza3D::get()->getComponentsManager()->getComponentCamera()->getCamera(),
                             colorPoints);
    }

    for (int j = 0; j < node->mNumChildren; j++) {
        drawBones(node->mChildren[j], Transforms);
    }
}

bool Mesh3DAnimated::isRemoveAtEndAnimation() const {
    return remove_at_end_animation;
}

void Mesh3DAnimated::setRemoveAtEndAnimation(bool removeAtEnds) {
    remove_at_end_animation = removeAtEnds;
}

aiNode *Mesh3DAnimated::getFollowPointNode() const {
    return follow_me_point_node;
}

void Mesh3DAnimated::setFollowPointNode(aiNode *followPointOrigin) {
    follow_me_point_node = followPointOrigin;

    std::string BoneName = follow_me_point_node->mName.C_Str();
    if (boneMapping.find(BoneName) == boneMapping.end()) {
        unsigned int BoneIndex = boneMapping.size();
        m_NumBones++;
        BoneInfo bi;
        bi.BoneOffset = follow_me_point_node->mTransformation;
        bi.name = BoneName;
        boneInfo.push_back(bi);
        boneMapping[BoneName] = BoneIndex;
    }
}

void Mesh3DAnimated::updateFollowObjectPosition(std::vector<aiMatrix4x4> Transforms) {
    if (boneMapping.find(this->follow_me_point_label) != boneMapping.end()) {
        int idCurrentNode = boneMapping[this->follow_me_point_node->mName.C_Str()];

        Vertex3D p;
        aiMatrix4x4 m = boneInfo[idCurrentNode].BoneOffset;
        aiMatrix4x4 m2 = Transforms[idCurrentNode];

        this->AIMatrixToVertex(p, m.Inverse());
        this->AIMatrixToVertex(p, m2);
        Transforms::objectSpace(p, p, this);

        this->getFollowMePointObject()->setPosition(p);

        aiMatrix4x4 t = m2;

        aiVector3D scale;
        aiQuaternion rotation;
        aiVector3D translation;
        t.Decompose(scale, rotation, translation);

        aiMatrix3x3 m3 = rotation.GetMatrix();
        M3 nm(
                m3.a1, m3.a2, m3.a3,
                m3.b1, m3.b2, m3.b3,
                m3.c1, m3.c2, m3.c3
        );

        M3 fixedRotation = M3::getMatrixRotationForEulerAngles(0, 180, 0); // :?
        this->getFollowMePointObject()->setRotation(fixedRotation * (this->getRotation() * nm).getTranspose());
    }
}

const std::string &Mesh3DAnimated::getFollowPointLabel() const {
    return follow_me_point_label;
}

void Mesh3DAnimated::setFollowPointLabel(const std::string &followPointLabel) {
    follow_me_point_label = followPointLabel;
}

Object3D *Mesh3DAnimated::getFollowMePointObject() const {
    return follow_me_point_object;
}

const M3 &Mesh3DAnimated::getFixedRotation() const {
    return fixedRotation;
}

void Mesh3DAnimated::setFixedRotation(const M3 &fixedRotation) {
    Mesh3DAnimated::fixedRotation = fixedRotation;
}

bool Mesh3DAnimated::isAnimationEnds() const {
    return animation_ends;
}