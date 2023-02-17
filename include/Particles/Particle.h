#ifndef BRAKEDA3D_PARTICLE_H
#define BRAKEDA3D_PARTICLE_H

#include "../Objects/Object3D.h"
#include "../Misc/Counter.h"
#include "../Misc/Color.h"

class Particle: public Object3D {
public:
    Particle(Object3D *parent, float force, float ttl, Color c);
    void onUpdate() override;
    void postUpdate();
private:
    Timer timer;
    Counter timeToLive;
    Vertex3D velocity;
    float force;
    Color color;
};


#endif //BRAKEDA3D_PARTICLE_H
