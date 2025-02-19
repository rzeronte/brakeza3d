
#include "../../include/Objects/Swarm.h"
#include "../../include/Render/Drawable.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

SwarmObject::SwarmObject()
:
    object(nullptr)
{
}

Swarm::Swarm(Vertex3D position, Vertex3D size) : size(size)
{
    setPosition(position);
    updateBounds();
}

void Swarm::updateBounds()
{
    const auto newSize = size.getScaled(getScale());
    Vertex3D halfSize = newSize.getScaled(0.5);

    minBounds = getPosition() - halfSize;
    maxBounds = getPosition() + halfSize;

    aabb.max = maxBounds;
    aabb.min = minBounds;

    aabb.updateVertices();
}

void Swarm::onUpdate()
{
    Object3D::onUpdate();

    if (debug) {
        Drawable::drawAABB(&this->aabb, Color::yellow());
    }

    updateBounds();
    updateBoids();
}

void Swarm::separation(SwarmObject* swarmObject, std::vector<SwarmObject*> &objects, float weight) const
{
    Vertex3D direction;
    for (auto o: objects) {
        float distance = o->object->getPosition().distance(swarmObject->object->getPosition());

        if (o != swarmObject && distance < neighborDist) {
            direction = swarmObject->object->getPosition() - o->object->getPosition();
            direction = direction.getNormalize();
            direction = direction.divide(distance);
            swarmObject->velocity = swarmObject->velocity + direction.getScaled(weight);
        }
    }
}

void Swarm::alignment(SwarmObject* object, std::vector<SwarmObject*> &objects, float weight) const
{
    Vertex3D avgDirection;
    int count = 0;
    for (auto o: objects) {
        float distance = o->object->getPosition().distance(object->object->getPosition());
        if (o != object && distance < neighborDist) {
            avgDirection = avgDirection + o->velocity;
            count++;
        }
    }
    if (count > 0) {
        avgDirection = avgDirection.divide((float) count);
        avgDirection = avgDirection.getNormalize();
        object->velocity = object->velocity + avgDirection.getScaled(weight);
    }
}

void Swarm::cohesion(SwarmObject* swarmObject, std::vector<SwarmObject*> &objects, float weight) const
{
    Vertex3D center;
    int count = 0;
    for (auto o: objects) {
        float distance = o->object->getPosition().distance(swarmObject->object->getPosition());
        if (o != swarmObject && distance < neighborDist) {
            center = center + o->object->getPosition();
            count++;
        }
    }
    if (count > 0) {
        center = center.divide((float) count);
        center = center - swarmObject->object->getPosition();
        center = center.getNormalize();
        swarmObject->velocity = swarmObject->velocity + center.getScaled(weight);
    }
}

void Swarm::updateBoids()
{
    for (auto o: objects) {
        separation(o, objects, separationWeight * globalBiasSeparation);
        alignment(o, objects, alignmentWeight * globalBiasAlignment);
        cohesion(o, objects, cohesionWeight * globalBiasCohesion);

        attractTo(o, getPosition(), true, centerAttractionWeight * globalBiasSeparation);

        checkBoundsAndAdjustVelocity(o);
        avoidPredators(o);

        limitVelocity(o);

        updatePosition(o, velocityBoidsFactor);
        o->object->lookAt(this);
    }
}

void Swarm::addPredator(SwarmObject *o)
{
    o->velocity = Vertex3D::randomVertex().getNormalize();
    predators.emplace_back(o);
}

SwarmObject* Swarm::createBoid(Object3D *mesh)
{
    mesh->setScale(0.50);
    auto o = new SwarmObject();

    o->object = mesh;
    o->velocity = Vertex3D::randomVertex().getNormalize();
    o->velocity.z = 0;

    objects.emplace_back(o);

    return o;
}

void Swarm::limitVelocity(SwarmObject* object) const
{
    float currentSpeed = object->velocity.getModule();

    if (currentSpeed > maxSpeed) {
        object->velocity = object->velocity.getNormalize().getScaled(maxSpeed);
    }

    //object->velocity.z = 0;
}

Vertex3D Swarm::randomVertexInsideAABB()
{
    auto x = (float) Tools::random((int) minBounds.x, (int) maxBounds.x);
    auto y = (float) Tools::random((int) minBounds.y, (int) maxBounds.y);
    float z = getPosition().z; //Tools::random(minBounds.z, maxBounds.z);

    return {x, y, z};
}

void Swarm::attractTo(SwarmObject* swarmObject, Vertex3D center, bool ignoreThresold, float weight) const
{
    Vertex3D directionFromCenter = swarmObject->object->getPosition() - center;

    float distanceFromCenter = directionFromCenter.getModule();

    if (distanceFromCenter > centerThreshold || ignoreThresold) {
        Vertex3D centerForce = directionFromCenter.getNormalize().getScaled(-weight);

        swarmObject->velocity = swarmObject->velocity + centerForce;
    }
}

void Swarm::avoidPredators(SwarmObject* swarmObject) {
    for(auto& predator : predators) {
        Vertex3D directionFromPredator = swarmObject->object->getPosition() -  predator->object->getPosition();

        float distanceFromPredator = directionFromPredator.getModule();

        if (distanceFromPredator < predatorThreshold) {
            float forceMagnitude = predatorAvoidanceWeight * (1 - distanceFromPredator / predatorThreshold);
            Vertex3D predatorForce = directionFromPredator.getNormalize().getScaled(forceMagnitude);

            swarmObject->velocity = swarmObject->velocity + predatorForce;
        }
    }
}

void Swarm::updatePosition(SwarmObject *o, float weight)
{
    o->object->setPosition(o->object->getPosition() + (o->velocity.getScaled(weight * Brakeza3D::get()->getDeltaTime())).getScaled(1, 1, 1));
}

void Swarm::checkBoundsAndAdjustVelocity(SwarmObject* swarmObject) {
    // Obtén la posición del objeto
    Vertex3D position = swarmObject->object->getPosition();

    // Si el objeto ha excedido los límites del AABB, aplica fuerza inversa (fricción)
    if (position.x < minBounds.x || position.x > maxBounds.x ||
        position.y < minBounds.y || position.y > maxBounds.y ||
        position.z < minBounds.z || position.z > maxBounds.z) {

        attractTo(swarmObject, getPosition(), false, centerAttractionWeight);
    }
}

void Swarm::reset()
{
    for (auto o: objects) {
        delete o;
    }

    for (auto o: predators) {
        delete o;
    }

    objects.clear();
    predators.clear();
}

void Swarm::removeBoid(SwarmObject *o)
{
    objects.erase(std::remove(objects.begin(), objects.end(), o), objects.end());
}

const char *Swarm::getTypeObject() {
    return "Swarm";
}

const char *Swarm::getTypeIcon() {
    return "swarmIcon";
}

void Swarm::drawImGuiProperties()
{
    Object3D::drawImGuiProperties();

    if (ImGui::TreeNode("Swarm Settings")) {
        const float range_min = -1000;
        const float range_max = 100;
        const float range_sensibility = 0.1;

        ImGui::Checkbox("Debug", &debug);

        if (ImGui::TreeNode("Size")) {
            ImGui::DragScalar("X", ImGuiDataType_Float, &size.x, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Y", ImGuiDataType_Float, &size.y, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Z", ImGuiDataType_Float, &size.z, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Weights")) {
            ImGui::DragScalar("separation", ImGuiDataType_Float, &separationWeight, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("alignment", ImGuiDataType_Float, &alignmentWeight, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("cohesion", ImGuiDataType_Float, &cohesionWeight, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Global Bias")) {
            ImGui::DragScalar("Bias Separation", ImGuiDataType_Float, &globalBiasSeparation, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Bias Alignment", ImGuiDataType_Float, &globalBiasAlignment, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Bias Cohesion", ImGuiDataType_Float, &globalBiasCohesion, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Predator")) {
            ImGui::DragScalar("Threshold", ImGuiDataType_Float, &predatorThreshold, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Avoidance Weight", ImGuiDataType_Float, &predatorAvoidanceWeight, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Center")) {
            ImGui::DragScalar("Center threshold", ImGuiDataType_Float, &centerThreshold, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::DragScalar("Center attraction", ImGuiDataType_Float, &centerAttractionWeight, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
            ImGui::TreePop();
        }

        ImGui::DragScalar("Max. Speed", ImGuiDataType_Float, &maxSpeed, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
        ImGui::DragScalar("Neighbor dist.", ImGuiDataType_Float, &neighborDist, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
        ImGui::DragScalar("Velocity Boids factor", ImGuiDataType_Float, &velocityBoidsFactor, range_sensibility ,&range_min, &range_max, "%f", 1.0f);
        ImGui::DragScalar("Turn factor", ImGuiDataType_Float, &turnFactor, range_sensibility ,&range_min, &range_max, "%f", 1.0f);

        ImGui::TreePop();
    }
}