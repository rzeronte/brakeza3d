
#ifndef BRAKEDA3D_PROJECTILE3DBODY_H
#define BRAKEDA3D_PROJECTILE3DBODY_H


#include "../Physics/Mesh3DBody.h"

class Projectile3DBody : public Mesh3DBody {
private:
    bool fromEnemy;
    float damage;
    float damageRadius;

public:
    Projectile3DBody();

    bool isFromEnemy();
    void setFromEnemy(bool isFromEnemy);

    float getDamage() const;
    void  setDamage(float damage);

    float getDamageRadius() const;
    void  setDamageRadius(float damageRadius);
};


#endif //BRAKEDA3D_PROJECTILE3DBODY_H
