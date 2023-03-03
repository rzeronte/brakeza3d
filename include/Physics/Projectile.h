//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_PROJECTILE_H
#define BRAKEZA3D_PROJECTILE_H


class Projectile {
private:
    Vertex3D direction;
public:

    Projectile(const Vertex3D direction) : direction(direction)
    {
    }

    [[nodiscard]] const Vertex3D &getDirection() const {
        return direction;
    }

    void setDirection(const Vertex3D &value) {
        Projectile::direction = value;
    }

};


#endif //BRAKEZA3D_PROJECTILE_H
