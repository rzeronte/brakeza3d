//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_PROJECTILE_H
#define BRAKEZA3D_PROJECTILE_H


class Projectile {
private:
    Counter *timeToLive;
    float ttl;
public:
    Projectile(float ttl, const Vertex3D &direction) : ttl(ttl), direction(direction)
    {
        timeToLive = new Counter(ttl);
        timeToLive->setEnabled(true);
    }

protected:
    Vertex3D direction;

public:
    [[nodiscard]] float getTTL() const {
        return ttl;
    }

    void setTTL(float v) {
        this->ttl = v;
        this->timeToLive->setStep(v);
        this->timeToLive->setEnabled(true);
    }

    [[nodiscard]] const Vertex3D &getDirection() const {
        return direction;
    }

    void setDirection(const Vertex3D &value) {
        Projectile::direction = value;
    }

    [[nodiscard]] Counter *getTimeToLive() const {
        return timeToLive;
    }
};


#endif //BRAKEZA3D_PROJECTILE_H
