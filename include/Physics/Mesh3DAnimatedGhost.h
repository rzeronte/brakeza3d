#ifndef BRAKEZA3D_MESH3DANIMATEDGHOST_H
#define BRAKEZA3D_MESH3DANIMATEDGHOST_H


#include "../Objects/Mesh3DAnimated.h"
#include "Ghost.h"

class Mesh3DAnimatedGhost : public Mesh3DAnimated, public Ghost {
public:
    Mesh3DAnimatedGhost();
private:

    void updateBulletFromMesh3D();

protected:
    void integrate() override;

    virtual void remove();

    void onUpdate() override;

public:
    void resolveCollision(Collisionable *collisionable) override;
};


#endif //BRAKEZA3D_MESH3DANIMATEDGHOST_H
