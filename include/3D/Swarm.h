//
// Created by eduardo on 13/05/23.
//

#ifndef BRAKEZA3D_SWARM_H
#define BRAKEZA3D_SWARM_H


#include <vector>
#include "Object3D.h"
#include "Cube3D.h"

class SwarmObject
{
public:
    SwarmObject();

    explicit SwarmObject(Object3D *object) : object(object) {}
    Object3D *object;
    Vertex3D velocity;
};

class Swarm : public Object3D {
    AABB3D aabb;
    std::vector<SwarmObject*> objects;
    std::vector<SwarmObject*> predators;

    bool debug = false;

    Vertex3D minBounds;
    Vertex3D maxBounds;

    float separationWeight = 0.9f;
    float alignmentWeight = 0.8;
    float cohesionWeight = 0.6f;

    float neighborDist = 0.3;

    float maxSpeed = 0.1f;
    float centerThreshold = 1.0;

    float velocityBoidsFactor = 2.0;

    float predatorThreshold = 3.0f;
    float predatorAvoidanceWeight = 30.0;

    float globalBiasSeparation = 1;
    float globalBiasAlignment = 1;
    float globalBiasCohesion = 1;
    float centerAttractionWeight = 1.0;

    float turnFactor = 0.1f;

    Vertex3D size;
public:
    Swarm(Vertex3D position, Vertex3D size);

    void updateBoids();
    void separation(SwarmObject *swarmObject, std::vector<SwarmObject*> &objects, float weight) const;
    void alignment(SwarmObject *object, std::vector<SwarmObject*> &objects, float weight) const;
    void cohesion(SwarmObject *swarmObject, std::vector<SwarmObject*> &objects, float weight) const;
    void limitVelocity(SwarmObject *object) const;
    void updateBounds();
    void onUpdate() override;
    SwarmObject* createBoid(Object3D *object);
private:
    Vertex3D randomVertexInsideAABB();
public:
    TypeObject getTypeObject() const override;
    GUIType::Sheet getIcon() override;
    void attractTo(SwarmObject *swarmObject, Vertex3D center, bool ignoreThresold, float weight) const;
    void avoidPredators(SwarmObject *swarmObject);
    void checkBoundsAndAdjustVelocity(SwarmObject* swarmObject);
    void addPredator(SwarmObject *o);
    void reset();
    void removeBoid(SwarmObject *o);
    void DrawPropertiesGUI() override;
    static void updatePosition(SwarmObject *o, float weight);

    friend class SwarmGUI;
};


#endif //BRAKEZA3D_SWARM_H
