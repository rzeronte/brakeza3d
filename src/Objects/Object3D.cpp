
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
    for (auto a: attachedObjects) {
        delete a;
    }

    if (isCollisionsEnabled()) {
        removeCollisionObject();
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
    Vertex3D direction = (o->getPosition() - position).getInverse().getNormalize();

    Vertex3D rightVector = Vertex3D(0, 0, 1) % (direction).getNormalize();
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
        if (ImGui::CollapsingHeader("Transformations")) {
            // position
            if (featuresGUI.position) {
                float vec3f[3];
                getPosition().toFloat(vec3f);
                if (ImGui::DragFloat3("Position", vec3f, 0.01f, -999999.0f, 999999.0f)) {
                    position.x = vec3f[0];
                    position.y = vec3f[1];
                    position.z = vec3f[2];
                }
                ImGui::Separator();
            }

            // rotation
            if (featuresGUI.rotation) {
                float oldPitch = getRotation().getPitchDegree();
                float oldYaw = getRotation().getYawDegree();
                float oldRoll = getRotation().getRollDegree();
                float pitch = oldPitch;
                float yaw = oldYaw;
                float roll = oldRoll;
                const float factor = 0.0025f;

                float vec3f[3];
                vec3f[0] = pitch;
                vec3f[1] = yaw;
                vec3f[2] = roll;
                if (ImGui::DragFloat3("Rotation", vec3f, 0.01f, -999999.0f, 999999.0f)) {
                    pitch = vec3f[0];
                    yaw = vec3f[1];
                    roll = vec3f[2];
                    if (abs(pitch - oldPitch) > 0) {
                        auto partialRotX = M3::arbitraryAxis(getRotation().X(), Maths::radiansToDegrees(pitch - oldPitch) * factor);
                        setRotation(getRotation() * partialRotX);
                        M3::normalize(rotation);
                    }

                    if (abs(yaw - oldYaw) > 0) {
                        auto partialRotY = M3::arbitraryAxis(getRotation().Y(), Maths::radiansToDegrees(yaw - oldYaw) * factor);
                        setRotation(getRotation() * partialRotY);
                        M3::normalize(rotation);
                    }

                    if (abs(roll - oldRoll) > 0) {
                        auto partialRotZ = M3::arbitraryAxis(getRotation().Z(), Maths::radiansToDegrees(roll - oldRoll) * factor);
                        setRotation(getRotation() * partialRotZ);
                        M3::normalize(rotation);
                    }
                }
                ImGui::Separator();
            }
            // scale
            if (featuresGUI.scale) {
                const float range_scale_min = -360;
                const float range_scale_max = 360;
                ImGui::DragScalar("Scale", ImGuiDataType_Float, &scale, 0.01, &range_scale_min, &range_scale_max, "%f", 1.0f);
            }
        }
    }
    // alpha
    if (featuresGUI.alpha) {
        if (ImGui::CollapsingHeader("Alpha")) {
            const float range_alpha_min = 0;
            const float range_alpha_max = 1;

            ImGui::DragScalar("Alpha##001", ImGuiDataType_Float, &getAlpha(), 0.01, &range_alpha_min, &range_alpha_max, "%f", 1.0f);
        }
    }

    if (featuresGUI.attached) {
        if (ImGui::CollapsingHeader("Attached Objects")) {
            if ((int) attachedObjects.size() <= 0) {
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", "Not objects found");
            }

            for (auto a: attachedObjects) {
                if (ImGui::TreeNode(a->getLabel().c_str())) {
                    a->drawImGuiProperties();
                    ImGui::TreePop();
                }
            }
        }
    }

    if (featuresGUI.attached) {
        if (ImGui::CollapsingHeader("Collider")) {
            if (ImGui::Checkbox("Enable collider", &collisionsEnabled)) {
                if (!collisionsEnabled) {
                    removeCollisionObject();
                } else {
                    setupGhostCollider(CollisionShape::SIMPLE_SHAPE);
                }
            }

            if (collisionsEnabled) {
                if (ImGui::Button(std::string("Update Collider").c_str())) {
                    UpdateShapeCollider();
                }
                ImGui::Separator();

                drawImGuiCollisionModeSelector();
                if (getCollisionMode() != CollisionMode::KINEMATIC) {
                    drawImGuiCollisionShapeSelector();
                }

                if (ImGui::TreeNode("Collider settings")) {
                    if (getCollisionMode() == CollisionMode::BODY) {
                        ImGui::Separator();

                        ImGui::Checkbox("Collider static", &colliderStatic);

                        ImGui::Separator();

                        ImGui::DragFloat("CCD Motion Treshold", &ccdMotionThreshold, 0.001f, 0.0f, 5.0f);

                        ImGui::Separator();

                        ImGui::DragFloat("CCD Swept Sphere Radius", &ccdSweptSphereRadius, 0.001f, 0.0f, 5.0f);
                    }

                    if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE) {
                        ImGui::Separator();
                        float vec3f[3];
                        simpleShapeSize.toFloat(vec3f);
                        if (ImGui::DragFloat3("Shape Size", vec3f, 0.01f, -1000.0f, 1000.0f)) {
                            simpleShapeSize.x = vec3f[0];
                            simpleShapeSize.y = vec3f[1];
                            simpleShapeSize.z = vec3f[2];
                        }
                    }

                    if (getCollisionMode() == KINEMATIC || getCollisionShape() == CAPSULE) {
                        ImGui::Separator();
                        const float range_min = 0;
                        const float range_max = 1000;

                        ImGui::DragScalar("Capsule radius", ImGuiDataType_Float, &kinematicCapsuleSize.x, 0.1 ,&range_min, &range_max, "%f", 1.0f);
                        ImGui::DragScalar("Capsule height", ImGuiDataType_Float, &kinematicCapsuleSize.y, 0.1 ,&range_min, &range_max, "%f", 1.0f);
                    }

                    ImGui::Separator();

                    if (getCollisionMode() == CollisionMode::BODY) {

                        if (!colliderStatic) {
                            ImGui::DragFloat("Mass", &mass, 0.1f, 0.0f, 5000.0f);
                            ImGui::Separator();
                        }

                        if ((getCollisionMode() == GHOST || getCollisionMode() == BODY)) {
                            float vec3f[3];
                            angularFactor.toFloat(vec3f);
                            if (ImGui::DragFloat3("Angular factor", vec3f, 0.01f, 0.0f, 1.0f)) {
                                angularFactor.x = vec3f[0];
                                angularFactor.y = vec3f[1];
                                angularFactor.z = vec3f[2];
                            }

                            ImGui::Separator();

                            linearFactor.toFloat(vec3f);
                            if (ImGui::DragFloat3("Linear Factor", vec3f, 0.01f, 0.0f, 1.0f)) {
                                linearFactor.x = vec3f[0];
                                linearFactor.y = vec3f[1];
                                linearFactor.z = vec3f[2];
                            }
                            ImGui::Separator();

                            ImGui::DragFloat("Margin", &shapeMargin, 0.01f, 0.0f, 1.0f);

                            ImGui::Separator();

                            ImGui::DragFloat("Friction", &friction, 0.01f, 0.0f, 1.0f);

                            ImGui::Separator();

                            ImGui::DragFloat("Linear Damping", &linearDamping, 0.01f, 0.0f, 1.0f);
                            ImGui::DragFloat("Angular Damping", &angularDamping, 0.01f, 0.0f, 1.0f);

                            ImGui::Separator();

                            ImGui::DragFloat("Restitution", &restitution, 0.01f, 0.0f, 1.0f);
                        }
                    }

                    ImGui::TreePop();
                }

                drawImGuiVariables();

            }
        }
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
        cJSON_AddNumberToObject(collider, "margin", shapeMargin);
        cJSON_AddNumberToObject(collider, "ccdMotionThreshold", ccdMotionThreshold);
        cJSON_AddNumberToObject(collider, "ccdSweptSphereRadius", ccdSweptSphereRadius);
        cJSON_AddNumberToObject(collider, "linearDamping", linearDamping);
        cJSON_AddNumberToObject(collider, "angularDamping", angularDamping);
        cJSON_AddNumberToObject(collider, "restitution", restitution);
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

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "ccdMotionThreshold") != nullptr) {
                auto ccdMotionThreshold = (float) cJSON_GetObjectItemCaseSensitive(colliderJSON, "ccdMotionThreshold")->valuedouble;
                o->setCcdMotionThreshold(ccdMotionThreshold);
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "ccdSweptSphereRadius") != nullptr) {
                auto ccdSweptSphereRadius = (float) cJSON_GetObjectItemCaseSensitive(colliderJSON, "ccdSweptSphereRadius")->valuedouble;
                o->setCcdSweptSphereRadius(ccdSweptSphereRadius);
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "margin") != nullptr) {
                auto margin = (float) cJSON_GetObjectItemCaseSensitive(colliderJSON, "margin")->valuedouble;
                o->setShapeMargin(margin);
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "restitution") != nullptr) {
                auto restitution = (float) cJSON_GetObjectItemCaseSensitive(colliderJSON, "restitution")->valuedouble;
                o->setRestitution(restitution);
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "linearDamping") != nullptr) {
                auto linearDamping = (float) cJSON_GetObjectItemCaseSensitive(colliderJSON, "linearDamping")->valuedouble;
                o->setLinearDamping(linearDamping);
            }

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "angularDamping") != nullptr) {
                auto angularDamping = (float) cJSON_GetObjectItemCaseSensitive(colliderJSON, "angularDamping")->valuedouble;
                o->setAngularDamping(angularDamping);
            }

            o->simpleShapeSize = ToolsJSON::parseVertex3DJSON(
                cJSON_GetObjectItemCaseSensitive(colliderJSON, "simpleShapeSize")
            );
            o->setMass(mass);

            if (cJSON_GetObjectItemCaseSensitive(colliderJSON, "kinematicCapsuleSize") != nullptr) {
                cJSON *kinematizSizeJSON = cJSON_GetObjectItemCaseSensitive(colliderJSON, "kinematicCapsuleSize");

                o->setCapsuleColliderSize(
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
    capsule->setMargin(shapeMargin);

    btVector3 inertia(0, 0, 0);
    if (mass > 0) {
        capsule->calculateLocalInertia(mass, inertia);
    }
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
    btCollisionShape *collisionShape;

    if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE) {
        collisionShape = new btBoxShape(simpleShapeSize.toBullet());
    }

    if (getCollisionShape() == CollisionShape::CAPSULE) {
        collisionShape = new btCapsuleShape(kinematicCapsuleSize.x, kinematicCapsuleSize.y);
    }

    collisionShape->setMargin(shapeMargin);

    btVector3 inertia(0, 0, 0);
    if (mass > 0) {
        collisionShape->calculateLocalInertia(mass, inertia);
    }

    btRigidBody::btRigidBodyConstructionInfo cInfo(
        mass,
        new btDefaultMotionState(transformation),
        collisionShape,
        inertia
    );

    body = new btRigidBody(cInfo);
    body->activate(true);
    body->setUserPointer(this);
    body->setRestitution(restitution);
    body->setAngularFactor(angularFactor.toBullet());
    body->setLinearFactor(linearFactor.toBullet());
    body->setFriction(friction);
    body->setDamping(linearDamping, angularDamping);
    body->setCcdMotionThreshold(ccdMotionThreshold);
    body->setCcdSweptSphereRadius(ccdSweptSphereRadius);

    world->addRigidBody(body, collisionGroup, collisionMask);
}

void Object3D::integrate()
{
    if (!isCollisionsEnabled()) return;

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

void Object3D::setupGhostCollider(CollisionShape mode)
{
    Logging::Message("[Collider] setupGhostCollider");
    removeCollisionObject();

    setCollisionMode(CollisionMode::GHOST);
    setCollisionShape(mode);

    if (getCollisionShape() == CollisionShape::SIMPLE_SHAPE || getCollisionShape() == CollisionShape::CAPSULE) {
        makeSimpleGhostBody(
            getPosition(),
            getModelMatrix(),
            simpleShapeSize,
            Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            EngineSetup::collisionGroups::AllFilter,
            EngineSetup::collisionGroups::AllFilter
        );
    }
}

void Object3D::checkClickObject(Vector3D ray, Object3D*& foundObject, float &lastDepthFound)
{
}
