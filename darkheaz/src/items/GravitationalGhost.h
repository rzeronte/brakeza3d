//
// Created by eduardo on 3/04/22.
//

#ifndef BRAKEDA3D_GRAVITATIONALGHOST_H
#define BRAKEDA3D_GRAVITATIONALGHOST_H


#include "../../../include/Physics/Mesh3DGhost.h"
#include "../../../include/Shaders/ShaderBlink.h"

class GravitationalGhost: public Mesh3DGhost {
private:
    float force;
    float factor;
public:
    GravitationalGhost(float force, float factor);

    void setForce(float force);

    void onUpdate() override;

    void setFactor(float factor);

    [[nodiscard]] float getFactor() const;

    [[nodiscard]] float getForce() const;
};


#endif //BRAKEDA3D_GRAVITATIONALGHOST_H
