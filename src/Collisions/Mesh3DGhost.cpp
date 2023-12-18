#include "../../include/Physics/Mesh3DGhost.h"
#include "../../include/Render/Logging.h"
#include "../../include/Physics/Projectile3DBody.h"
#include "../../include/Brakeza3D.h"

Mesh3DGhost::Mesh3DGhost() {
}

void Mesh3DGhost::integrate()
{
    updateBulletFromMesh3D();
}

void Mesh3DGhost::updateBulletFromMesh3D()
{
    btTransform trans;
    trans.setIdentity();
    trans.setOrigin(btVector3(getPosition().x, getPosition().y, getPosition().z));

    btMatrix3x3 matrixRotation = Tools::M3ToBulletM3(getRotation());
    btQuaternion rotation;
    matrixRotation.getRotation(rotation);

    trans.setRotation(rotation);
    getGhostObject()->setWorldTransform(trans);
}

void Mesh3DGhost::resolveCollision(Collisionable *with)
{
    if (EngineSetup::get()->LOG_COLLISION_OBJECTS) {
        auto *object = dynamic_cast<Object3D*> (with);
        Logging::Log("Mesh3DGhost: Collision %s with %s",  getLabel().c_str(), object->getLabel().c_str());
    }

    if (ComponentsManager::get()->getComponentRender()->getStateLUAScripts() == EngineSetup::LUA_PLAY) {
        runResolveCollisionScripts(with);
    }
}

void Mesh3DGhost::remove()
{
    this->setRemoved(true);
}

Mesh3DGhost::~Mesh3DGhost()
{
    Logging::Log("Delete Mesh3DGhost");
}

void Mesh3DGhost::magnetizableTo(Object3D *object)
{
    Vertex3D to = object->getPosition();
    Vector3D direction(getPosition(), to);

    const float distance = getPosition().distance(to);
    const float speed = powf(5000 / distance, 3) * 0.00075f;

    if (distance < 100) {
        setPosition(to);
    } else {
        setPosition(getPosition() + direction.getComponent().getScaled(speed));
    }
}

void Mesh3DGhost::drawImGuiProperties()
{
    Mesh3D::drawImGuiProperties();

    ImGui::Separator();
    if (ImGui::TreeNode("Ghost Body")) {
        auto flags = ImGuiComboFlags_None;
        const char* items[] = { "Simple ghost", "Triangle Mesh ghost" };
        int item_current_idx = getTypeShape();
        const char* combo_preview_value = items[item_current_idx];
        auto titleCombo = "Collision shape###" + getLabel();
        if (ImGui::BeginCombo(titleCombo.c_str(), combo_preview_value, flags)) {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                const bool is_selected = (item_current_idx == n);
                if (ImGui::Selectable(items[n], is_selected)) {
                    if (!is_selected) {
                        item_current_idx = n;
                        switch (n) {
                            case 0: {
                                Logging::Message("Making object %s physics with: Box shape", getLabel().c_str());
                                removeCollisionObject();
                                updateBoundingBox();
                                this->makeSimpleGhostBody(
                                        getAabb().size(),
                                        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                                        btBroadphaseProxy::DefaultFilter,
                                        btBroadphaseProxy::DefaultFilter
                                );

                                break;
                            }
                            case 1: {
                                Logging::Message("Making object %s physics with: Mesh3D shape", getLabel().c_str());
                                removeCollisionObject();
                                this->makeGhostBody(
                                        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                                        this,
                                        btBroadphaseProxy::DefaultFilter,
                                        btBroadphaseProxy::DefaultFilter
                                );
                                break;
                            }
                        }
                    }
                }
            }
            ImGui::EndCombo();
        }


        ImGui::TreePop();
    }
}

void Mesh3DGhost::runResolveCollisionScripts(Collisionable *with)
{
    for (auto script : scripts) {
        script->runEnvironment(luaEnvironment, "onCollision");
    }
}

const char *Mesh3DGhost::getTypeObject() {
    return "Mesh3DGhost";
}

const char *Mesh3DGhost::getTypeIcon() {
    return "ghostIcon";
}

cJSON * Mesh3DGhost::getJSON()
{
    cJSON *root = Mesh3D::getJSON();

    cJSON_AddNumberToObject(root, "typeShape", typeShape);

    return root;
}

void Mesh3DGhost::setPropertiesFromJSON(cJSON *object, Mesh3DGhost *o)
{
    o->setBelongToScene(true);
    Mesh3D::setPropertiesFromJSON(object, o);
    auto type = cJSON_GetObjectItemCaseSensitive(object, "typeShape")->valueint;
    switch (type) {
        case GhostTypeShape::GHOST_SIMPLE_SHAPE: {
            o->setTypeShape(GhostTypeShape::GHOST_SIMPLE_SHAPE);
            o->updateBoundingBox();
            o->makeSimpleGhostBody(
                o->getAabb().size(),
                Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                btBroadphaseProxy::DefaultFilter,
                btBroadphaseProxy::DefaultFilter
            );
            break;
        }
        case GhostTypeShape::GHOST_TRIANGLE3D_MESH_SHAPE: {
            o->setTypeShape(GhostTypeShape::GHOST_TRIANGLE3D_MESH_SHAPE);
            o->makeGhostBody(
                Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
                o,
                btBroadphaseProxy::DefaultFilter,
                btBroadphaseProxy::DefaultFilter
            );
            break;
        }
    }
}

void Mesh3DGhost::createFromJSON(cJSON *object)
{
    auto o = new Mesh3DGhost();

    Mesh3DGhost::setPropertiesFromJSON(object, o);

    Brakeza3D::get()->addObject3D(o, cJSON_GetObjectItemCaseSensitive(object, "name")->valuestring);
}