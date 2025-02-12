
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include "../../include/Objects/Object3D.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"
#include <glm/gtx/euler_angles.hpp>

Object3D::Object3D() :
    rotation(M3::getMatrixIdentity()),
    motion(nullptr),
    parent(nullptr),
    enabled(true),
    removed(false),
    decal(false),
    belongToScene(false),
    multiScene(false),
    alphaEnabled(false),
    alpha(1.0f),
    luaEnvironment(sol::environment(
            ComponentsManager::get()->getComponentScripting()->getLua(),
        sol::create, ComponentsManager::get()->getComponentScripting()->getLua().globals())
    ),
    distanceToCamera(0),
    enableLights(false),
    scale(1),
    position(Vertex3D(1, 1, 1))
{
    luaEnvironment["this"] = this;
    timer.start();
}

Vertex3D &Object3D::getPosition()
{
    return position;
}

M3 Object3D::getRotation()
{
    return rotation;
}

void Object3D::setPosition(Vertex3D p)
{
    position = p;
}

void Object3D::setRotation(M3 r)
{
    this->rotation = r;
}

std::string Object3D::getLabel() const
{
    return label;
}

void Object3D::setLabel(const std::string& label)
{
    Object3D::label = label;
}

bool &Object3D::isEnabled() {
    return enabled;
}

void Object3D::setEnabled(bool enabled)
{
    Object3D::enabled = enabled;
}

Vertex3D Object3D::AxisUp()
{
    Vertex3D v = getRotation() * EngineSetup::get()->up;
    return v.getNormalize();
}

Vertex3D Object3D::AxisDown()
{
    Vertex3D v = getRotation() * EngineSetup::get()->down;
    return v.getNormalize();
}

Vertex3D Object3D::AxisForward()
{
    Vertex3D v = getRotation() * EngineSetup::get()->forward;
    return v.getNormalize();
}

Vertex3D Object3D::AxisBackwards()
{
    Vertex3D v = getRotation() * EngineSetup::get()->backward;

    return v.getNormalize();
}

Vertex3D Object3D::AxisRight()
{
    Vertex3D v = getRotation() * EngineSetup::get()->right;

    return v.getNormalize();
}

Vertex3D Object3D::AxisLeft()
{
    Vertex3D v = getRotation() * EngineSetup::get()->left;

    return v.getNormalize();
}

float Object3D::getScale() const
{
    return scale;
}

void Object3D::setScale(float value)
{
    Object3D::scale = value;
}

bool Object3D::isRemoved() const
{
    return removed;
}

void Object3D::setRemoved(bool value)
{
    Object3D::removed = value;
}

bool Object3D::isDecal() const
{
    return decal;
}

void Object3D::setDecal(bool value)
{
    Object3D::decal = value;
}

void Object3D::setDrawOffset(Vertex3D offset)
{
    this->drawOffset = offset;
}

Vertex3D &Object3D::getDrawOffset()
{
    return this->drawOffset;
}

Object3D *Object3D::getParent() const
{
    return parent;
}

void Object3D::setParent(Object3D *object)
{
    Object3D::parent = object;
}

void Object3D::onUpdate()
{
    if (isRemoved() || !isEnabled()) return;

    for (auto a: attachedObjects) {
        if (a->isEnabled()) a->onUpdate();
    }

    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();

    distanceToCamera = camera->getPosition().distance(getPosition());

    if (getBehavior() != nullptr && getBehavior()->isEnabled()) {
        motion->onUpdate(position);
    }

    if (ComponentsManager::get()->getComponentScripting()->getStateLUAScripts() == EngineSetup::LUA_PLAY) {
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

    for (auto s: effects) {
        s->postUpdate();
    }

    for (auto a: attachedObjects) {
        if (a->isEnabled())  a->postUpdate();
    }

    if (EngineSetup::get()->RENDER_OBJECTS_AXIS) {
        Drawable::drawObject3DAxis(this,true,true,true);
    }
}

void Object3D::addToPosition(Vertex3D v) {
    this->position = this->position + v;
}

Object3DBehavior *Object3D::getBehavior() const
{
    return motion;
}

void Object3D::setBehavior(Object3DBehavior *motion)
{
    Object3D::motion = motion;
}

float &Object3D::getAlpha() {
    return alpha;
}

void Object3D::setAlpha(float alpha)
{
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
    for (auto s: effects) {
        delete s;
    }

    for (auto a: attachedObjects) {
        delete a;
    }

    if (isCollisionsEnabled()) {
        removeCollisionObject();
    }
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

void Object3D::setEnableLights(bool enableLights)
{
    Object3D::enableLights = enableLights;
}

void Object3D::lookAt(Object3D *o)
{
    Vertex3D direction = (o->getPosition() - position).getNormalize();

    Vertex3D rightVector = Vertex3D(0, 0, -1) % (direction).getNormalize();
    Vertex3D correctedUpVector = direction % (rightVector).getNormalize();

    setRotation(M3::getFromVectors(direction, correctedUpVector));
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

const char *Object3D::getTypeObject()
{
    return "Object3D";
}

const char *Object3D::getTypeIcon()
{
    return "objectIcon";
}

const std::vector<ScriptLUA *> &Object3D::getScripts() const
{
    return scripts;
}

void Object3D::runStartScripts()
{
    for (auto script : scripts) {
        script->runEnvironment(luaEnvironment, "onStart");
    }
}

bool Object3D::isBelongToScene() const
{
    return belongToScene;
}

void Object3D::setBelongToScene(bool belongToScene)
{
    Object3D::belongToScene = belongToScene;
}

void Object3D::drawImGuiProperties()
{
    auto ImGuiTextures = Brakeza3D::get()->getManagerGui()->getImGuiTextures();

    static char name[256];
    strncpy(name, label.c_str(), sizeof(name));
    ImGui::Image(TexturePackage::getOGLTextureID(*ImGuiTextures, getTypeIcon()), ImVec2(22, 24));
    ImGui::SameLine();
    ImGui::SetNextItemWidth(125.0f);
    ImGui::InputText("Name##nameObject", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_AlwaysOverwrite);
    if (ImGui::IsItemEdited()) {
        setLabel(name);
    }
    ImGui::Separator();

    if (featuresGUI.position || featuresGUI.rotation || featuresGUI.scale) {
        if (ImGui::TreeNode("Transformations")) {
            // position
            if (featuresGUI.position) {
                Tools::ImGuiVertex3D("Position", "X", "Y", "Z", &position, 0.1, -100000, 100000);
                ImGui::Separator();
            }

            // rotation
            if (featuresGUI.rotation) {
                if (ImGui::TreeNode("Rotation")) {
                    const float range_angle_min = -360;
                    const float range_angle_max = 360;

                    float oldPitch = getRotation().getPitchDegree();
                    float oldYaw = getRotation().getYawDegree();
                    float oldRoll = getRotation().getRollDegree();
                    float pitch = oldPitch;
                    float yaw = oldYaw;
                    float roll = oldRoll;

                    const float factor = 0.0025f;
                    if (ImGui::DragScalar("Pitch", ImGuiDataType_Float, &pitch, 0.1f, &range_angle_min,&range_angle_max, "%f", 1.0f)) {
                        auto partialRot = M3::arbitraryAxis(getRotation().X(), Maths::radiansToDegrees(pitch - oldPitch) * factor);
                        setRotation(getRotation() * partialRot);
                        M3::normalize(rotation);
                    }
                    if (ImGui::DragScalar("Yaw", ImGuiDataType_Float, &yaw, 0.1f, &range_angle_min,&range_angle_max, "%f", 1.0f)) {
                        auto partialRot = M3::arbitraryAxis(getRotation().Y(), Maths::radiansToDegrees(yaw - oldYaw) * factor);
                        setRotation(getRotation() * partialRot);
                        M3::normalize(rotation);
                    }
                    if (ImGui::DragScalar("Roll", ImGuiDataType_Float, &roll, 0.1f, &range_angle_min,&range_angle_max, "%f", 1.0f)) {
                        auto partialRot = M3::arbitraryAxis(getRotation().Z(), Maths::radiansToDegrees(roll - oldRoll) * factor);
                        setRotation(getRotation() * partialRot);
                        M3::normalize(rotation);
                    }
                    ImGui::TreePop();
                }
                ImGui::Separator();
            }
            // scale
            if (featuresGUI.scale) {
                if (ImGui::TreeNode("Scale")) {
                    const float range_scale_min = -360;
                    const float range_scale_max = 360;
                    ImGui::DragScalar("Scale", ImGuiDataType_Float, &scale, 0.01, &range_scale_min, &range_scale_max, "%f", 1.0f);

                    ImGui::TreePop();
                }
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
    }
    // alpha
    if (featuresGUI.alpha) {
        if (ImGui::TreeNode("Alpha")) {
            const float range_alpha_min = 0;
            const float range_alpha_max = 1;

            ImGui::DragScalar("Alpha", ImGuiDataType_Float, &getAlpha(), 0.01, &range_alpha_min, &range_alpha_max, "%f", 1.0f);

            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    if (featuresGUI.shaders) {
        if (ImGui::TreeNode("Effects")) {
            if ((int) effects.size() <= 0) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "No shaders attached");
            }

            for (int i = 0; i < (int) effects.size(); i++) {
                ImGui::PushID(i);

                auto s = effects[i];
                ImGui::Image(TexturePackage::getOGLTextureID(*ImGuiTextures, "shaderIcon"), ImVec2(24, 24));
                ImGui::SameLine(100);

                if (!s->isEnabled()) {
                    if (ImGui::ImageButton(TexturePackage::getOGLTextureID(*ImGuiTextures, "unlockIcon"), ImVec2(14, 14))) {
                        s->setEnabled(true);
                    }
                } else {
                    if (ImGui::ImageButton(TexturePackage::getOGLTextureID(*ImGuiTextures, "lockIcon"), ImVec2(14, 14))) {
                        s->setEnabled(false);
                    }
                }
                ImGui::SameLine();
                if (ImGui::ImageButton(TexturePackage::getOGLTextureID(*ImGuiTextures, "removeIcon"), ImVec2(14, 14))) {
                    removeFXOpenGLByIndex(i);
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

    if (featuresGUI.attached) {
        if (ImGui::TreeNode("Attached Objects")) {
            if ((int) attachedObjects.size() <= 0) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Not objects found");
            }

            for (auto a: attachedObjects) {
                if (ImGui::TreeNode(a->getLabel().c_str())) {
                    a->drawImGuiProperties();
                }
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
    }

    if (featuresGUI.attached) {
        if (ImGui::TreeNode("Collider")) {
            if (ImGui::Checkbox("Enable collider", &collisionsEnabled)) {
                if (!collisionsEnabled) {
                    removeCollisionObject();
                } else {
                    setupGhostCollider(CollisionShape::SIMPLE_SHAPE);
                }
            }

            if (collisionsEnabled) {
                drawImGuiCollisionModeSelector();
                drawImGuiCollisionShapeSelector();

                ImGui::Separator();

                if (getCollisionMode() == CollisionMode::BODY) {
                    ImGui::Checkbox("Collider static", &colliderStatic);

                    if (!colliderStatic) {
                        if (ImGui::TreeNode("Mass")) {
                            const float range_min = 0;
                            const float range_max = 1000;

                            ImGui::DragScalar("Mass", ImGuiDataType_Float, &mass, 0.1 ,&range_min, &range_max, "%f", 1.0f);

                            ImGui::TreePop();
                        }
                    }
                }

                ImGui::Separator();

                if (getCollisionMode() == KINEMATIC) {
                    if (ImGui::TreeNode("Kinematic capsule size")) {
                        const float range_min = 0;
                        const float range_max = 1000;

                        ImGui::DragScalar("Radius", ImGuiDataType_Float, &kinematicCapsuleSize.x, 0.1 ,&range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Height", ImGuiDataType_Float, &kinematicCapsuleSize.y, 0.1 ,&range_min, &range_max, "%f", 1.0f);

                        ImGui::TreePop();
                    }
                }

                if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE && (getCollisionMode() == GHOST || getCollisionMode() == BODY)) {
                    const float range_min = -500000;
                    const float range_max = 500000;

                    Tools::ImGuiVertex3D("Simple shape size", "X", "Y", "Z", &simpleShapeSize, 0.1f ,range_min, range_max);
                    ImGui::Separator();

                    if (ImGui::TreeNode("Angular factor")) {
                        const float range_min = 0;
                        const float range_max = 1;

                        ImGui::DragScalar("X", ImGuiDataType_Float, &angularFactor.x, 0.1 ,&range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_Float, &angularFactor.y, 0.1 ,&range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Z", ImGuiDataType_Float, &angularFactor.z, 0.1 ,&range_min, &range_max, "%f", 1.0f);

                        ImGui::TreePop();
                    }
                    ImGui::Separator();

                    if (ImGui::TreeNode("Linear factor")) {
                        const float range_min = 0;
                        const float range_max = 1;

                        ImGui::DragScalar("X", ImGuiDataType_Float, &linearFactor.x, 0.1 ,&range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Y", ImGuiDataType_Float, &linearFactor.y, 0.1 ,&range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Z", ImGuiDataType_Float, &linearFactor.z, 0.1 ,&range_min, &range_max, "%f", 1.0f);

                        ImGui::TreePop();
                    }
                    ImGui::Separator();
                    if (ImGui::TreeNode("Friction")) {
                        const float range_min = 0;
                        const float range_max = 1;

                        ImGui::DragScalar("Value", ImGuiDataType_Float, &friction, 0.1 ,&range_min, &range_max, "%f", 1.0f);

                        ImGui::TreePop();
                    }

                }

                ImGui::Separator();
                if (ImGui::Button(std::string("Update collision shape").c_str())) {
                    UpdateShape();
                }
            }
            ImGui::TreePop();
        }
        ImGui::Separator();
    }
}

cJSON *Object3D::getJSON()
{
    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "name", getLabel().c_str());
    cJSON_AddNumberToObject(root, "scale", getScale());

    cJSON *position = cJSON_CreateObject();
    cJSON_AddNumberToObject(position, "x", (float) getPosition().x);
    cJSON_AddNumberToObject(position, "y", (float) getPosition().y);
    cJSON_AddNumberToObject(position, "z", (float) getPosition().z);
    cJSON_AddItemToObject(root, "position", position);

    cJSON *rotation = cJSON_CreateObject();
    cJSON_AddNumberToObject(rotation, "x", (float) getRotation().getPitchDegree());
    cJSON_AddNumberToObject(rotation, "y", (float) getRotation().getYawDegree());
    cJSON_AddNumberToObject(rotation, "z", (float) getRotation().getRollDegree());
    cJSON_AddItemToObject(root, "rotation", rotation);

    cJSON_AddBoolToObject(root, "isCollisionsEnabled", isCollisionsEnabled());
    if (isCollisionsEnabled()) {
        cJSON *collider = cJSON_CreateObject();
        cJSON_AddNumberToObject(collider, "mode", getCollisionMode());
        cJSON_AddNumberToObject(collider, "shape", getCollisionShape());

        cJSON_AddNumberToObject(collider, "friction", friction);
        cJSON_AddNumberToObject(collider, "mass", mass);
        cJSON_AddBoolToObject(collider, "colliderStatic", colliderStatic);

        cJSON *simpleShapeSizeJSON = cJSON_CreateObject();
        cJSON_AddNumberToObject(simpleShapeSizeJSON, "x", simpleShapeSize.x);
        cJSON_AddNumberToObject(simpleShapeSizeJSON, "y", simpleShapeSize.y);
        cJSON_AddNumberToObject(simpleShapeSizeJSON, "z", simpleShapeSize.z);
        cJSON_AddItemToObject(collider, "simpleShapeSize", simpleShapeSizeJSON);

        cJSON *kinematicCapsuleSizeJSON = cJSON_CreateObject();
        cJSON_AddNumberToObject(kinematicCapsuleSizeJSON, "x", kinematicCapsuleSize.x);
        cJSON_AddNumberToObject(kinematicCapsuleSizeJSON, "y", kinematicCapsuleSize.y);
        cJSON_AddItemToObject(collider, "kinematicCapsuleSize", kinematicCapsuleSizeJSON);

        cJSON *angularFactorJSON = cJSON_CreateObject();
        cJSON_AddNumberToObject(angularFactorJSON, "x", angularFactor.x);
        cJSON_AddNumberToObject(angularFactorJSON, "y", angularFactor.y);
        cJSON_AddNumberToObject(angularFactorJSON, "z", angularFactor.z);
        cJSON_AddItemToObject(collider, "angularFactor", angularFactorJSON);

        cJSON *linearFactorJSON = cJSON_CreateObject();
        cJSON_AddNumberToObject(linearFactorJSON, "x", linearFactor.x);
        cJSON_AddNumberToObject(linearFactorJSON, "y", linearFactor.y);
        cJSON_AddNumberToObject(linearFactorJSON, "z", linearFactor.z);
        cJSON_AddItemToObject(collider, "linearFactor", linearFactorJSON);

        cJSON_AddItemToObject(root, "collider", collider);
    }

    cJSON *scriptsArray = cJSON_CreateArray();
    for (auto script : scripts) {
        cJSON_AddItemToArray(scriptsArray, script->getTypesJSON());
    }
    cJSON_AddItemToObject(root, "scripts", scriptsArray);

    cJSON *effectsArrayJSON = cJSON_CreateArray();
    for ( auto s : effects) {
        cJSON_AddItemToArray(effectsArrayJSON, s->getJSON());
    }
    cJSON_AddItemToObject(root, "effects", effectsArrayJSON);

    return root;
}

void Object3D::setPropertiesFromJSON(cJSON *object, Object3D *o)
{
    o->setBelongToScene(true);

    o->setPosition(ToolsJSON::parseVertex3DJSON(cJSON_GetObjectItemCaseSensitive(object, "position")));
    o->setScale((float)cJSON_GetObjectItemCaseSensitive(object, "scale")->valuedouble);

    if (cJSON_GetObjectItemCaseSensitive(object, "rotation") != nullptr) {
        cJSON *rotation = cJSON_GetObjectItemCaseSensitive(object, "rotation");

        auto dX = (float) cJSON_GetObjectItemCaseSensitive(rotation, "x")->valuedouble;
        auto dY = (float) cJSON_GetObjectItemCaseSensitive(rotation, "y")->valuedouble;
        auto dZ = (float) cJSON_GetObjectItemCaseSensitive(rotation, "z")->valuedouble;

        M3 MRX = M3::RX(dX);
        M3 MRY = M3::RY(dY);
        M3 MRZ = M3::RZ(dZ);
        auto r = MRZ * MRY * MRX;
        M3::normalize(r);
        o->setRotation(r);
    }

    if (cJSON_GetObjectItemCaseSensitive(object, "isCollisionsEnabled") != nullptr) {
        bool collisionsEnabled = cJSON_GetObjectItemCaseSensitive(object, "isCollisionsEnabled")->valueint;
        cJSON *colliderJSON = cJSON_GetObjectItemCaseSensitive(object, "collider");

        if (collisionsEnabled) {

            o->setCollisionsEnabled(true);
            if ((cJSON_GetObjectItemCaseSensitive(colliderJSON, "colliderStatic") != nullptr)) {
                o->setColliderStatic(cJSON_GetObjectItemCaseSensitive(colliderJSON, "colliderStatic")->valueint);
            }

            int mode = (int) cJSON_GetObjectItemCaseSensitive(colliderJSON, "mode")->valueint;
            int shape = (int) cJSON_GetObjectItemCaseSensitive(colliderJSON, "shape")->valueint;
            auto mass = (float) cJSON_GetObjectItemCaseSensitive(colliderJSON, "mass")->valuedouble;
            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "friction") != nullptr) {
                auto friction = (float) cJSON_GetObjectItemCaseSensitive(colliderJSON, "friction")->valuedouble;
                o->setFriction(friction);
            }

            o->simpleShapeSize = ToolsJSON::parseVertex3DJSON(
                cJSON_GetObjectItemCaseSensitive(colliderJSON, "simpleShapeSize")
            );
            o->setMass(mass);

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "kinematicCapsuleSize") != nullptr) {
                cJSON *kinematizSizeJSON = cJSON_GetObjectItemCaseSensitive(colliderJSON, "kinematicCapsuleSize");

                o->setKinematicSize(
                    (float) cJSON_GetObjectItemCaseSensitive(kinematizSizeJSON, "x")->valuedouble,
                    (float) cJSON_GetObjectItemCaseSensitive(kinematizSizeJSON, "y")->valuedouble
                );
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "angularFactor") != nullptr) {
                o->angularFactor = ToolsJSON::parseVertex3DJSON(
                    cJSON_GetObjectItemCaseSensitive(colliderJSON, "angularFactor")
                );
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "linearFactor") != nullptr) {
                o->linearFactor = ToolsJSON::parseVertex3DJSON(
                    cJSON_GetObjectItemCaseSensitive(colliderJSON, "linearFactor")
                );
            }

            switch(mode) {
                case CollisionMode::GHOST:
                    if (shape == CollisionShape::SIMPLE_SHAPE) {
                        o->setupGhostCollider(CollisionShape::SIMPLE_SHAPE);
                    }
                    break;
                case CollisionMode::BODY:
                    if (shape == CollisionShape::SIMPLE_SHAPE) {
                        o->setupRigidBodyCollider(CollisionShape::SIMPLE_SHAPE);
                    }
                    break;
                case CollisionMode::KINEMATIC:
                    o->setupKinematicCollider();
                    break;
            }
        }
    }

    if (cJSON_GetObjectItemCaseSensitive(object, "scripts") != nullptr) {
        cJSON *currentScript;
        cJSON_ArrayForEach(currentScript, cJSON_GetObjectItemCaseSensitive(object, "scripts")) {
            auto filename = cJSON_GetObjectItemCaseSensitive(currentScript, "name")->valuestring;
            auto typesJSON = cJSON_GetObjectItemCaseSensitive(currentScript, "types");
            o->attachScript(new ScriptLUA(filename, typesJSON));
        }
    }

    if (cJSON_GetObjectItemCaseSensitive(object, "effects") != nullptr) {
        auto mesh3DShaderTypes = ComponentsManager::get()->getComponentRender()->getSceneLoader().getFXOpenGLTypes();
        cJSON *currentShader;
        cJSON_ArrayForEach(currentShader, cJSON_GetObjectItemCaseSensitive(object, "shaders")) {
            auto type = cJSON_GetObjectItemCaseSensitive(currentShader, "type")->valuestring;
            switch(mesh3DShaderTypes[type]) {
                case FXOpenGLLoaderMapping::FXTint: {
                    auto c = cJSON_GetObjectItemCaseSensitive(currentShader, "color");
                    auto alpha = (float) cJSON_GetObjectItemCaseSensitive(currentShader, "alpha")->valuedouble;
                    auto enabled = (bool) cJSON_GetObjectItemCaseSensitive(currentShader, "enabled")->valueint;
                    auto shader = new FXColorTint(enabled, ToolsJSON::parseColorJSON(c), alpha);
                    o->addFXOpenGL(shader);
                    break;
                }
                case FXOpenGLLoaderMapping::FXOutliner: {
                    auto c = cJSON_GetObjectItemCaseSensitive(currentShader, "color");
                    auto size = (float) cJSON_GetObjectItemCaseSensitive(currentShader, "size")->valuedouble;
                    auto enabled = (bool) cJSON_GetObjectItemCaseSensitive(currentShader, "enabled")->valueint;
                    auto color = ToolsJSON::parseColorJSON(c);
                    auto shader = new FXOutliner(enabled, o, color, size);
                    o->addFXOpenGL(shader);
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

M3 Object3D::getM3ModelMatrix()
{
    return M3::fromMat3GLM(getModelMatrix());
}

glm::mat4 Object3D::getModelMatrix()
{
    glm::vec3 scaled(scale);
    glm::vec3 translated = position.toGLM();
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translated);
    glm::mat4 rotationMatrix = glm::mat4(getRotation().toGLMMat3());
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scaled);

    return translationMatrix * rotationMatrix * scaleMatrix;
}

void Object3D::addFXOpenGL(FXOpenGL *fx)
{
    effects.push_back(fx);
}

void Object3D::removeFXOpenGLByIndex(int index)
{
    if (index >= 0 && index < (int) effects.size()) {
        effects.erase(effects.begin() + index);
    }
}

const Timer &Object3D::getTimer() const
{
    return timer;
}

float Object3D::getDistanceToCamera() const
{
    return distanceToCamera;
}

bool Object3D::isMultiScene() const
{
    return multiScene;
}

void Object3D::setMultiScene(bool multiScene)
{
    Object3D::multiScene = multiScene;
}

const std::vector<Object3D *> &Object3D::getAttached() const
{
    return attachedObjects;
}

void Object3D::attachObject(Object3D* o)
{
    attachedObjects.push_back(o);
}

sol::object Object3D::getLocalScriptVar(const char *varName)
{
    return luaEnvironment[varName];
}

void Object3D::makeKineticBody(float x, float y, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    Logging::Message("[Object3D] makeKineticBody for %s", getLabel().c_str());

    btTransform t;
    t.setIdentity();
    t.setOrigin(getPosition().toBullet());

    kinematicBody = new btPairCachingGhostObject();
    kinematicBody->setWorldTransform(t);

    auto capsule = new btCapsuleShapeZ(kinematicCapsuleSize.x, kinematicCapsuleSize.y);
    btVector3 inertia(0, 0, 0);
    capsule->calculateLocalInertia(mass, inertia);
    kinematicBody->setCollisionShape(capsule);
    kinematicBody->setUserPointer(this);
    kinematicBody->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    characterController = new btKinematicCharacterController(
        kinematicBody,
        (btConvexShape*)kinematicBody->getCollisionShape(),
        0.1f
    );

    characterController->setGravity(EngineSetup::get()->gravity.toBullet());
    characterController->setUseGhostSweepTest(false);
    characterController->setMaxSlope(btRadians(45));       // Pendiente máxima

    world->addCollisionObject(
            kinematicBody,
        EngineSetup::collisionGroups::AllFilter,
        EngineSetup::collisionGroups::AllFilter
    );

    world->addAction(characterController);
}

void Object3D::makeSimpleRigidBody(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    Logging::Message("[Object3D] makeSimpleRigidBody for %s", getLabel().c_str());

    setMass(mass);

    btTransform transformation;
    transformation.setIdentity();
    transformation.setOrigin(getPosition().toBullet());

    btMatrix3x3 brakezaRotation = rotation.toBulletMat3();
    btQuaternion qRotation;
    brakezaRotation.getRotation(qRotation);

    transformation.setRotation(qRotation);

    btCollisionShape *collisionShape = new btBoxShape(simpleShapeSize.toBullet());
    btVector3 inertia(0, 0, 0);
    collisionShape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo cInfo(
        mass,
        new btDefaultMotionState(transformation),
        collisionShape,
        inertia
    );

    body = new btRigidBody(cInfo);
    body->activate(true);
    body->setUserPointer(this);
    body->setRestitution(0.5);
    body->setActivationState(DISABLE_DEACTIVATION);
    body->setAngularFactor(angularFactor.toBullet());
    body->setLinearFactor(linearFactor.toBullet());
    body->setFriction(friction);

    world->addRigidBody(body, collisionGroup, collisionMask);
}

void Object3D::integrate()
{
    if (getCollisionMode() == CollisionMode::GHOST) {
        getGhostObject()->setWorldTransform(
            Tools::GLMMatrixToBulletTransform(getModelMatrix())
        );
    }

    if (getCollisionMode() == CollisionMode::BODY) {
        updateFromBullet();
    }

    if (getCollisionMode() == CollisionMode::KINEMATIC) {
        btTransform t = kinematicBody->getWorldTransform();
        setPosition(Vertex3D::fromBullet(t.getOrigin()));
    }
}

void Object3D::updateFromBullet()
{
    if (this->body == nullptr || this->mass <= 0) {
        return;
    }

    btTransform t;
    body->getMotionState()->getWorldTransform(t);

    setPosition(Vertex3D::fromBullet(t.getOrigin()));

    btMatrix3x3 matrixRotation;
    matrixRotation.setRotation(t.getRotation());

    setRotation(M3::fromMat3Bullet(matrixRotation));
}

void Object3D::resolveCollision(Collider *with)
{
    if (EngineSetup::get()->LOG_COLLISION_OBJECTS) {
        auto *object = dynamic_cast<Object3D*> (with);
        Logging::Message("Object3D: Collision %s with %s",  getLabel().c_str(), object->getLabel().c_str());
    }

    if (ComponentsManager::get()->getComponentScripting()->getStateLUAScripts() == EngineSetup::LUA_PLAY) {
        runResolveCollisionScripts(with);
    }
}

void Object3D::runResolveCollisionScripts(Collider *with)
{
    auto *object = dynamic_cast<Object3D*> (with);
    const sol::state &lua = ComponentsManager::get()->getComponentScripting()->getLua();

    sol::object luaValue = sol::make_object(lua, object);

    for (auto script : scripts) {
        script->runEnvironment(luaEnvironment, "onCollision", luaValue);
    }
}

const sol::environment &Object3D::getLuaEnvironment() const {
    return luaEnvironment;
}
