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
    Object3D *object;
    Vertex3D velocity;
    bool avoidMove;
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

    void add(SwarmObject *o);
    void createPredator(std::string filename);

    void separation(SwarmObject *swarmObject, std::vector<SwarmObject*> &objects, float weight) const;

    void alignment(SwarmObject *object, std::vector<SwarmObject*> &objects, float weight);

    void cohesion(SwarmObject *swarmObject, std::vector<SwarmObject*> &objects, float weight);

    void limitVelocity(SwarmObject *object) const;

    void updateBounds();

    void onUpdate() override;

    float separationWeight = 0.5f;
    float alignmentWeight = 0.f;
    float cohesionWeight = 0.5f;
    float neighborDist = 2500;

    float maxSpeed = 50.0f;
    float centerThreshold = 1.0;

    float velocityBoidsFactor = 1.0;
    float velocityPredatorsFactor = 1.0;

    float predatorThreshold = 1.0f;
    float predatorAvoidanceWeight = 1.0;
    float predatorAttractionWeight = 1.0f;
    float predatorSeparationWeight = 0.5;
    float predatorCohesionWeight = 0.5;
    float predatorAlignmentWeight = 0.5f;

    void createBoid(const std::string& filename);

    float scaleObject = 0.1f;

    float globalBiasSeparation = 1;
    float globalBiasAligniment = 1;
    float globalBiasCohesion = 1;

    Vertex3D rotationObject;
    Vertex3D size;

    void addFromObject(Object3D *o);

private:
    void postUpdate() override;

    void onDraw() override;

    Vertex3D randomVertexInsideAABB();

    Vertex3D calculateCenter();

    Vertex3D avoidPosition;

public:
    float centerAttractionWeight = 1.0;
    void attractTo(SwarmObject *swarmObject, Vertex3D center, bool ignoreThresold, float weight);

    void avoidPredators(SwarmObject *swarmObject);

    void updatePredators();

    void updatePosition(SwarmObject *o, float weight);

    void checkBoundsAndAdjustVelocity(SwarmObject *swarmObject, float turnFactor);
};


#endif //BRAKEZA3D_SWARM_H
