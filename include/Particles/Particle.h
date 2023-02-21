#ifndef BRAKEDA3D_PARTICLE_H
#define BRAKEDA3D_PARTICLE_H

#include "../Objects/Object3D.h"
#include "../Misc/Counter.h"
#include "../Misc/Color.h"

class Particle {
public:
    Particle(Object3D *parent, float force, float ttl, Color c);
    void onUpdate();
private:
    Object3D *parent;
    Vertex3D position;
    M3 rotation;

    bool active;
    Timer timer;
    Counter timeToLive;
    Vertex3D velocity;
    float force;
    Color color;

public:
    [[nodiscard]] const Vertex3D &getPosition() const;

    void setPosition(const Vertex3D &position);

    [[nodiscard]] const M3 &getRotation() const;

    void setRotation(const M3 &rotation);

    [[nodiscard]] bool isActive() const;

    void setActive(bool active);

    [[nodiscard]] Object3D *getParent() const;

    void setParent(Object3D *parent);
};


#endif //BRAKEDA3D_PARTICLE_H
