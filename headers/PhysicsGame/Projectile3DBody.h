
#ifndef BRAKEDA3D_PROJECTILE3DBODY_H
#define BRAKEDA3D_PROJECTILE3DBODY_H


#include "../Physics/SpriteDirectional3DBody.h"

class Projectile3DBody : public SpriteDirectional3DBody {
private:
    bool fromEnemy;

public:
    Projectile3DBody();

    bool isFromEnemy();
    void setFromEnemy(bool isFromEnemy);
};


#endif //BRAKEDA3D_PROJECTILE3DBODY_H
