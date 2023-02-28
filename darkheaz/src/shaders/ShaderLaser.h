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

    int filterGroup;
    int filterMask;

    Color color;
    Object3D* target;
    Object3D* parent;

    Image *image;

    cl_mem opencl_buffer_pixels_image;
public:
    explicit ShaderLaser(Object3D *parent, Color c, int filterGroup, int filterMask);

    void update() override;
    void executeKernelOpenCL();

    [[nodiscard]] float getIntensity() const;

    void setIntensity(float intensity);

    [[nodiscard]] float getReach() const;

    void resetReach();

    [[nodiscard]] float getDamage() const;

    void setDamage(float damage);

    void increaseReach();

    [[nodiscard]] float getSpeed() const;

    void setSpeed(float speed);

    [[nodiscard]] Object3D *getTarget() const;

    void setTarget(Object3D *object);

    [[nodiscard]] const Color &getColor() const;

    void setColor(const Color &c);
};


#endif //BRAKEZA3D_SHADERLASER_H
