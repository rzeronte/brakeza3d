//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_PROJECTILE_H
#define BRAKEZA3D_PROJECTILE_H


class Projectile {
private:
    Counter timeToLive;

    float ttl;
    Vertex3D direction;
public:

    Projectile(float ttl, const Vertex3D direction) : timeToLive(Counter(ttl)), ttl(ttl), direction(direction)
    {
    }

    [[nodiscard]] float getTTL() const {
        return ttl;
    }

    [[nodiscard]] const Vertex3D &getDirection() const {
        return direction;
    }

    void setDirection(const Vertex3D &value) {
        Projectile::direction = value;
    }

    [[nodiscard]] Counter &getTimeToLive() {
        return timeToLive;
    }
};


#endif //BRAKEZA3D_PROJECTILE_H
