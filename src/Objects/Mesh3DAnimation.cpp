#include "../../include/Objects/Mesh3DAnimation.h"
#include "../../include/Brakeza3D.h"

Mesh3DAnimation::Mesh3DAnimation()
:
    scene(nullptr),
    numBones(0),
    indexCurrentAnimation(0),
    runningTime(0),
    remove_at_end_animation(false),
    animation_speed(1),
    animation_ends(false)
{
}

void Mesh3DAnimation::onUpdate()
{
    if (isRemoved() || scene == nullptr) return;

    updateFrameTransformations();
    updateOGLBuffers();

    Mesh3D::onUpdate();
}

void Mesh3DAnimation::updateFrameTransformations()
{
    if (isRemoved() || scene == nullptr) return;

    if (!scene->HasAnimations()) return;

    animation_ends = false;
    runningTime += Brakeza3D::get()->getDeltaTime() * animation_speed;
    auto maxTime = (float) (scene->mAnimations[indexCurrentAnimation]->mDuration / scene->mAnimations[indexCurrentAnimation]->mTicksPerSecond);

    if (runningTime >= maxTime) {
        if (isRemoveAtEndAnimation()) {
            getParent()->setRemoved(true);
            return;
        }
        animation_ends = true;
        runningTime = 0.000;
    }

    std::vector<aiMatrix4x4> Transforms;
    BoneTransform(runningTime, Transforms);

    int numModelTriangles = 0;
    for (int i = 0; i < (int) scene->mNumMeshes; i++) {

        if (meshVertices[i].empty()) continue;

        for (unsigned int k = 0; k < scene->mMeshes[i]->mNumFaces; k++) {

            const aiFace &Face = scene->mMeshes[i]->mFaces[k];

            Vertex3D V1 = meshVertices[i].at(Face.mIndices[0]);
            Vertex3D V2 = meshVertices[i].at(Face.mIndices[1]);
            Vertex3D V3 = meshVertices[i].at(Face.mIndices[2]);

            updateForBone(V1, i, Face.mIndices[0], Transforms);
            updateForBone(V2, i, Face.mIndices[1], Transforms);
            updateForBone(V3, i, Face.mIndices[2], Transforms);

            auto *T = meshes[i].modelTriangles[numModelTriangles];

            T->A = V1;
            T->B = V2;
            T->C = V3;

            numModelTriangles++;
        }
    }

    if (EngineSetup::get()->DRAW_ANIMATION_BONES) {
        this->drawBones(scene->mRootNode, Transforms, nullptr);
    }
}

bool Mesh3DAnimation::AssimpLoadAnimation(const std::string &filename)
{
    Logging::Message("AssimpLoadAnimation for %s", filename.c_str());

    if (!Tools::fileExists(filename.c_str())) {
        Logging::Message("[AssimpLoadAnimation] ERROR: File not found %s", filename.c_str());
        return false;
    }

    scene = importer.ReadFile(
        filename,
        aiProcess_Triangulate |
        aiProcess_SortByPType |
        aiProcess_FlipUVs|
        aiProcess_GenNormals
    );

    if (!scene) {
        Logging::Message("Error import 3D file for ASSIMP");
        exit(-1);
    }

    AssimpInitMaterials(scene, filename);
    ReadNodesFromRoot();

    fillBuffers();
    setSourceFile(filename);

    return true;
}

void Mesh3DAnimation::ReadNodesFromRoot()
{
    // Transformación raíz
    globalInverseTransform = scene->mRootNode->mTransformation;
    globalInverseTransform.Inverse();
    meshes.resize(scene->mNumMeshes);

    // Para cada malla preparamos sus array de "Vertices" y "VertexBoneData"
    meshVertices.resize(scene->mNumMeshes);
    meshVerticesBoneData.resize(scene->mNumMeshes);

    // Procesamos los nodos desde el raíz
    ProcessNodeAnimation(scene->mRootNode);
}

void Mesh3DAnimation::ProcessNodeAnimation(aiNode *node)
{
    // Procesamos las mallas del nodo dado
    for (unsigned int x = 0; x < node->mNumMeshes; x++) {
        int idMesh = (int) node->mMeshes[x];
        this->ProcessMeshAnimation(idMesh, scene->mMeshes[idMesh]);
    }

    // Continuamos recursivamente por los hijos del nodo dado
    for (unsigned int j = 0; j < node->mNumChildren; j++) {
        ProcessNodeAnimation(node->mChildren[j]);
    }
}

void Mesh3DAnimation::ProcessMeshAnimation(int i, aiMesh *mesh)
{
    if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
        Logging::Message("Skip mesh non triangle");
        return;
    }

    std::cout << "Loading mesh animation with materialIndex: " << mesh->mMaterialIndex << std::endl;

    std::vector<VertexBoneData> localMeshBones(mesh->mNumVertices);
    std::vector<Vertex3D> localMeshVertices(mesh->mNumVertices);

    LoadMeshBones(i, mesh, localMeshBones);
    LoadMeshVertex(i, mesh, localMeshVertices);

    meshVertices[i] = localMeshVertices;
    meshVerticesBoneData[i] = localMeshBones;

    for (unsigned int k = 0; k < mesh->mNumFaces; k++) {
        const aiFace &Face = mesh->mFaces[k];

        if (Face.mNumIndices < 3) continue;

        Vertex3D V1 = localMeshVertices.at(Face.mIndices[0]);
        Vertex3D V2 = localMeshVertices.at(Face.mIndices[1]);
        Vertex3D V3 = localMeshVertices.at(Face.mIndices[2]);

        meshes[i].modelTriangles.push_back(new Triangle(V1, V2, V3, this));
    }
}

void Mesh3DAnimation::LoadMeshBones(int meshId, aiMesh *mesh, std::vector<VertexBoneData> &meshVertexBoneData)
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

        //Logging::Message(std::string("Loading Bone info for BoneIndex: ") + std::to_string(BoneIndex) + ": " + mesh->mBones[i]->mName.C_Str());

        for (int j = 0; j < (int) mesh->mBones[i]->mNumWeights; j++) {
            unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = mesh->mBones[i]->mWeights[j].mWeight;

            //Logging::Message(std::to_string(j) + " - AddBoneData for VertexID: " + std::to_string(VertexID) + " in idMesh: " + std::string(mesh->mName.C_Str()) + ", BoneIndex: " + std::to_string(BoneIndex) + ", Weight: " + std::to_string(Weight));
            meshVertexBoneData[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}

void Mesh3DAnimation::LoadMeshVertex(int meshId, aiMesh *mesh, std::vector<Vertex3D> &meshVertex)
{
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
        aiVector3t vf = mesh->mVertices[j];
        Vertex3D v(vf.x, vf.y, vf.z);

        const aiVector3D *pTexCoord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][j]) : &Zero3D;
        v.u = pTexCoord->x;
        v.v = pTexCoord->y;

        meshVertex[j] = v;

        // OpenGL
        meshes[meshId].vertices.emplace_back(vf.x, vf.y, vf.z);
        meshes[meshId].uvs.emplace_back(v.u, v.v);

        aiVector3t n = mesh->mNormals[j];
        meshes[meshId].normals.emplace_back(n.x, n.y, n.z);
    }
}

void Mesh3DAnimation::BoneTransform(float TimeInSeconds, std::vector<aiMatrix4x4> &Transforms)
{
    aiMatrix4x4 Identity = aiMatrix4x4();

    float TicksPerSecond = scene->mAnimations[indexCurrentAnimation]->mTicksPerSecond != 0
                           ? scene->mAnimations[indexCurrentAnimation]->mTicksPerSecond : 0.25f;
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    auto AnimationTime = (float) fmod(TimeInTicks, scene->mAnimations[indexCurrentAnimation]->mDuration);

    ReadNodeHierarchy(AnimationTime, scene->mRootNode, Identity);

    Transforms.resize(this->numBones);

    for (int i = 0; i < (int) this->numBones; i++) {
        Transforms[i] = boneInfo[i].FinalTransformation;
    }
}

void Mesh3DAnimation::ReadNodeHierarchy(float AnimationTime, const aiNode *pNode, const aiMatrix4x4 &ParentTransform)
{
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
        unsigned int BoneIndex = boneMapping[NodeName];
        boneInfo[BoneIndex].FinalTransformation =
                globalInverseTransform * GlobalTransformation * boneInfo[BoneIndex].BoneOffset;
    }

    for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
        ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

const aiNodeAnim *Mesh3DAnimation::FindNodeAnim(const aiAnimation *pAnimation, const std::string& NodeName)
{
    for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
        const aiNodeAnim *pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return nullptr;
}

unsigned int Mesh3DAnimation::FindRotation(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

//    assert(0);
    return 0;
}

unsigned int Mesh3DAnimation::FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    //assert(0);

    return 0;
}

int Mesh3DAnimation::updateForBone(Vertex3D &V, int meshID, int vertexID, std::vector<aiMatrix4x4> &Transforms)
{
    int numVertexBones = 0;

    if (this->numBones == 0) return 0;

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

    V = Vertex3D::fromAssimp(BoneTransform * V.toAssimp());

    return numVertexBones;
}

unsigned int Mesh3DAnimation::FindScaling(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    assert(pNodeAnim->mNumScalingKeys > 0);

    for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mScalingKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

void Mesh3DAnimation::CalcInterpolatedRotation(aiQuaternion &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
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

void Mesh3DAnimation::CalcInterpolatedPosition(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
    unsigned int NextPositionIndex = (PositionIndex + 1);
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


void Mesh3DAnimation::CalcInterpolatedScaling(aiVector3D &Out, float AnimationTime, const aiNodeAnim *pNodeAnim)
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

void Mesh3DAnimation::drawBones(aiNode *node, std::vector<aiMatrix4x4> &Transforms, Vertex3D *lastBonePosition)
{
    int idCurrentNode;

    if (boneMapping.find(node->mName.C_Str()) != boneMapping.end()) {
        idCurrentNode = boneMapping[node->mName.C_Str()];

        aiMatrix4x4 mOffset = boneInfo[idCurrentNode].BoneOffset;
        aiMatrix4x4 mT = Transforms[idCurrentNode];
        aiVector3D aBonePosition;

        aBonePosition = mOffset.Inverse() * aBonePosition;
        aBonePosition = mT * aBonePosition;

        Vertex3D bonePosition = Vertex3D::fromAssimp(aBonePosition);
        Transforms::objectSpace(bonePosition, bonePosition, this);

        if (lastBonePosition) {
            auto render = ComponentsManager::get()->getComponentRender();
            auto window = ComponentsManager::get()->getComponentWindow();
            render->getShaderOGLLine3D()->render(
                *lastBonePosition,
                bonePosition,
                window->getForegroundFramebuffer(),
                Color::yellow()
            );
        }

        lastBonePosition = &bonePosition;
    }

    for (int j = 0; j < (int) node->mNumChildren; j++) {
        drawBones(node->mChildren[j], Transforms, lastBonePosition);
    }
}

bool Mesh3DAnimation::isRemoveAtEndAnimation() const
{
    return remove_at_end_animation;
}

void Mesh3DAnimation::setRemoveAtEndAnimation(bool removeAtEnds)
{
    remove_at_end_animation = removeAtEnds;
}

Mesh3DAnimation* Mesh3DAnimation::create(Vertex3D position, const std::string& animationFile)
{
    auto o = new Mesh3DAnimation();
    o->setPosition(position);

    o->AssimpLoadAnimation(animationFile);

    return o;
}

const char *Mesh3DAnimation::getTypeObject()
{
    return "Mesh3DAnimation";
}

const char *Mesh3DAnimation::getTypeIcon()
{
    return "Mesh3DAnimationIcon";
}

void Mesh3DAnimation::updateOGLBuffers()
{
    for (auto &m: meshes) {
        m.vertices.clear();
        for (auto t: m.modelTriangles) {
            m.vertices.emplace_back(t->A.x, t->A.y, t->A.z);
            m.vertices.emplace_back(t->B.x, t->B.y, t->B.z);
            m.vertices.emplace_back(t->C.x, t->C.y, t->C.z);
        }

        if (glIsBuffer(m.vertexbuffer)) {
            glDeleteBuffers(1, &m.vertexbuffer);
        }

        glGenBuffers(1, &m.vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m.vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(glm::vec3), &m.vertices[0], GL_STATIC_DRAW);
    }
}

void Mesh3DAnimation::drawImGuiProperties()
{
    Mesh3D::drawImGuiProperties();

    const float range_min = 0.0f;
    const float range_max = 1.0f;

    ImGui::Separator();

    if (ImGui::TreeNode("Mesh3DAnimation")) {
        auto flags = ImGuiComboFlags_None;

        const char* items[(int) scene->mNumAnimations];
        for (int i = 0; i < (int) scene->mNumAnimations; i++) {
            items[i] = scene->mAnimations[i]->mName.C_Str();
        }
        auto comboTitle = "Animations##" + getLabel();
        ImGui::Combo("Animation", &indexCurrentAnimation, items, IM_ARRAYSIZE(items));

        ImGui::Separator();
        ImGui::DragScalar("Speed", ImGuiDataType_Float, &animation_speed, 0.01f ,&range_min, &range_max, "%f", 1.0f);
        ImGui::TreePop();
    }
}

cJSON *Mesh3DAnimation::getJSON()
{
    cJSON *root = Mesh3D::getJSON();

    cJSON_AddNumberToObject(root, "animationSpeed", animation_speed);

    return root;
}

void Mesh3DAnimation::createFromJSON(cJSON *object)
{
    auto o = new Mesh3DAnimation();

    Mesh3DAnimation::setPropertiesFromJSON(object, o);

    o->AssimpLoadAnimation(cJSON_GetObjectItemCaseSensitive(object, "model")->valuestring);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

void Mesh3DAnimation::setPropertiesFromJSON(cJSON *object, Mesh3DAnimation *o)
{
    Object3D::setPropertiesFromJSON(object, o);

    auto speed = cJSON_GetObjectItemCaseSensitive(object, "animationSpeed")->valuedouble;
    o->setAnimationSpeed(speed);
    Logging::Message("Speed %f", speed);

}

void Mesh3DAnimation::setAnimationSpeed(float animationSpeed) {
    animation_speed = animationSpeed;
}

Mesh3DAnimation::~Mesh3DAnimation()
{
    meshVerticesBoneData.clear();
    meshVertices.clear();
    boneMapping.clear();
    boneInfo.clear();
}

void Mesh3DAnimation::setIndexCurrentAnimation(int indexCurrentAnimation)
{
    Mesh3DAnimation::indexCurrentAnimation = indexCurrentAnimation;
}
