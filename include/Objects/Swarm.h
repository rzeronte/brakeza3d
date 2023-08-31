//
// Created by eduardo on 13/05/23.
//

#ifndef BRAKEZA3D_SWARM_H
#define BRAKEZA3D_SWARM_H


#include <vector>
#include "Object3D.h"
#include "Cube3D.h"

struct OCBoid {
    OCBoid(const OCVertex3D &position, const OCVertex3D &velocity, const OCVertex3D &color) : position(position),
                                                                                              velocity(velocity),
                                                                                              color(color) {}

    OCVertex3D position;
    OCVertex3D velocity;
    OCVertex3D color;
};

struct OCSwarmContext {
    OCSwarmContext(float separationWeight, float alignmentWeight, float cohesionWeight, const CameraData &cameraData, const FrustumData &frustumData) : separationWeight(
            separationWeight), alignmentWeight(alignmentWeight), cohesionWeight(cohesionWeight), frustumData(frustumData), cameraData(cameraData) {}

    float separationWeight = 1.0f;
    float alignmentWeight = 1.0f;
    float cohesionWeight = 1.0f;
    float distanceNeighbor = 100;
    float maxVelocity = 1;
    FrustumData frustumData;
    CameraData cameraData;
};

class SwarmObject
{
public:
    SwarmObject() {}

    explicit SwarmObject(Object3D *object) : object(object) {}
    Object3D *object;
    Vertex3D velocity;
};

class Swarm : public Object3D {
    AABB3D aabb;
    std::vector<SwarmObject*> objects;
    std::vector<SwarmObject*> predators;

    Vertex3D minBounds;
    Vertex3D maxBounds;

public:
    Swarm(Vertex3D position, Vertex3D size);

    void updateBoids();

    SwarmObject* createBoid(Object3D *object);

    void separation(SwarmObject *swarmObject, std::vector<SwarmObject*> &objects, float weight) const;

    void alignment(SwarmObject *object, std::vector<SwarmObject*> &objects, float weight);

    void cohesion(SwarmObject *swarmObject, std::vector<SwarmObject*> &objects, float weight);

    void limitVelocity(SwarmObject *object) const;

    void updateBounds();

    void onUpdate() override;

    float separationWeight = 0.9f;
    float alignmentWeight = 0.8;
    float cohesionWeight = 0.6f;
    float neighborDist = 3000;

    float maxSpeed = 100.0f;
    float centerThreshold = 1.0;

    float velocityBoidsFactor = 25.0;
    float velocityPredatorsFactor = 1.0;

    float predatorThreshold = 3000.0f;
    float predatorAvoidanceWeight = 30.0;
    float predatorAttractionWeight = 1.0f;
    float predatorSeparationWeight = 0.5;
    float predatorCohesionWeight = 0.5;
    float predatorAlignmentWeight = 0.5f;

    float scaleObject = 0.1f;

    float globalBiasSeparation = 1;
    float globalBiasAligniment = 1;
    float globalBiasCohesion = 1;

    Vertex3D rotationObject;
    Vertex3D size;

private:

    void onDrawHostBuffer() override;

    Vertex3D randomVertexInsideAABB();

public:
    float centerAttractionWeight = 1.0;
    void attractTo(SwarmObject *swarmObject, Vertex3D center, bool ignoreThresold, float weight);

    void avoidPredators(SwarmObject *swarmObject);

    void updatePosition(SwarmObject *o, float weight);

    void checkBoundsAndAdjustVelocity(SwarmObject *swarmObject, float turnFactor);

    void addPredator(SwarmObject *o);

    void reset();

    void removeBoid(SwarmObject *o);
};


#endif //BRAKEZA3D_SWARM_H
