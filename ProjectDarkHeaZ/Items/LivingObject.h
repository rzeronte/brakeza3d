//
// Created by eduardo on 3/07/23.
//

#ifndef BRAKEZA3D_LIVINGOBJECT_H
#define BRAKEZA3D_LIVINGOBJECT_H


#include "../../include/Misc/Counter.h"
#include "../../include/Shaders/ShaderBlink.h"

#define INITIAL_STAMINA 100

class LivingObject {
protected:
    float startStamina;
    float stamina;
    ShaderBlink *blink;
    Counter counterDamageBlink;
public:
    explicit LivingObject(Mesh3D *parent);

    [[nodiscard]] float getStartStamina() const;

    [[nodiscard]] float getStamina() const;

    void setStamina(float stamina);

    void setStartStamina(float startStamina);

    float currentStaminaPercentage();
};


#endif //BRAKEZA3D_LIVINGOBJECT_H
