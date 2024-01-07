
#include <iostream>
#include <ext/matrix_transform.hpp>
#include "../../include/Objects/Object3D.h"
#include "../../include/Misc/Tools.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"
#include <glm/gtx/euler_angles.hpp>

Object3D::Object3D() :
    position(Vertex3D(1, 1, 1)),
    motion(nullptr),
    parent(nullptr),
    transparent(false),
    enabled(true),
    removed(false),
    decal(false),
    followCamera(false),
    stencilBufferEnabled(false),
    distanceToCamera(0),
    rotX(0),
    rotY(0),
    rotZ(0),
    rotXFrame(0),
    rotYFrame(0),
    rotZFrame(0),
    alphaEnabled(false),
    alpha(0),
    enableLights(false),
    scale(1),
    rotationFrameEnabled(false),
    belongToScene(false),
    multiScene(false),
    luaEnvironment(sol::environment(
            LUAManager::get()->getLua(),
            sol::create, LUAManager::get()->getLua().globals())
    )
{
    luaEnvironment.set("this", this);
    timer.start();
}

Vertex3D &Object3D::getPosition() {
    return position;
}

M3 Object3D::getRotation() {
    return rotation;
}

void Object3D::setPosition(Vertex3D p) {
    position = p;
}

void Object3D::setRotation(M3 r) {
    this->rotation = r;
}

std::string Object3D::getLabel() const {
    return label;
}

void Object3D::setLabel(const std::string& label) {
    Object3D::label = label;
}

bool &Object3D::isEnabled() {
    return enabled;
}

void Object3D::setEnabled(bool enabled) {
    Object3D::enabled = enabled;
}

Vertex3D Object3D::AxisUp() {
    Vertex3D v = getRotation() * EngineSetup::get()->up;
    return v.getNormalize();
}

Vertex3D Object3D::AxisDown() {
    Vertex3D v = getRotation() * EngineSetup::get()->down;
    return v.getNormalize();
}

Vertex3D Object3D::AxisForward() {
    Vertex3D v = getRotation() * EngineSetup::get()->forward;
    return v.getNormalize();
}

Vertex3D Object3D::AxisBackwards() {
    Vertex3D v = getRotation() * EngineSetup::get()->backward;

    return v.getNormalize();
}

Vertex3D Object3D::AxisRight() {
    Vertex3D v = getRotation() * EngineSetup::get()->right;

    return v.getNormalize();
}

Vertex3D Object3D::AxisLeft() {
    Vertex3D v = getRotation() * EngineSetup::get()->left;

    return v.getNormalize();
}

float Object3D::getScale() const {
    return scale;
}

void Object3D::setScale(float value) {
    Object3D::scale = value;
}

bool Object3D::isRemoved() const {
    return removed;
}

void Object3D::setRemoved(bool value) {
    Object3D::removed = value;
}

bool Object3D::isDecal() const {
    return decal;
}

void Object3D::setDecal(bool value) {
    Object3D::decal = value;
}

void Object3D::setDrawOffset(Vertex3D offset) {
    this->drawOffset = offset;
}

Vertex3D &Object3D::getDrawOffset() {
    return this->drawOffset;
}

Object3D *Object3D::getParent() const {
    return parent;
}

void Object3D::setParent(Object3D *object) {
    Object3D::parent = object;
}

bool &Object3D::isFollowCamera() {
    return this->followCamera;
}

void Object3D::setFollowCamera(bool value) {
    Object3D::followCamera = value;
}

void Object3D::onUpdate()
{
    if (isRemoved()) return;

    if (!isEnabled()) return;

    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    distanceToCamera = camera->getPosition().distance(getPosition());

    if (this->isFollowCamera()) {
        this->setPosition(camera->getPosition());
        this->setRotation(camera->getRotation().getTranspose());
    }

    if (getBehavior() != nullptr) {
        motion->onUpdate(position);
    }

    if (ComponentsManager::get()->getComponentRender()->getStateLUAScripts() == EngineSetup::LUA_PLAY) {
        runScripts();
    }
}

void Object3D::runScripts()
{
    for (auto script: scripts) {
        script->runEnvironment(luaEnvironment, "onUpdate");
    }
}

void Object3D::postUpdate()
{
    if (!isEnabled()) return;

    for (auto s: shaders) {
        s->postUpdate();
    }

    if (isRotationFrameEnabled()) {
        setRotation(getRotation() * M3::getMatrixRotationForEulerAngles(rotationFrame.x, rotationFrame.y, rotationFrame.z));
    }

    if (EngineSetup::get()->RENDER_OBJECTS_AXIS) {
        Drawable::drawObject3DAxis(this,true,true,true);
    }
}

void Object3D::setRotation(float x, float y, float z) {
    this->rotX = x;
    this->rotY = y;
    this->rotZ = z;
    this->setRotation(M3::getMatrixRotationForEulerAngles(x, y, z));
}

void Object3D::addToPosition(Vertex3D v) {
    this->position = this->position + v;
}

bool Object3D::isRotationFrameEnabled() const {
    return rotationFrameEnabled;
}

void Object3D::setRotationFrameEnabled(bool value) {
    this->rotationFrameEnabled = value;
}

void Object3D::setRotationFrame(Vertex3D r) {
    this->rotXFrame = r.x;
    this->rotYFrame = r.x;
    this->rotZFrame = r.x;
    this->rotationFrame = r;
}

bool &Object3D::isStencilBufferEnabled() {
    return stencilBufferEnabled;
}

void Object3D::setStencilBufferEnabled(bool value) {
    Object3D::stencilBufferEnabled = value;
}

Object3DBehavior *Object3D::getBehavior() const {
    return motion;
}

void Object3D::setBehavior(Object3DBehavior *motion) {
    Object3D::motion = motion;
}

float &Object3D::getAlpha() {
    return alpha;
}

void Object3D::setAlpha(float alpha) {
    Object3D::alpha = alpha;
}

bool &Object3D::isAlphaEnabled() {
    return alphaEnabled;
}

void Object3D::setAlphaEnabled(bool alphaEnabled)
{
    Object3D::alphaEnabled = alphaEnabled;
}

Object3D::~Object3D()
{
    for (auto s: shaders) {
        delete s;
    }
}

float &Object3D::getRotX() {
    return rotX;
}

float &Object3D::getRotY() {
    return rotY;
}

float &Object3D::getRotZ() {
    return rotZ;
}

void Object3D::onDrawHostBuffer()
{
    if (EngineSetup::get()->RENDER_OBJECTS_AXIS) {
        Drawable::drawObject3DAxis(this,true,true,true);
    }
}

bool Object3D::isEnableLights() const {
    return enableLights;
}

void Object3D::setEnableLights(bool enableLights) {
    Object3D::enableLights = enableLights;
}

void Object3D::lookAt(Object3D *o)
{
    Vertex3D direction = (o->getPosition() - position).getNormalize();

    // Calcular el eje de rotación
    Vertex3D rightVector = Vertex3D(0, 0, -1) % (direction).getNormalize();
    Vertex3D correctedUpVector = direction % (rightVector).getNormalize();

    // Crear una matriz de rotación
    M3 r;
    r.setX(rightVector.x, correctedUpVector.x, -direction.x);
    r.setY(rightVector.y, correctedUpVector.y, -direction.y);
    r.setZ(rightVector.z, correctedUpVector.z, -direction.z);

    setRotation(r);
}

void Object3D::attachScript(ScriptLUA *script)
{
    scripts.push_back(script);
    reloadScriptsEnvironment();
}

void Object3D::reloadScriptsEnvironment()
{
    for (auto script : scripts) {
        script->reloadEnvironment(luaEnvironment);
    }
}

void Object3D::reloadScriptsCode()
{
    for (auto script : scripts) {
        script->reloadScriptCode();
    }
}

void Object3D::removeScript(ScriptLUA *script)
{
    Logging::Message("Removing object script %s", script->scriptFilename.c_str());

    for (auto it = scripts.begin(); it != scripts.end(); ++it) {
        if ((*it)->scriptFilename == script->scriptFilename) {
            delete *it;
            scripts.erase(it);
            return;
        }
    }
}

const char *Object3D::getTypeObject() {
    return "Object3D";
}

const char *Object3D::getTypeIcon() {
    return "objectIcon";
}

const std::vector<ScriptLUA *> &Object3D::getScripts() const {
    return scripts;
}

void Object3D::runStartScripts()
{
    for (auto script : scripts) {
        script->runEnvironment(luaEnvironment, "onStart");
    }
}

bool Object3D::isBelongToScene() const {
    return belongToScene;
}

void Object3D::setBelongToScene(bool belongToScene) {
    Object3D::belongToScene = belongToScene;
}

void Object3D::drawImGuiProperties()
{
    ImGui::Checkbox("Enabled", &enabled);
    ImGui::SameLine();
    ImGui::Checkbox("Transparent", &transparent);
    ImGui::SameLine();

    static char name[256];
    strncpy(name, label.c_str(), sizeof(name));
    ImGui::InputText("Name##nameObject", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AlwaysOverwrite);
    if (ImGui::IsItemEdited()) {
        setLabel(name);
    }
    ImGui::Separator();

    if (ImGui::TreeNode("Position")) {
        const float range_min = -500000;
        const float range_max = 500000;
        const float range_sensibility = 0.1;

        ImGui::DragScalar("X", ImGuiDataType_Float, &getPosition().x, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
        ImGui::DragScalar("Y", ImGuiDataType_Float, &getPosition().y, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
        ImGui::DragScalar("Z", ImGuiDataType_Float, &getPosition().z, range_sensibility ,&range_min, &range_max, "%f", 1.0f);

        ImGui::TreePop();
    }
    ImGui::Separator();

    // rotation
    if (ImGui::TreeNode("Rotation")) {
        const float range_angle_min = -360;
        const float range_angle_max = 360;
        const float range_angle_sensibility = 0.1;

        bool needUpdateRotation = false;
        ImGui::DragScalar("X", ImGuiDataType_Float, &getRotX(), range_angle_sensibility, &range_angle_min,&range_angle_max, "%f", 1.0f);
        if (ImGui::IsItemEdited()) {
            needUpdateRotation = true;
        }
        ImGui::DragScalar("Y", ImGuiDataType_Float, &getRotY(), range_angle_sensibility, &range_angle_min,&range_angle_max, "%f", 1.0f);
        if (ImGui::IsItemEdited()) {
            needUpdateRotation = true;
        }
        ImGui::DragScalar("Z", ImGuiDataType_Float, &getRotZ(), range_angle_sensibility, &range_angle_min,&range_angle_max, "%f", 1.0f);
        if (ImGui::IsItemEdited()) {
            needUpdateRotation = true;
        }
        if (needUpdateRotation) {
            setRotation(M3::getMatrixRotationForEulerAngles(getRotX(), getRotY(), getRotZ()));
        }

        ImGui::Checkbox("Rotation Frame", &rotationFrameEnabled);

        ImGui::Separator();

        if (rotationFrameEnabled) {
            if (ImGui::TreeNode("Rotation Each Frame")) {
                ImGui::DragScalar("X", ImGuiDataType_Float, &rotationFrame.x, range_angle_sensibility, &range_angle_min,&range_angle_max, "%f", 1.0f);
                ImGui::DragScalar("Y", ImGuiDataType_Float, &rotationFrame.y, range_angle_sensibility, &range_angle_min,&range_angle_max, "%f", 1.0f);
                ImGui::DragScalar("Z", ImGuiDataType_Float, &rotationFrame.z, range_angle_sensibility, &range_angle_min,&range_angle_max, "%f", 1.0f);
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
    ImGui::Separator();

    if (ImGui::TreeNode("Scale")) {
        // scale
        const float range_scale_min = -360;
        const float range_scale_max = 360;
        const float range_scale_sensibility = 0.01;
        ImGui::DragScalar("Scale", ImGuiDataType_Float, &scale, range_scale_sensibility, &range_scale_min, &range_scale_max, "%f", 1.0f);

        ImGui::TreePop();
    }
    ImGui::Separator();

    if (ImGui::TreeNode("Alpha")) {
        ImGui::Checkbox("Enable Alpha", &isAlphaEnabled());

        if (isAlphaEnabled()) {
            const float range_alpha_min = 0;
            const float range_alpha_max = 255;
            const float range_alpha_sensibility = 1;

            ImGui::DragScalar("Alpha", ImGuiDataType_Float, &getAlpha(), range_alpha_sensibility, &range_alpha_min, &range_alpha_max, "%f", 1.0f);
        }

        ImGui::TreePop();
    }
    ImGui::Separator();

    if (ImGui::TreeNode("Shaders")) {
        if ((int) shaders.size() <= 0) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No shaders attached");
        }
        auto ImGuiTextures = Brakeza3D::get()->getManagerGui()->getImGuiTextures();

        for (int i = 0; i < (int) shaders.size(); i++) {
            ImGui::PushID(i);

            auto s = shaders[i];
            ImGui::Image((ImTextureID)ImGuiTextures->getTextureByLabel("shaderIcon")->getOGLTextureID(), ImVec2(24, 24));
            ImGui::SameLine(100);

            if (!s->isEnabled()) {
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures->getTextureByLabel("unlockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                    s->setEnabled(true);
                }
            } else {
                if (ImGui::ImageButton((ImTextureID)ImGuiTextures->getTextureByLabel("lockIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                    s->setEnabled(false);
                }
            }
            ImGui::SameLine();
            if (ImGui::ImageButton((ImTextureID)ImGuiTextures->getTextureByLabel("removeIcon")->getOGLTextureID(), ImVec2(14, 14))) {
                removeShader(i);
            }
            ImGui::SameLine();
            if (ImGui::CollapsingHeader(s->getLabel().c_str(), ImGuiTreeNodeFlags_None)) {
                ImGui::PushID(i);
                s->drawImGuiProperties();
                ImGui::PopID();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }

    ImGui::Separator();
}

cJSON *Object3D::getJSON()
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "name", getLabel().c_str());
    cJSON_AddNumberToObject(root, "scale", getScale());
    cJSON_AddBoolToObject(root, "stencilBufferEnabled", isStencilBufferEnabled());
    cJSON_AddBoolToObject(root, "transparent", isTransparent());

    cJSON *position = cJSON_CreateObject();
    cJSON_AddNumberToObject(position, "x", (int) getPosition().x);
    cJSON_AddNumberToObject(position, "y", (int) getPosition().y);
    cJSON_AddNumberToObject(position, "z", (int) getPosition().z);
    cJSON_AddItemToObject(root, "position", position);

    cJSON *rotation = cJSON_CreateObject();
    cJSON_AddNumberToObject(rotation, "x", (int) rotX);
    cJSON_AddNumberToObject(rotation, "y", (int) rotY);
    cJSON_AddNumberToObject(rotation, "z", (int) rotZ);
    cJSON_AddItemToObject(root, "rotation", rotation);

    cJSON_AddBoolToObject(root, "rotationFrameEnabled", isRotationFrameEnabled());

    if (isRotationFrameEnabled()) {
        cJSON *rFrame = cJSON_CreateObject();
        cJSON_AddNumberToObject(rFrame, "x", (int) rotXFrame);
        cJSON_AddNumberToObject(rFrame, "y", (int) rotYFrame);
        cJSON_AddNumberToObject(rFrame, "z", (int) rotZFrame);
        cJSON_AddItemToObject(root, "rotationFrame", rFrame);
    }

    cJSON *scriptsArray = cJSON_CreateArray();
    for (auto script : scripts) {
        cJSON_AddItemToArray(scriptsArray, script->getTypesJSON());
    }
    cJSON_AddItemToObject(root, "scripts", scriptsArray);

    return root;
}

void Object3D::setPropertiesFromJSON(cJSON *object, Object3D *o)
{
    o->setBelongToScene(true);

    o->setPosition(SceneLoader::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(object, "position")));
    o->setScale((float)cJSON_GetObjectItemCaseSensitive(object, "scale")->valuedouble);

    if (cJSON_GetObjectItemCaseSensitive(object, "rotation") != nullptr) {
        cJSON *rotation = cJSON_GetObjectItemCaseSensitive(object, "rotation");
        o->setRotation(
            (float) cJSON_GetObjectItemCaseSensitive(rotation, "x")->valuedouble,
            (float) cJSON_GetObjectItemCaseSensitive(rotation, "y")->valuedouble,
            (float) cJSON_GetObjectItemCaseSensitive(rotation, "z")->valuedouble
        );
    }
    o->setStencilBufferEnabled(cJSON_GetObjectItemCaseSensitive(object, "stencilBufferEnabled")->valueint);
    o->setRotationFrameEnabled(cJSON_GetObjectItemCaseSensitive(object, "rotationFrameEnabled")->valueint);

    if (cJSON_GetObjectItemCaseSensitive(object, "rotationFrame") != nullptr) {
        o->setRotationFrame(SceneLoader::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(object, "rotationFrame")));
    }

    if (cJSON_GetObjectItemCaseSensitive(object, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(object, "scripts")) {
            auto filename = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
            auto typesJSON = cJSON_GetObjectItemCaseSensitive(currentScript, "types");
            o->attachScript(new ScriptLUA(filename, typesJSON));
        }
    }

    if (cJSON_GetObjectItemCaseSensitive(object, "shaders") != nullptr) {
        auto mesh3DShaderTypes = ComponentsManager::get()->getComponentRender()->getSceneLoader().getMesh3DShaderTypes();
        cJSON *currentShader;
        cJSON_ArrayForEach(currentShader, cJSON_GetObjectItemCaseSensitive(object, "shaders")) {
            auto type = cJSON_GetObjectItemCaseSensitive(currentShader, "type")->valuestring;
            switch(mesh3DShaderTypes[type]) {
                case Mesh3DShaderLoaderMapping::ShaderEdgeObject: {
                    break;
                }
            }
        }
    }
}

void Object3D::createFromJSON(cJSON *object)
{
    auto o = new Object3D();

    Object3D::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}

glm::mat4 Object3D::getModelMatrix()
{
    glm::vec3 scaled(scale);
    glm::vec3 rotated(glm::radians(getRotation().getPitchDegree()), glm::radians(getRotation().getYawDegree()), glm::radians(getRotation().getRollDegree()));
    glm::vec3 translated = position.toGLM();

    glm::mat4 modelMatrix =
        glm::translate(glm::mat4(1.0f), translated) *
        (glm::eulerAngleX(rotated.x) * glm::eulerAngleY(rotated.y) * glm::eulerAngleZ(rotated.z)) *
        glm::scale(glm::mat4(1.0f), scaled)
    ;

    return modelMatrix;
}

void Object3D::addMesh3DShader(FXEffectOpenGL *shader) {
    shaders.push_back(shader);
}

void Object3D::removeShader(int index) {
    if (index >= 0 && index < shaders.size()) {
        shaders.erase(shaders.begin() + index);
    }
}

const Timer &Object3D::getTimer() const {
    return timer;
}

bool Object3D::isTransparent() const {
    return transparent;
}

void Object3D::setTransparent(bool transparent) {
    Object3D::transparent = transparent;
}

float Object3D::getDistanceToCamera() const {
    return distanceToCamera;
}

bool Object3D::isMultiScene() const {
    return multiScene;
}

void Object3D::setMultiScene(bool multiScene) {
    Object3D::multiScene = multiScene;
}
