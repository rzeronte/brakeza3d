#include "../../include/Physics/Mesh3DGhost.h"

#include <glm/trigonometric.hpp>

#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"

Mesh3DGhost::Mesh3DGhost()
{
}

void Mesh3DGhost::integrate()
{
    getGhostObject()->setWorldTransform(Tools::GLMMatrixToBulletTransform(getModelMatrix()));
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

void Mesh3DGhost::magnetizableTo(Object3D *object)
{
    Vertex3D to = object->getPosition();
    Vector3D direction(getPosition(), to);

    const float distance = getPosition().distance(to);
    const float speed = powf(1 / distance, 3) * 0.25f;

    if (distance < 0.01) {
        setPosition(to);
    } else {
        setPosition(getPosition() + direction.getComponent().getScaled(speed));
    }
}

void Mesh3DGhost::drawImGuiProperties()
{
    Mesh3D::drawImGuiProperties();

    ImGui::Separator();
    if (ImGui::TreeNode("Mesh3DGhost")) {
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
                                makeDefaultSimple();
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

        if (getTypeShape() == GHOST_SIMPLE_SHAPE) {
            if (ImGui::TreeNode("Simple shape size")) {
                const float range_min = -500000;
                const float range_max = 500000;
                const float range_sensibility = 0.1;

                ImGui::DragScalar("X", ImGuiDataType_Float, &simpleShapeSize.x, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
                ImGui::DragScalar("Y", ImGuiDataType_Float, &simpleShapeSize.y, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
                ImGui::DragScalar("Z", ImGuiDataType_Float, &simpleShapeSize.z, range_sensibility ,&range_min, &range_max, "%f", 1.0f);

                if (ImGui::Button(std::string("Update collision shape").c_str())) {
                    removeCollisionObject();
                    makeDefaultSimple();
                }
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

void Mesh3DGhost::runResolveCollisionScripts(Collisionable *with)
{
    auto *object = dynamic_cast<Object3D*> (with);
    const sol::state &lua = LUAManager::get()->getLua();

    sol::object luaValue = sol::make_object(lua, object);

    for (auto script : scripts) {
        script->runEnvironment(luaEnvironment, "onCollision", luaValue);
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
            o->makeDefaultSimple();
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

Mesh3DGhost::~Mesh3DGhost() {

}

Mesh3DGhost *Mesh3DGhost::create(Vertex3D position, const std::string &modelFile)
{
    auto *o = new Mesh3DGhost();
    o->setBelongToScene(true);
    o->setPosition(position);
    o->AssimpLoadGeometryFromFile(modelFile);
    o->makeDefaultSimple();

    return o;
}

void Mesh3DGhost::makeDefaultSimple()
{
    this->makeSimpleGhostBody(
        simpleShapeSize,
        Brakeza3D::get()->getComponentsManager()->getComponentCollisions()->getDynamicsWorld(),
        btBroadphaseProxy::DefaultFilter,
        btBroadphaseProxy::DefaultFilter
    );
}
