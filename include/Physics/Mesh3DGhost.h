#ifndef BRAKEDA3D_MESH3DGHOST_H
#define BRAKEDA3D_MESH3DGHOST_H


#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include "../Objects/Vertex3D.h"
#include "../Objects/Mesh3D.h"
#include "Ghost.h"

class Mesh3DGhost : public Mesh3D, public Ghost {
public:
    Mesh3DGhost();

protected:
    void integrate() override;

public:
    void resolveCollision(Collisionable *collisionable) override;

    virtual void remove();

    ~Mesh3DGhost() override;

    void magnetizableTo(Object3D *object);

    void drawImGuiProperties() override;

    void runResolveCollisionScripts(Collisionable *with);

    const char *getTypeIcon() override;

    const char *getTypeObject() override;

    cJSON *getJSON() override;

    static void createFromJSON(cJSON *object);

    static void setPropertiesFromJSON(cJSON *object, Mesh3DGhost *o);
};

#endif //BRAKEDA3D_MESH3DGHOST_H
