#include "../../include/Objects/Mesh3DAnimation.h"
#include "../../include/Brakeza3D.h"

Mesh3DAnimation::Mesh3DAnimation()
:
    scene(nullptr),
    numBones(0),
    indexCurrentAnimation(0),
    runningTime(0),
    remove_at_end_animation(false),
    animation_speed(1)
{
}

void Mesh3DAnimation::onUpdate()
{
    if (isRemoved() || scene == nullptr) return;

    UpdateFrameTransformations();

    UpdateOpenGLBones();

    Mesh3D::onUpdate();

    if (EngineSetup::get()->DRAW_ANIMATION_BONES) {
        drawBones(scene->mRootNode, nullptr);
    }
}

void Mesh3DAnimation::UpdateOpenGLBones()
{
    fillAnimationBuffers();

    std::vector<glm::mat4> transformations(MAX_BONES, glm::mat4(0));

    for (int i = 0; i < (int) boneInfo.size(); i++) {
        transformations[i] = Tools::aiMat4toGLMMat4(boneInfo[i].FinalTransformation);
    }

    for (auto &m: meshes) {
        ComponentsManager::get()->getComponentRender()->getShaderOGLBonesTransforms()->render(
            m,
            transformations,
            ComponentsManager::get()->getComponentWindow()->getSceneFramebuffer()
        );
    }
}

void Mesh3DAnimation::UpdateFrameTransformations()
{
    if (isRemoved() || scene == nullptr || !scene->HasAnimations()) return;

    runningTime += Brakeza3D::get()->getDeltaTime() * animation_speed;

    checkIfEndAnimation();

    if (isAnimationEnds() && isRemoveAtEndAnimation()) {
        getParent()->setRemoved(true);
        return;
    }

    UpdateBonesFinalTransformations(runningTime);
}

void Mesh3DAnimation::checkIfEndAnimation()
{
    animation_ends = false;
    if (runningTime >= getCurrentAnimationMaxTime()) {
        animation_ends = true;
        runningTime = 0.000;
    }
}

float Mesh3DAnimation::getCurrentAnimationMaxTime() const
{
    return (float) (
        scene->mAnimations[indexCurrentAnimation]->mDuration / scene->mAnimations[indexCurrentAnimation]->mTicksPerSecond
    );
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
        aiProcess_GenNormals |
        aiProcess_OptimizeMeshes |
        aiProcess_FlipUVs
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
    meshVerticesBoneDataOrdered.resize(scene->mNumMeshes);

    // Procesamos los nodos desde el nodo raíz
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
    std::vector<Vertex3D> localMeshNormals(mesh->mNumVertices);

    LoadMeshBones(i, mesh, localMeshBones);
    LoadMeshVertex(i, mesh, localMeshVertices, localMeshNormals);

    meshVertices[i] = localMeshVertices;
    meshVerticesBoneData[i] = localMeshBones;

    for (unsigned int k = 0; k < mesh->mNumFaces; k++) {
        const aiFace &Face = mesh->mFaces[k];

        //if (Face.mNumIndices < 3) continue;

        auto index1 = Face.mIndices[0];
        auto index2 = Face.mIndices[1];
        auto index3 = Face.mIndices[2];

        Vertex3D V1 = meshVertices[i][index1];
        Vertex3D V2 = meshVertices[i][index2];
        Vertex3D V3 = meshVertices[i][index3];

        meshes[i].modelTriangles.push_back(new Triangle(V1, V2, V3, this));

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

        for (int j = 0; j < (int) mesh->mBones[i]->mNumWeights; j++) {
            unsigned int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = mesh->mBones[i]->mWeights[j].mWeight;

            //Logging::Message(std::to_string(j) + " - AddBoneData for VertexID: " + std::to_string(VertexID) + " in idMesh: " + std::string(mesh->mName.C_Str()) + ", BoneIndex: " + std::to_string(BoneIndex) + ", Weight: " + std::to_string(Weight));
            meshVertexBoneData[VertexID].AddBoneData(BoneIndex, Weight);
        }
    }
}

void Mesh3DAnimation::LoadMeshVertex(int meshId, aiMesh *mesh, std::vector<Vertex3D> &meshVertex,  std::vector<Vertex3D> &meshNormal)
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

void Mesh3DAnimation::UpdateBonesFinalTransformations(float TimeInSeconds)
{
    aiMatrix4x4 Identity = aiMatrix4x4();

    float TicksPerSecond = scene->mAnimations[indexCurrentAnimation]->mTicksPerSecond != 0
                           ? scene->mAnimations[indexCurrentAnimation]->mTicksPerSecond : 0.25f;
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    auto AnimationTime = (float) fmod(TimeInTicks, scene->mAnimations[indexCurrentAnimation]->mDuration);

    ReadNodeHierarchy(AnimationTime, scene->mRootNode, Identity);
}

void Mesh3DAnimation::ReadNodeHierarchy(float AnimationTime, const aiNode *pNode, const aiMatrix4x4 &ParentTransform)
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

    return 0;
}

unsigned int Mesh3DAnimation::FindPosition(float AnimationTime, const aiNodeAnim *pNodeAnim)
{
    for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
        if (AnimationTime < (float) pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    return 0;
}

void Mesh3DAnimation::updateForBone(Vertex3D &V, int meshID, int vertexID)
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

void Mesh3DAnimation::drawBones(aiNode *node, Vertex3D *lastBonePosition)
{
    std::vector<aiMatrix4x4> Transforms;
    Transforms.resize(numBones);
    for (int i = 0; i < (int) numBones; i++) {
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
        drawBones(node->mChildren[j], lastBonePosition);
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

void Mesh3DAnimation::drawImGuiProperties()
{
    Mesh3D::drawImGuiProperties();

    const float range_min = 0.0f;
    const float range_max = 1.0f;

    if (ImGui::CollapsingHeader("Mesh3DAnimation")) {

        const char* items[(int) scene->mNumAnimations];
        for (int i = 0; i < (int) scene->mNumAnimations; i++) {
            items[i] = scene->mAnimations[i]->mName.C_Str();
        }
        auto comboTitle = "Animations##" + getLabel();
        ImGui::Combo("Animation", &indexCurrentAnimation, items, IM_ARRAYSIZE(items));

        ImGui::Separator();
        ImGui::DragScalar("Speed", ImGuiDataType_Float, &animation_speed, 0.01f ,&range_min, &range_max, "%f", 1.0f);
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

    if (o->isCollisionsEnabled()) {
        o->UpdateFrameTransformations();
        o->updateBoundingBox();
        o->UpdateShape();
    }

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

void Mesh3DAnimation::setPropertiesFromJSON(cJSON *object, Mesh3DAnimation *o)
{
    Mesh3D::setPropertiesFromJSON(object, o, false);

    auto speed = cJSON_GetObjectItemCaseSensitive(object, "animationSpeed")->valuedouble;
    o->setAnimationSpeed(speed);
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

bool Mesh3DAnimation::isAnimationEnds() const
{
    return animation_ends;
}

void Mesh3DAnimation::fillAnimationBuffers()
{
    for (int m = 0; m < meshes.size(); m++) {
        if (glIsBuffer(meshes[m].vertexBoneDataBuffer)) {
            glDeleteBuffers(1, &meshes[m].vertexBoneDataBuffer);
        }

        glGenBuffers(1, &meshes[m].vertexBoneDataBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, meshes[m].vertexBoneDataBuffer);
        glBufferData(GL_ARRAY_BUFFER, meshVerticesBoneData[m].size() * sizeof(VertexBoneData), meshVerticesBoneData[m].data(), GL_STATIC_DRAW);


        if (!glIsBuffer(meshes[m].vertexbuffer)) {
            glGenBuffers(1, &meshes[m].vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, meshes[m].vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, meshes[m].vertices.size() * sizeof(glm::vec4), meshes[m].vertices.data(), GL_STREAM_DRAW);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, meshes[m].vertexbuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0, meshes[m].vertices.size() * sizeof(glm::vec4), meshes[m].vertices.data());
        }
    }
}
