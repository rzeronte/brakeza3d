//
// Created by eduardo on 3/07/23.
//

#ifndef BRAKEZA3D_LIVINGOBJECT_H
#define BRAKEZA3D_LIVINGOBJECT_H


#include "../../include/Misc/Counter.h"
#include "../../include/FXEffect/FXBlink.h"

class LivingObject {
protected:
    float startStamina;
    float stamina;
    FXBlink *blink;
    Counter counterDamageBlink;
public:
    explicit LivingObject(Mesh3D *parent);

    virtual ~LivingObject();

    [[nodiscard]] float getStartStamina() const;

    [[nodiscard]] float getStamina() const;

    void setStamina(float stamina);

    void setStartStamina(float startStamina);

    [[nodiscard]] float currentStaminaPercentage() const;
};


#endif //BRAKEZA3D_LIVINGOBJECT_H
