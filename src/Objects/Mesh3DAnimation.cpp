#include "../../include/Objects/Mesh3DAnimation.h"
#include "../../include/Brakeza3D.h"
#include "../../include/Misc/ToolsJSON.h"

Mesh3DAnimation::Mesh3DAnimation()
:
    scene(nullptr),
    numBones(0),
    loop(true),
    boneColliderEnabled(false),
    indexCurrentAnimation(0),
    runningTime(0),
    remove_at_end_animation(false),
    animation_speed(1),
    animation_ends(false),
    boneColliderIndex(0)
{
    luaEnvironment["this"] = this;
}

void Mesh3DAnimation::onUpdate()
{
    Object3D::onUpdate();

    if (isRemoved() || scene == nullptr) return;

    UpdateFrameTransformations();

    UpdateOpenGLBones();

    if (boneColliderEnabled) {
        UpdateBoneColliders();
    }

    auto render = ComponentsManager::get()->getComponentRender();
    auto window = ComponentsManager::get()->getComponentWindow();

    if (render->getSelectedObject() == this) {
        render->getShaderOGLOutline()->drawOutline(this, Color::green(), 0.1f);
    }

    if (EngineSetup::get()->TRIANGLE_MODE_TEXTURIZED && isRender()) {
        render->getShaderOGLRender()->renderAnimatedMesh(this,window->getSceneFramebuffer());
    }

    if (EngineSetup::get()->TRIANGLE_MODE_PIXELS && isRender()) {
        render->getShaderOGLPoints()->renderMeshAnimation(this, window->getSceneFramebuffer());
    }

    if (EngineSetup::get()->TRIANGLE_MODE_COLOR_SOLID && isRender()) {
        render->getShaderOGLShading()->renderMeshAnimation(this, window->getSceneFramebuffer());
    }

    if (EngineSetup::get()->TRIANGLE_MODE_WIREFRAME && isRender()) {
        render->getShaderOGLWireframe()->renderMeshAnimation(this, window->getSceneFramebuffer());
    }

    if (EngineSetup::get()->DRAW_MESH3D_AABB && isRender()) {
        updateBoundingBox();
        Drawable::drawAABB(&aabb, Color::white());
    }

    if (EngineSetup::get()->DRAW_MESH3D_OCTREE && octree != nullptr) {
        Drawable::drawOctree(octree);
    }

    if (EngineSetup::get()->DRAW_MESH3D_GRID && grid != nullptr) {
        Drawable::drawGrid3D(grid);
    }

    for (auto &s: customShaders) {
        s->render(window->getSceneFramebuffer());
    }

    if (EngineSetup::get()->DRAW_ANIMATION_BONES) {
        drawBones(scene->mRootNode, nullptr);
    }
}

void Mesh3DAnimation::UpdateOpenGLBones()
{
    std::vector<glm::mat4> transformations(MAX_BONES, glm::mat4(0));

    for (int i = 0; i < (int) boneInfo.size(); i++) {
        transformations[i] = Tools::aiMat4toGLMMat4(boneInfo[i].FinalTransformation);
    }

    for (auto &m: meshes) {
        if ((int) m.vertices.size() <= 0) continue;
        ComponentsManager::get()->getComponentRender()->getShaderOGLBonesTransforms()->render(
            m,
            transformations,
            ComponentsManager::get()->getComponentWindow()->getSceneFramebuffer()
        );
    }
}

void Mesh3DAnimation::UpdateFrameTransformations()
{
    if (!scene->HasAnimations()) return;

    CheckIfEndAnimation();

    if (isRemoveAtEndAnimation() && isAnimationEnds()) {
        getParent()->setRemoved(true);
        return;
    }

    UpdateBonesFinalTransformations(runningTime);
}

void Mesh3DAnimation::CheckIfEndAnimation()
{
    auto timeIncrement = Brakeza3D::get()->getDeltaTime() * animation_speed;
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

    ComponentsManager::get()->getComponentRender()->FillOGLBuffers(meshes);

    FillAnimationBoneDataOGLBuffers();

    boneColliderIndex = 0;
    boneColliderEnabled = true;

    UpdateBonesFinalTransformations(0);

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

        if (Face.mNumIndices < 3) continue;

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
            Logging::Message("Loading BoneInfo %s", boneInfo[BoneIndex].name.c_str());
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

        ImGui::Separator();
        ImGui::Checkbox("Loop", &loop);
        ImGui::Separator();
        ImGui::Checkbox("Bones Colliders", &boneColliderEnabled);

        if (boneColliderEnabled) {
            if (ImGui::Button(std::string("Manage Bone Mappings##" + getLabel()).c_str())) {
                Brakeza3D::get()->getManagerGui()->openBoneInfoDialog();
            }
        }
    }
}

cJSON *Mesh3DAnimation::getJSON()
{
    cJSON *root = Mesh3D::getJSON();

    cJSON_AddNumberToObject(root, "animationSpeed", animation_speed);
    cJSON_AddBoolToObject(root, "boneColliderEnabled", boneColliderEnabled);

    cJSON *bonesCollidersJSON = cJSON_CreateArray();
    for (const auto& b : boneMappingColliders) {
        cJSON *bonesColliderJSON = cJSON_CreateObject();

        cJSON_AddStringToObject(bonesColliderJSON, "nameMapping", b.nameMapping.c_str());

        cJSON *bonesInfoJSON = cJSON_CreateArray();
        for (const auto& boneInfoCollider : b.boneColliderInfo) {
            if (!boneInfoCollider.enabled) continue;

            cJSON *boneInfoColliderJSON = cJSON_CreateObject();

            cJSON_AddStringToObject(boneInfoColliderJSON, "name", boneInfoCollider.name.c_str());
            cJSON_AddNumberToObject(boneInfoColliderJSON, "boneId", boneInfoCollider.boneId);
            cJSON_AddBoolToObject(boneInfoColliderJSON, "enabled", boneInfoCollider.enabled);
            cJSON_AddNumberToObject(boneInfoColliderJSON, "shape", boneInfoCollider.shape);

            cJSON *position = cJSON_CreateObject();
            cJSON_AddNumberToObject(position, "x", (float) boneInfoCollider.position.x);
            cJSON_AddNumberToObject(position, "y", (float) boneInfoCollider.position.y);
            cJSON_AddNumberToObject(position, "z", (float) boneInfoCollider.position.z);
            cJSON_AddItemToObject(boneInfoColliderJSON, "position", position);

            cJSON *size = cJSON_CreateObject();
            cJSON_AddNumberToObject(size, "x", (float) boneInfoCollider.size.x);
            cJSON_AddNumberToObject(size, "y", (float) boneInfoCollider.size.y);
            cJSON_AddNumberToObject(size, "z", (float) boneInfoCollider.size.z);
            cJSON_AddItemToObject(boneInfoColliderJSON, "size", size);

            cJSON_AddItemToArray(bonesInfoJSON, boneInfoColliderJSON);
        }

        cJSON_AddItemToObject(bonesColliderJSON, "bonesInfoColliders", bonesInfoJSON);

        cJSON_AddItemToArray(bonesCollidersJSON, bonesColliderJSON);
    }
    cJSON_AddItemToObject(root, "bonesColliders", bonesCollidersJSON);

    return root;
}

void Mesh3DAnimation::createFromJSON(cJSON *object)
{
    auto o = new Mesh3DAnimation();

    Mesh3D::setPropertiesFromJSON(object, o, false);

    o->AssimpLoadAnimation(cJSON_GetObjectItemCaseSensitive(object, "model")->valuestring);

    if (o->isCollisionsEnabled()) {
        o->UpdateFrameTransformations();
        o->updateBoundingBox();
        o->UpdateShapeCollider();
    }

    Mesh3DAnimation::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

void Mesh3DAnimation::setPropertiesFromJSON(cJSON *object, Mesh3DAnimation *o)
{

    auto speed = cJSON_GetObjectItemCaseSensitive(object, "animationSpeed")->valuedouble;
    o->setAnimationSpeed((float) speed);

    cJSON *currentMapCollider;
    cJSON_ArrayForEach(currentMapCollider, cJSON_GetObjectItemCaseSensitive(object, "bonesColliders")) {
        std::string nameMapping = cJSON_GetObjectItemCaseSensitive(currentMapCollider, "nameMapping")->valuestring;
        o->createBonesMappingColliders(nameMapping);

        cJSON *currentBoneInfoCollider;
        cJSON_ArrayForEach(currentBoneInfoCollider, cJSON_GetObjectItemCaseSensitive(currentMapCollider, "bonesInfoColliders")) {
            std::string boneName = cJSON_GetObjectItemCaseSensitive(currentBoneInfoCollider, "name")->valuestring;
            unsigned int boneId = cJSON_GetObjectItemCaseSensitive(currentBoneInfoCollider, "boneId")->valueint;
            unsigned int shape = cJSON_GetObjectItemCaseSensitive(currentBoneInfoCollider, "shape")->valueint;
            bool enabled = (bool) cJSON_GetObjectItemCaseSensitive(currentBoneInfoCollider, "enabled")->valueint;

            //auto position = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(currentBoneInfoCollider, "position"));
            //auto size = ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(currentBoneInfoCollider, "size"));

            o->SetMappingBoneColliderInfo(nameMapping, boneId, enabled, BoneCollisionShape::BONE_CAPSULE);
        }
    }

}

void Mesh3DAnimation::setAnimationSpeed(float animationSpeed)
{
    animation_speed = animationSpeed;
}

Mesh3DAnimation::~Mesh3DAnimation()
{
    meshVerticesBoneData.clear();
    meshVertices.clear();
    boneMapping.clear();
    boneInfo.clear();

    for (auto &c : boneMappingColliders) {
        for (auto &bone : c.boneColliderInfo) {
            if (bone.ghostObject != nullptr) {
                ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld()->removeCollisionObject(bone.ghostObject);
                delete bone.ghostObject;
            }
            if (bone.convexHullShape != nullptr) {
                delete bone.convexHullShape;
            }
        }
    }
}

void Mesh3DAnimation::setIndexCurrentAnimation(int indexCurrentAnimation)
{
    Mesh3DAnimation::indexCurrentAnimation = indexCurrentAnimation;
}

bool Mesh3DAnimation::isAnimationEnds() const
{
    return animation_ends;
}

void Mesh3DAnimation::FillAnimationBoneDataOGLBuffers()
{
    for (int i = 0; i < meshes.size(); i++) {
        if (meshes[i].vertices.size() <= 0) continue;

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

void Mesh3DAnimation::updateBoundingBox()
{
    glm::mat4 mvpMatrix = getModelMatrix();

    float maxX = -FLT_MAX, minX = FLT_MAX, maxY = -FLT_MAX, minY = FLT_MAX, maxZ = -FLT_MAX, minZ = FLT_MAX;

    std::vector<Vertex3D> vertices;

    for (int i = 0; i < (int) scene->mNumMeshes; i++) {

        if (meshVertices[i].empty()) continue;

        for (unsigned int k = 0; k < scene->mMeshes[i]->mNumFaces; k++) {

            const aiFace &Face = scene->mMeshes[i]->mFaces[k];

            auto v1Index= Face.mIndices[0];
            auto v2Index= Face.mIndices[1];
            auto v3Index= Face.mIndices[2];

            Vertex3D V1 = meshVertices[i][v1Index];
            Vertex3D V2 = meshVertices[i][v2Index];
            Vertex3D V3 = meshVertices[i][v3Index];

            updateForBone(V1, i, (int) v1Index);
            updateForBone(V2, i, (int) v2Index);
            updateForBone(V3, i, (int) v3Index);

            vertices.emplace_back(V1);
            vertices.emplace_back(V2);
            vertices.emplace_back(V3);
        }
    }

    for (auto &vertex : vertices) {
        glm::vec4 transformedVertex = mvpMatrix * vertex.toGLM4();
        transformedVertex /= transformedVertex.w;

        Vertex3D V(transformedVertex.x, transformedVertex.y, transformedVertex.z);

        maxX = std::max(maxX, V.x);
        minX = std::min(minX, V.x);
        maxY = std::max(maxY, V.y);
        minY = std::min(minY, V.y);
        maxZ = std::max(maxZ, V.z);
        minZ = std::min(minZ, V.z);
    }

    this->aabb.max.x = maxX;
    this->aabb.max.y = maxY;
    this->aabb.max.z = maxZ;

    this->aabb.min.x = minX;
    this->aabb.min.y = minY;
    this->aabb.min.z = minZ;

    this->aabb.updateVertices();
}

void Mesh3DAnimation::setAnimationByName(const std::string& name)
{
    const char* items[(int) scene->mNumAnimations];

    for (int i = 0; i < (int) scene->mNumAnimations; i++) {
        if (name == scene->mAnimations[i]->mName.C_Str()) {
            setIndexCurrentAnimation(i);
        }
    }
}

bool Mesh3DAnimation::isLoop() const {
    return loop;
}

void Mesh3DAnimation::setLoop(bool loop) {
    Mesh3DAnimation::loop = loop;
}

void Mesh3DAnimation::createBonesMappingColliders(std::string name)
{
    BonesMappingColliders bmc;
    bmc.nameMapping = name;
    bmc.boneColliderInfo.resize(this->numBones);

    for (int i = 0; i < (int) boneInfo.size(); i++) {
        auto ci = bmc.boneColliderInfo[i];
        bmc.boneColliderInfo[i].ghostObject = nullptr;
        bmc.boneColliderInfo[i].shape = BoneCollisionShape::BONE_SPHERE;
        bmc.boneColliderInfo[i].enabled = false;
        bmc.boneColliderInfo[i].name = boneInfo[i].name;
        bmc.boneColliderInfo[i].boneId = i;
    }

    boneMappingColliders.push_back(bmc);

    if (boneColliderIndex <= 0) {
        boneColliderIndex = 0;
    }
}

void Mesh3DAnimation::SetMappingBoneColliderInfo(
    const std::string& mappingName,
    unsigned int boneId,
    bool enabled,
    BoneCollisionShape shape = BONE_SPHERE
) {
    Logging::Message("Setting Bone %d for %s to %d", boneId, mappingName.c_str(), enabled);

    auto mapping = getBonesMappingByName(mappingName);
    auto &ci = mapping->boneColliderInfo[boneId];

    ci.boneId = boneId;
    ci.shape = shape;
    ci.enabled = enabled;
    ci.name = boneInfo[boneId].name;

    auto world = ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld();

    if (enabled) {
        if (ci.ghostObject == nullptr ) {
            createBoneGhostBody(boneId, shape, ci);
        } else {
            world->removeCollisionObject(ci.ghostObject);
            ci.ghostObject = nullptr;
        }
    } else {
        if (ci.ghostObject != nullptr ) {
            world->removeCollisionObject(ci.ghostObject);
            ci.ghostObject = nullptr;
        }
    }
}

void Mesh3DAnimation::createBoneGhostBody(unsigned int boneId, const BoneCollisionShape &shape, BoneColliderInfo &ci)
{
    aiVector3t<float> scaling, rotationAxis, position;
    float rotationAngle;

    aiMatrix4x4t<float> FinalTransformation = boneInfo[boneId].FinalTransformation; // Esta matriz debe estar previamente inicializada.
    FinalTransformation.Decompose(scaling, rotationAxis, rotationAngle, position);

    ci.position = Vertex3D(position.x, position.y, position.z);
    ci.size = Vertex3D(0.1, 0.1, 0.1);

    btTransform transformation;
    transformation.setOrigin((getPosition() + ci.position).toBullet());

    btMatrix3x3 brakezaRotation;
    brakezaRotation.setIdentity();

    btQuaternion qRotation;
    brakezaRotation.getRotation(qRotation);
    transformation.setRotation(qRotation);

    btConvexHullShape *convexHullShape;
    switch (shape) {
        case BONE_SPHERE: {
            convexHullShape = reinterpret_cast<btConvexHullShape *>(new btSphereShape(ci.size.x));;
            break;
        }
        case BONE_CAPSULE: {
            convexHullShape = reinterpret_cast<btConvexHullShape *>(new btCapsuleShape(ci.size.x, ci.size.y));;
            break;
        }
        case BONE_CUBE: {
            convexHullShape = reinterpret_cast<btConvexHullShape *>(new btBoxShape(ci.size.toBullet()));;
            break;
        }
    }

    ci.ghostObject = new btPairCachingGhostObject();
    ci.ghostObject->setCollisionShape(convexHullShape);
    ci.ghostObject->setWorldTransform(transformation);
    ci.ghostObject->setUserPointer(this);
    ci.ghostObject->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);

    ComponentsManager::get()
        ->getComponentCollisions()
        ->getDynamicsWorld()
        ->addCollisionObject(
            ci.ghostObject,
            btBroadphaseProxy::DefaultFilter,
            btBroadphaseProxy::DefaultFilter
        )
    ;
}

void Mesh3DAnimation::UpdateBoneColliders()
{
    if (boneColliderIndex < 0) return;

    for (auto &bc : boneMappingColliders) {
        for (auto &b : bc.boneColliderInfo) {
            if (b.enabled) {
                if (b.ghostObject == nullptr) {
                    continue;
                }

                aiMatrix4x4 mOffset = boneInfo[b.boneId].BoneOffset;
                aiMatrix4x4 mT = boneInfo[b.boneId].FinalTransformation;
                aiVector3D aBonePosition;

                aBonePosition = mOffset.Inverse() * aBonePosition;
                aBonePosition = mT * aBonePosition;

                Vertex3D bonePosition = Vertex3D::fromAssimp(aBonePosition);
                Transforms::objectSpace(bonePosition, bonePosition, this);

                btTransform transformation;
                transformation.setOrigin(bonePosition.toBullet());

                btMatrix3x3 brakezaRotation; //rotation.toBulletMat3();
                brakezaRotation.setIdentity();

                btQuaternion qRotation;
                brakezaRotation.getRotation(qRotation);
                transformation.setRotation(qRotation);
                b.ghostObject->setWorldTransform(transformation);
            }
        }
    }
}

BonesMappingColliders *Mesh3DAnimation::getBonesMappingByName(const std::string& name)
{
    for (auto &b : boneMappingColliders) {
        if (b.nameMapping == name) {
            return &b;
        }
    }

    return nullptr;
}

const std::vector<BonesMappingColliders> *Mesh3DAnimation::getBoneMappingColliders() const {
    return &boneMappingColliders;
}

void Mesh3DAnimation::removeBonesColliderMapping(const std::string& name)
{
    auto bm = getBonesMappingByName(name);
    Logging::Message("Deleting bone collider mapping: %s, new boneColliderIndex: %d", name.c_str(), boneColliderIndex);

    if (bm == nullptr) return;

    auto world = ComponentsManager::get()->getComponentCollisions()->getDynamicsWorld();

    for (auto &b : bm->boneColliderInfo) {
        if (b.ghostObject == nullptr) continue;
        world->removeCollisionObject(b.ghostObject);
        b.ghostObject = nullptr;
        b.enabled = false;
    }

    for (auto it = boneMappingColliders.begin(); it != boneMappingColliders.end();) {
        if (it->nameMapping == name) {
            it = boneMappingColliders.erase(it);
        } else {
            ++it;
        }
    }

    if (boneMappingColliders.empty()) {
        boneColliderIndex = -1;
    } else if (boneColliderIndex >= boneMappingColliders.size()) {
        boneColliderIndex = (int)boneMappingColliders.size() - 1; // Ajustar al último elemento válido
    }
}