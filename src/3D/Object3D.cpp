
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "../../include/3D/Object3D.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Misc/ToolsJSON.h"
#include "../../include/Components/ComponentsManager.h"
#include "../../include/Brakeza.h"
#include "../../include/Render/Drawable.h"
#include "../../include/GUI/Objects/Object3DGUI.h"

Object3D::Object3D()
:
    id(Brakeza::get()->getNextObjectID()),
    luaEnvironment(sol::environment(
        ComponentsManager::get()->getComponentScripting()->getLua(),
        sol::create, ComponentsManager::get()->getComponentScripting()->getLua().globals())
    ),
    pickingColor(Color::idToColor(id))
{
    luaEnvironment["this"] = this;
    timer.start();
}

Vertex3D &Object3D::getPosition()
{
    return position;
}

M3 Object3D::getRotation() const
{
    return rotation;
}

void Object3D::setPosition(const Vertex3D &p)
{
    position = p;
}

void Object3D::setRotation(const M3 &r)
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

void Object3D::setEnabled(bool value)
{
    Object3D::enabled = value;
}

Vertex3D Object3D::AxisUp() const
{
    Vertex3D v = getRotation() * BrakezaSetup::get()->up;
    return v.getNormalize();
}

Vertex3D Object3D::AxisDown() const
{
    Vertex3D v = getRotation() * BrakezaSetup::get()->down;
    return v.getNormalize();
}

Vertex3D Object3D::AxisForward() const
{
    Vertex3D v = getRotation() * BrakezaSetup::get()->forward;
    return v.getNormalize();
}

Vertex3D Object3D::AxisBackwards() const
{
    Vertex3D v = getRotation() * BrakezaSetup::get()->backward;

    return v.getNormalize();
}

Vertex3D Object3D::AxisRight() const
{
    Vertex3D v = getRotation() * BrakezaSetup::get()->right;

    return v.getNormalize();
}

Vertex3D Object3D::AxisLeft() const
{
    Vertex3D v = getRotation() * BrakezaSetup::get()->left;

    return v.getNormalize();
}

float Object3D::getScale() const
{
    return scale;
}

void Object3D::setScale(float value)
{
    scale = value;
}

bool Object3D::isRemoved() const
{
    return removed;
}

void Object3D::setRemoved(bool value)
{
    removed = value;
}

bool Object3D::isDecal() const
{
    return decal;
}

void Object3D::setDecal(bool value)
{
    decal = value;
}

void Object3D::setDrawOffset(const Vertex3D &offset)
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
    parent = object;
}

void Object3D::onUpdate()
{
    if (isRemoved() || !isEnabled()) return;

    for (auto a: attachedObjects) {
        if (a->isEnabled()) a->onUpdate();
    }

    distanceToCamera = ComponentsManager::get()->getComponentCamera()->getCamera()->getPosition().distance(getPosition());

    if (ComponentsManager::get()->getComponentScripting()->getStateLUAScripts() == BrakezaSetup::LUA_PLAY) {
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

    if (BrakezaSetup::get()->RENDER_OBJECTS_AXIS) {
        Drawable::drawObject3DAxis(this,true,true,true);
    }
}

void Object3D::drawImGuiProperties()
{
    Object3DGUI::drawImGuiProperties(this);
}

void Object3D::addToPosition(const Vertex3D &v)
{
    this->position = this->position + v;
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

void Object3D::setAlphaEnabled(bool value)
{
    alphaEnabled = value;
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

void Object3D::setEnableLights(bool value)
{
    enableLights = value;
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

void Object3D::reloadScriptsCode() const
{
    for (auto script : scripts) {
        script->reloadScriptCode();
    }
}

void Object3D::removeScript(const ScriptLUA *script)
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
    return SceneObjectTypes::OBJECT_3D;
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

bool& Object3D::enabledPointer()
{
    return enabled;
}

Vertex3D & Object3D::positionPointer()
{
    return position;
}

M3 Object3D::getM3ModelMatrix() const
{
    return M3::fromMat3GLM(getModelMatrix());
}

glm::mat4 Object3D::getModelMatrix() const
{
    glm::vec3 scaled(scale);
    glm::vec3 translated = position.toGLM();
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), translated);
    glm::mat4 rotationMatrix = glm::mat4(getRotation().toGLMMat3());
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), scaled);

    return translationMatrix * rotationMatrix * scaleMatrix;
}

M3 & Object3D::rotationPointer()
{
    return rotation;
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
    kinematicBody->setUserIndex(BrakezaSetup::CollisionSource::OBJECT_COLLIDER);
    kinematicBody->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    characterController = new btKinematicCharacterController(
        kinematicBody,
        static_cast<btConvexShape *>(kinematicBody->getCollisionShape()),
        0.1f
    );

    characterController->setGravity(BrakezaSetup::get()->gravity.toBullet());
    characterController->setUseGhostSweepTest(false);
    characterController->setMaxSlope(btRadians(45)); // Max slope

    world->addCollisionObject(
            kinematicBody,
        BrakezaSetup::collisionGroups::AllFilter,
        BrakezaSetup::collisionGroups::AllFilter
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
    btCollisionShape *collisionShape = nullptr;

    if (getCollisionShape() == SIMPLE_SHAPE) {
        collisionShape = new btBoxShape(simpleShapeSize.toBullet());
    }

    if (getCollisionShape() == CAPSULE) {
        collisionShape = new btCapsuleShape(kinematicCapsuleSize.x, kinematicCapsuleSize.y);
    }

    if (collisionShape == nullptr) {
        printf("Collider Shape not valid!!. Exiting...");
        exit(-1);
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
    body->setUserIndex(BrakezaSetup::CollisionSource::OBJECT_COLLIDER);
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

    if (getCollisionMode() == GHOST) {
        getGhostObject()->setWorldTransform(
            Tools::GLMMatrixToBulletTransform(getModelMatrix())
        );
    }

    if (getCollisionMode() == BODY) {
        updateFromBullet();
    }

    if (getCollisionMode() == KINEMATIC) {
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

void Object3D::resolveCollision(CollisionInfo with)
{
    if (BrakezaSetup::get()->LOG_COLLISION_OBJECTS) {
        auto *object = static_cast<Object3D *>(with.with);
        Logging::Message("Object3D: Collision %s with %s",  getLabel().c_str(), object->getLabel().c_str());
    }

    if (ComponentsManager::get()->getComponentScripting()->getStateLUAScripts() == BrakezaSetup::LUA_PLAY) {
        runResolveCollisionScripts(with);
    }
}

void Object3D::runResolveCollisionScripts(CollisionInfo with)
{
    //auto *object = static_cast<Object3D *>(with.with);
    const sol::state &lua = ComponentsManager::get()->getComponentScripting()->getLua();

    sol::object luaValue = sol::make_object(lua, with);

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

    setCollisionMode(GHOST);
    setCollisionShape(mode);

    if (getCollisionShape() == SIMPLE_SHAPE || getCollisionShape() == CAPSULE) {
        makeSimpleGhostBody(
            getPosition(),
            getModelMatrix(),
            simpleShapeSize,
            Brakeza::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
            BrakezaSetup::collisionGroups::AllFilter,
            BrakezaSetup::collisionGroups::AllFilter
        );
    }
}

void Object3D::checkClickObject(Vector3D ray, Object3D*& foundObject, float &lastDepthFound)
{
}

Color Object3D::getPickingColor() const
{
    return pickingColor;
}

int Object3D::getId() const
{
    return id;
}