
#include <algorithm>
#include "../../include/3D/Swarm.h"
#include "../../include/Render/Drawable.h"
#include "../../include/Brakeza.h"
#include "../../include/GUI/Inspectors/SwarmGUI.h"
#include "../../include/Misc/Logging.h"

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

    // Las mallas de la escena cargan de forma asíncrona y ThreadJobResolveSwarms solo corre una vez:
    // si algún boid o depredador terminaba de cargar después, se quedaba fuera para siempre (y sin
    // depredador los boids no huyen). Se reintenta hasta tenerlos todos.
    if (hasPendingMembers()) {
        resolveRetryTimer += Brakeza::get()->getDeltaTime();
        if (resolveRetryTimer >= 0.25f) {
            resolveRetryTimer = 0.0f;
            const size_t before = pendingBoidNames.size() + pendingPredatorNames.size();
            resolvePendingMembers(true);
            const size_t after = pendingBoidNames.size() + pendingPredatorNames.size();
            if (after == 0) {
                LOG_MESSAGE("[Swarm] '%s': all members resolved (%d boids, %d predators)",
                    getName().c_str(), (int) objects.size(), (int) predators.size());
            } else if (after != before) {
                LOG_MESSAGE("[Swarm] '%s': %d member(s) still pending", getName().c_str(), (int) after);
            }
        }
    }

    updateBoids();
}

void Swarm::resolvePendingMembers(bool quiet)
{
    auto resolve = [&](std::vector<std::string>& pending, bool isPredator) {
        std::vector<std::string> unresolved;
        for (const auto& name : pending) {
            Object3D *obj = Brakeza::get()->getObjectByName(name);
            if (obj != nullptr && obj != this && !obj->isRemoved()) {
                if (isPredator) {
                    auto* sw = new SwarmObject(obj);
                    sw->velocity = Vertex3D::randomVertex().getNormalize();
                    addPredator(sw);
                } else {
                    createBoid(obj);
                }
            } else {
                unresolved.push_back(name);
            }
        }
        pending = unresolved;
        if (!quiet && !unresolved.empty()) {
            LOG_MESSAGE("[Swarm] resolvePendingMembers: %d %s not found yet",
                (int)unresolved.size(), isPredator ? "predators" : "boids");
        }
    };

    resolve(pendingBoidNames, false);
    resolve(pendingPredatorNames, true);
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
    // Miembros cuyo objeto se ha eliminado (borrado desde el editor o un script): fuera de la lista
    // antes de tocarlos, o se desreferencia un objeto muerto
    auto dropRemoved = [](std::vector<SwarmObject*> &list) {
        for (auto it = list.begin(); it != list.end();) {
            if ((*it)->object == nullptr || (*it)->object->isRemoved()) {
                delete *it;
                it = list.erase(it);
            } else {
                ++it;
            }
        }
    };
    dropRemoved(objects);
    dropRemoved(predators);

    for (auto o: objects) {
        separation(o, objects, separationWeight * globalBiasSeparation);
        alignment(o, objects, alignmentWeight * globalBiasAlignment);
        cohesion(o, objects, cohesionWeight * globalBiasCohesion);

        attractTo(o, getPosition(), true, centerAttractionWeight * globalBiasSeparation);

        checkBoundsAndAdjustVelocity(o);
        const bool fleeing = avoidPredators(o);

        // Huyendo, el boid puede ir más rápido que su paso normal (pánico); si no, la huida
        // quedaba recortada al mismo tope que el paseo y el depredador alcanzaba a la bandada
        limitVelocity(o, fleeing ? std::max(1.0f, predatorPanicSpeedFactor) : 1.0f);

        updatePosition(o, velocityBoidsFactor);

        // Mirar hacia donde se mueve (antes miraba al centro del enjambre)
        if (o->velocity.getModule() > 0.0001f) {
            o->object->LookAt(o->object->getPosition() + o->velocity);
        }
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

void Swarm::limitVelocity(SwarmObject* object, float speedFactor) const
{
    float currentSpeed = object->velocity.getModule();
    const float limit = maxSpeed * speedFactor;

    if (currentSpeed > limit) {
        object->velocity = object->velocity.getNormalize().getScaled(limit);
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

bool Swarm::avoidPredators(SwarmObject* swarmObject) {
    bool fleeing = false;
    for(auto& predator : predators) {
        Vertex3D directionFromPredator = swarmObject->object->getPosition() -  predator->object->getPosition();

        float distanceFromPredator = directionFromPredator.getModule();

        if (distanceFromPredator < predatorThreshold) {
            float forceMagnitude = predatorAvoidanceWeight * (1 - distanceFromPredator / predatorThreshold);
            Vertex3D predatorForce = directionFromPredator.getNormalize().getScaled(forceMagnitude);

            swarmObject->velocity = swarmObject->velocity + predatorForce;
            fleeing = true;
        }
    }
    return fleeing;
}

void Swarm::updatePosition(SwarmObject *o, float weight)
{
    o->object->setPosition(o->object->getPosition() + (o->velocity.getScaled(weight * Brakeza::get()->getDeltaTime())).getScaled(1, 1, 1));
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

void Swarm::removePredator(SwarmObject *o)
{
    predators.erase(std::remove(predators.begin(), predators.end(), o), predators.end());
}

ObjectType Swarm::getTypeObject() const
{
    return ObjectType::Swarm;
}

GUIType::Sheet Swarm::getIcon()
{
    return IconObject::SWARM;
}

void Swarm::DrawPropertiesGUI()
{
    Object3D::DrawPropertiesGUI();
    SwarmGUI::DrawPropertiesGUI(this);
}