
#ifndef BRAKEDA3D_ENEMY_H
#define BRAKEDA3D_ENEMY_H


#include <vector>
#include "../Objects/Vertex3D.h"
#include "../Physics/Body.h"
#include "../Misc/Counter.h"
#include "../Render/M3.h"

class Enemy {
public:
    std::string classname;  // For BSP classname match

    Vertex3D respawnPosition;
    M3 respawnRotation;

    float startStamina;
    float stamina;

    bool dead;

    float damage;   // Damage from enemy to others

    float cadence;  // cadence management
    Counter *counterCadence;

    float range;
    float speed;

    Enemy();

    void takeDamage(float damageTaken);

    bool isTakeHeavyDamage(float damageTaken) const;

    float getDamage() const;

    void setDamage(float damage);

    float getRange() const;

    void setRange(float range);

    float getSpeed() const;

    void setSpeed(float speed);

    float getCadence() const;

    void setCadence(float cadence);

    const std::string &getClassname() const;

    void setClassname(const std::string &classname);

    bool isDead() const;

    void setDead(bool dead);

    const Vertex3D &getRespawnPosition() const;

    void setRespawnPosition(const Vertex3D &respawnPosition);

    const M3 &getRespawnRotation() const;

    void setRespawnRotation(const M3 &respawnRotation);
};


#endif //BRAKEDA3D_ENEMY_H
