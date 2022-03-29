//
// Created by eduardo on 25/3/22.
//

#ifndef BRAKEDA3D_AMMOSMARTPROJECTILEBODY_H
#define BRAKEDA3D_AMMOSMARTPROJECTILEBODY_H


#include "AmmoProjectileBody.h"

class AmmoSmartProjectileBody : public AmmoProjectileBody {
    Object3D *target;
public:
    Object3D *getTarget() const;

    void setTarget(Object3D *target);

    void onUpdate() override;
};


#endif //BRAKEDA3D_AMMOSMARTPROJECTILEBODY_H
