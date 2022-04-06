//
// Created by eduardo on 3/04/22.
//

#ifndef BRAKEDA3D_GRAVITATIONALGHOST_H
#define BRAKEDA3D_GRAVITATIONALGHOST_H


#include "../Physics/Mesh3DGhost.h"

class GravitationalGhost: public Mesh3DGhost {
private:
    float force;
    float factor;
public:
    GravitationalGhost(float force, float factor);

    float getForce() const;

    void setForce(float force);

    virtual void onUpdate() override;

    float getFactor() const;

    void setFactor(float factor);
};


#endif //BRAKEDA3D_GRAVITATIONALGHOST_H
