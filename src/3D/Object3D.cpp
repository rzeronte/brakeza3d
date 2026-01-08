
#include <iostream>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "../../include/3D/Object3D.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Components/Components.h"
#include "../../include/Brakeza.h"
#include "../../include/Render/Drawable.h"
#include "../../include/GUI/Objects/Object3DGUI.h"

Object3D::Object3D()
:
    id(Brakeza::getNextUniqueObjectId()),
    luaEnvironment(sol::environment(
        Components::get()->Scripting()->getLua(),
        sol::create, Components::get()->Scripting()->getLua().globals())
    ),
    pickingColor(Color::idToColor(id)),
    type(ObjectType::Object3D)
{
    luaEnvironment["this"] = this;
}

Vertex3D Object3D::up() const
{
    Vertex3D v = getRotation() * Config::get()->up;
    return v.getNormalize();
}

Vertex3D Object3D::down() const
{
    Vertex3D v = getRotation() * Config::get()->down;
    return v.getNormalize();
}

Vertex3D Object3D::forward() const
{
    Vertex3D v = getRotation() * Config::get()->forward;
    return v.getNormalize();
}

Vertex3D Object3D::backwards() const
{
    Vertex3D v = getRotation() * Config::get()->backward;

    return v.getNormalize();
}

Vertex3D Object3D::right() const
{
    Vertex3D v = getRotation() * Config::get()->right;

    return v.getNormalize();
}

Vertex3D Object3D::left() const
{
    Vertex3D v = getRotation() * Config::get()->left;

    return v.getNormalize();
}

void Object3D::onUpdate()
{
    if (isRemoved() || !isEnabled()) return;

    for (auto &a: attachedObjects) {
        if (!a->isEnabled()) continue;
        a->onUpdate();
    }

    distanceToCamera = Components::get()->Camera()->getCamera()->getPosition().distance(getPosition());

    if (Components::get()->Scripting()->isExecuting()) {
        RunScripts();
    }
}

void Object3D::RunScripts()
{
    for (auto &script: scripts) {
        script->RunEnvironment(luaEnvironment, "onUpdate");
        if (isGUISelected()) {
            script->RunEnvironment(luaEnvironment, "onClick");
        }
    }
}

void Object3D::postUpdate()
{
    if (!isEnabled()) return;

    for (auto a: attachedObjects) {
        if (a->isEnabled()) a->postUpdate();
    }

    if (Config::get()->RENDER_OBJECTS_AXIS) {
        Drawable::drawObject3DAxis(this,true,true,true);
    }
}

void Object3D::DrawPropertiesGUI()
{
    Object3DGUI::DrawPropertiesGUI(this);
}

void Object3D::AddToPosition(const Vertex3D &v)
{
    this->position = this->position + v;
}

void Object3D::setEnableLights(bool value)
{
    enableLights = value;
}

void Object3D::LookAt(Object3D *o)
{
    Vertex3D direction = (o->getPosition() - position).getInverse().getNormalize();

    Vertex3D rightVector = Vertex3D(0, 0, 1) % (direction).getNormalize();
    Vertex3D correctedUpVector = direction % (rightVector).getNormalize();

    setRotation(M3::getFromVectors(direction, correctedUpVector));
}

void Object3D::AttachScript(ScriptLUA *script)
{
    scripts.push_back(script);
    ReloadScriptsEnvironment();
}

void Object3D::ReloadScriptsEnvironment()
{
    for (auto &script : scripts) {
        script->ReloadEnvironment(luaEnvironment);
    }
}

void Object3D::ReloadScriptsCode() const
{
    for (auto &script : scripts) {
        script->ReloadScriptCode();
    }
}

void Object3D::RemoveScript(const ScriptLUA *script)
{
    LOG_MESSAGE("Removing object script %s", script->scriptFilename.c_str());

    for (auto it = scripts.begin(); it != scripts.end(); ++it) {
        if ((*it)->scriptFilename == script->scriptFilename) {
            delete *it;
            scripts.erase(it);
            return;
        }
    }
}

void Object3D::RunStartScripts()
{
    for (auto &script : scripts) {
        script->RunEnvironment(luaEnvironment, "onStart");
    }
}

void Object3D::setBelongToScene(bool belongToScene)
{
    Object3D::belongToScene = belongToScene;
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

void Object3D::MakeKineticBody(float x, float y, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    LOG_MESSAGE("[Object3D] makeKineticBody for %s", getName().c_str());

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
    kinematicBody->setUserIndex(Config::CollisionSource::OBJECT_COLLIDER);
    kinematicBody->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

    characterController = new btKinematicCharacterController(
        kinematicBody,
        dynamic_cast<btConvexShape *>(kinematicBody->getCollisionShape()),
        0.1f
    );

    characterController->setGravity(Config::get()->gravity.toBullet());
    characterController->setUseGhostSweepTest(false);
    characterController->setMaxSlope(btRadians(45)); // Max slope

    world->addCollisionObject(
            kinematicBody,
        Config::collisionGroups::AllFilter,
        Config::collisionGroups::AllFilter
    );

    world->addAction(characterController);
}

void Object3D::MakeSimpleRigidBody(float mass, btDiscreteDynamicsWorld *world, int collisionGroup, int collisionMask)
{
    LOG_MESSAGE("[Object3D] makeSimpleRigidBody for %s", getName().c_str());

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

    if (getCollisionShape() == CAPSULE_SHAPE) {
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
    body->setUserIndex(Config::CollisionSource::OBJECT_COLLIDER);
    body->setRestitution(restitution);
    body->setAngularFactor(angularFactor.toBullet());
    body->setLinearFactor(linearFactor.toBullet());
    body->setFriction(friction);
    body->setDamping(linearDamping, angularDamping);
    body->setCcdMotionThreshold(ccdMotionThreshold);
    body->setCcdSweptSphereRadius(ccdSweptSphereRadius);

    world->addRigidBody(body, collisionGroup, collisionMask);
}

void Object3D::Integrate()
{
    if (!isCollisionsEnabled()) return;

    if (getCollisionMode() == GHOST) {
        getGhostObject()->setWorldTransform(
            Tools::GLMMatrixToBulletTransform(getModelMatrix())
        );
    }

    if (getCollisionMode() == BODY) {
        UpdateFromBullet();
    }

    if (getCollisionMode() == KINEMATIC) {
        btTransform t = kinematicBody->getWorldTransform();
        setPosition(Vertex3D::fromBullet(t.getOrigin()));
    }
}

void Object3D::UpdateFromBullet()
{
    if (body == nullptr || mass <= 0) {
        return;
    }

    btTransform t;
    body->getMotionState()->getWorldTransform(t);

    setPosition(Vertex3D::fromBullet(t.getOrigin()));

    btMatrix3x3 matrixRotation;
    matrixRotation.setRotation(t.getRotation());

    setRotation(M3::fromMat3Bullet(matrixRotation));
}

void Object3D::ResolveCollision(CollisionInfo with)
{
    if (Config::get()->LOG_COLLISION_OBJECTS) {
        auto *object = static_cast<Object3D *>(with.with);
        LOG_MESSAGE("Object3D: Collision %s with %s",  getName().c_str(), object->getName().c_str());
    }

    if (Components::get()->Scripting()->isExecuting()) {
        RunResolveCollisionScripts(with);
    }
}

void Object3D::RunResolveCollisionScripts(CollisionInfo with)
{
    //auto *object = static_cast<Object3D *>(with.with);
    const sol::state &lua = Components::get()->Scripting()->getLua();

    sol::object luaValue = sol::make_object(lua, with);

    for (auto &script : scripts) {
        script->RunEnvironment(luaEnvironment, "onCollision", luaValue);
    }
}

void Object3D::SetupGhostCollider(CollisionShape mode)
{
    LOG_MESSAGE("[Collider] setupGhostCollider");
    RemoveCollisionObject();

    setCollisionMode(GHOST);
    setCollisionShape(mode);

    if (getCollisionShape() == SIMPLE_SHAPE || getCollisionShape() == CAPSULE_SHAPE) {
        makeSimpleGhostBody(
            getPosition(),
            getModelMatrix(),
            simpleShapeSize,
            Brakeza::get()->getComponentsManager()->Collisions()->getDynamicsWorld(),
            Config::collisionGroups::AllFilter,
            Config::collisionGroups::AllFilter
        );
    }
}

bool Object3D::isGUISelected() const
{
    return Components::get()->Render()->getSelectedObject() == this;
}

void Object3D::setMultiScene(bool value)
{
    multiScene = value;
}

void Object3D::AttachObject(Object3D* o)
{
    attachedObjects.push_back(o);
}

void Object3D::setAlpha(float alpha)
{
    Object3D::alpha = alpha;
}

void Object3D::setAlphaEnabled(bool value)
{
    alphaEnabled = value;
}

void Object3D::setScale(float value)
{
    scale = value;
}

void Object3D::setRemoved(bool value)
{
    removed = value;
}

void Object3D::setDrawOffset(const Vertex3D &value)
{
    drawOffset = value;
}

void Object3D::setParent(Object3D *object)
{
    parent = object;
}

void Object3D::setName(const std::string& value)
{
    name = value;
}

void Object3D::setEnabled(bool value)
{
    enabled = value;
}

void Object3D::setPosition(const Vertex3D &p)
{
    position = p;
}

void Object3D::setRotation(const M3 &r)
{
    this->rotation = r;
}

Object3D::~Object3D()
{
    for (auto a: attachedObjects) {
        delete a;
    }

    if (isCollisionsEnabled()) {
        RemoveCollisionObject();
    }
}