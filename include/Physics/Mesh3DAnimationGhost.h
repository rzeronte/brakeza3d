#ifndef BRAKEZA3D_MESH3DANIMATIONGHOST_H
#define BRAKEZA3D_MESH3DANIMATIONGHOST_H


#include "../Objects/Mesh3DAnimation.h"
#include "Ghost.h"

class Mesh3DAnimationGhost : public Mesh3DAnimation, public Ghost {
public:
    Mesh3DAnimationGhost();
private:

    void updateBulletFromMesh3D();

protected:
    void integrate() override;

    virtual void remove();

    void onUpdate() override;

public:
    void resolveCollision(Collisionable *collisionable) override;
};


#endif //BRAKEZA3D_MESH3DANIMATIONGHOST_H
