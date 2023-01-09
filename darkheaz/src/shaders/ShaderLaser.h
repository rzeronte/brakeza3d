//
// Created by eduardo on 8/01/23.
//

#ifndef BRAKEZA3D_SHADERLASER_H
#define BRAKEZA3D_SHADERLASER_H


#include "../../../include/Render/ShaderOpenCL.h"

class ShaderLaser : public ShaderOpenCL {
    float intensity;
    float reach;
    float damage;
public:
    explicit ShaderLaser();

    void update() override;
    void executeKernelOpenCL();

    float getIntensity() const;

    void setIntensity(float intensity);

    float getReach() const;

    void setReach(float reach);

    float getDamage() const;

    void setDamage(float damage);
};


#endif //BRAKEZA3D_SHADERLASER_H
