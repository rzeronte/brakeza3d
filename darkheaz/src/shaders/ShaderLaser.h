//
// Created by eduardo on 8/01/23.
//

#ifndef BRAKEZA3D_SHADERLASER_H
#define BRAKEZA3D_SHADERLASER_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Misc/Image.h"
#include "../../../include/Objects/Object3D.h"

class ShaderLaser : public ShaderOpenCL {
    float intensity;
    float reach;
    float speed;
    float damage;

    Color color;
    Object3D* target;
    Object3D* parent;

    Image *image;

    cl_mem opencl_buffer_pixels_image;
public:
    explicit ShaderLaser(Object3D *parent);

    void update() override;
    void executeKernelOpenCL();

    float getIntensity() const;

    void setIntensity(float intensity);

    float getReach() const;

    void setReach(float reach);

    float getDamage() const;

    void setDamage(float damage);

    void increaseReach();

    float getSpeed() const;

    void setSpeed(float speed);

    Object3D *getTarget() const;

    void setTarget(Object3D *object);

    const Color &getColor() const;

    void setColor(const Color &c);
};


#endif //BRAKEZA3D_SHADERLASER_H
