

#include "../../include/Objects/Swarm.h"
#include "../../include/Render/Drawable.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

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
            swarmObject->velocity = swarmObject->velocity + direction.getScaled(weight * 600);
        }
    }
}

void Swarm::alignment(SwarmObject* object, std::vector<SwarmObject*> &objects, float weight) {
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
        avgDirection = avgDirection.divide(count);
        avgDirection = avgDirection.getNormalize();
        object->velocity = object->velocity + avgDirection.getScaled(weight);
    }
}

void Swarm::cohesion(SwarmObject* swarmObject, std::vector<SwarmObject*> &objects, float weight)
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
        center = center.divide(count);
        center = center - swarmObject->object->getPosition();
        center = center.getNormalize();
        swarmObject->velocity = swarmObject->velocity + center.getScaled(weight);
    }
}

void Swarm::updateBoids()
{
    for (auto o: objects) {
        separation(o, objects, separationWeight * globalBiasSeparation);
        alignment(o, objects, alignmentWeight * globalBiasAligniment);
        cohesion(o, objects, cohesionWeight * globalBiasCohesion);

        attractTo(o, getPosition(), true, centerAttractionWeight * globalBiasSeparation);
        checkBoundsAndAdjustVelocity(o, 1.2);
        avoidPredators(o);

        limitVelocity(o);

        updatePosition(o, velocityBoidsFactor);
    }
}

void Swarm::addPredator(SwarmObject *o)
{
    o->velocity = Tools::randomVertex().getNormalize();
    predators.emplace_back(o);
}

SwarmObject* Swarm::createBoid(Object3D *mesh)
{
    mesh->setScale(0.50);
    auto o = new SwarmObject();

    o->object = mesh;
    o->velocity = Tools::randomVertex().getNormalize();
    o->velocity.z = 0;

    objects.emplace_back(o);

    return o;
}

void Swarm::limitVelocity(SwarmObject* object) const {

    // Calcula la magnitud actual de la velocidad
    float currentSpeed = object->velocity.getModule();

    // Si la velocidad actual es mayor que la velocidad máxima, reescala la velocidad
    if (currentSpeed > maxSpeed) {
        object->velocity = object->velocity.getNormalize().getScaled(maxSpeed);
    }

    object->velocity.z = 0;
}

Vertex3D Swarm::randomVertexInsideAABB()
{
    float x = Tools::random(minBounds.x, maxBounds.x);
    float y = Tools::random(minBounds.y, maxBounds.y);
    float z = getPosition().z; //Tools::random(minBounds.z, maxBounds.z);
    return Vertex3D(x, y, z);
}

void Swarm::attractTo(SwarmObject* swarmObject, Vertex3D center, bool ignoreThresold, float weight)
{
    Vertex3D directionFromCenter = swarmObject->object->getPosition() - center;

    // Calcula la distancia al centro.
    float distanceFromCenter = directionFromCenter.getModule();

    // Si la distancia al centro es mayor que un cierto umbral, aplica una fuerza que empuja a la partícula de vuelta hacia el centro.
    if (distanceFromCenter > centerThreshold || ignoreThresold) {
        // La fuerza es proporcional al cuadrado de la distancia al centro.
        Vertex3D centerForce = directionFromCenter.getNormalize().getScaled(-weight);

        // Aplica la fuerza al vector de velocidad de la partícula.
        swarmObject->velocity = swarmObject->velocity + centerForce;
    }
}

void Swarm::avoidPredators(SwarmObject* swarmObject) {
    for(auto& predator : predators) {
        Vertex3D directionFromPredator = swarmObject->object->getPosition() -  predator->object->getPosition();

        // Calcula la distancia al depredador.
        float distanceFromPredator = directionFromPredator.getModule();

        // Si la distancia al depredador es menor que un cierto umbral, aplica una fuerza que empuja a la partícula lejos del depredador.
        if (distanceFromPredator < predatorThreshold) {
            float forceMagnitude = predatorAvoidanceWeight * (1 - distanceFromPredator / predatorThreshold);
            Vertex3D predatorForce = directionFromPredator.getNormalize().getScaled(forceMagnitude);

            // Aplica la fuerza al vector de velocidad de la partícula.
            swarmObject->velocity = swarmObject->velocity + predatorForce;
        }
    }
}

void Swarm::updatePosition(SwarmObject *o, float weight)
{
    o->object->setPosition(o->object->getPosition() + (o->velocity.getScaled(weight * Brakeza3D::get()->getDeltaTime())).getScaled(1, 1, 0));
}

void Swarm::checkBoundsAndAdjustVelocity(SwarmObject* swarmObject, float turnFactor) {
    // Obtén la posición del objeto
    Vertex3D position = swarmObject->object->getPosition();

    // Si el objeto ha excedido los límites del AABB, ajusta su velocidad
    if (position.x < minBounds.x) {
        swarmObject->velocity.x = swarmObject->velocity.x + turnFactor;
    }
    if (position.x > maxBounds.x) {
        swarmObject->velocity.x = swarmObject->velocity.x - turnFactor;
    }
    if (position.y < minBounds.y) {
        swarmObject->velocity.y = swarmObject->velocity.y + turnFactor;
    }
    if (position.y > maxBounds.y) {
        swarmObject->velocity.y = swarmObject->velocity.y - turnFactor;
    }
    if (position.z < minBounds.z) {
        swarmObject->velocity.z = swarmObject->velocity.z + turnFactor;
    }
    if (position.z > maxBounds.z) {
        swarmObject->velocity.z = swarmObject->velocity.z - turnFactor;
    }
}

void Swarm::onDrawHostBuffer()
{
    Object3D::onDrawHostBuffer();

    if (EngineSetup::get()->DRAW_MESH3D_AABB) {
        Drawable::drawAABB(&this->aabb, Color::yellow());
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
