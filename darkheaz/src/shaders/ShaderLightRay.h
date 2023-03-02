//
// Created by eduardo on 8/01/23.
//

#ifndef BRAKEZA3D_SHADERLIGHTRAY_H
#define BRAKEZA3D_SHADERLIGHTRAY_H

#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Misc/Image.h"
#include "../../../include/Objects/Object3D.h"

class ShaderLightRay : public ShaderOpenCL {
private:
    float intensity;
    float reach;
    float speed;
    float damage;

    int filterGroup;
    int filterMask;

    Color color;
    Object3D* target;
    Object3D* parent;

    Image image;

    cl_mem opencl_buffer_pixels_image;
public:
    explicit ShaderLightRay(Object3D *parent,  float speed, float damage, Color c, int filterGroup, int filterMask);

    ~ShaderLightRay() override;

    void update() override;

    void executeKernelOpenCL();

    void setIntensity(float intensity);

    void resetReach();

    void setDamage(float damage);

    void increaseReach();

    void setSpeed(float speed);

    void setTarget(Object3D *object);

    void setColor(const Color &c);

    [[nodiscard]] float getReach() const;
    [[nodiscard]] float getDamage() const;
    [[nodiscard]] float getSpeed() const;
    [[nodiscard]] Object3D *getTarget() const;
    [[nodiscard]] const Color &getColor() const;
};


#endif //BRAKEZA3D_SHADERLIGHTRAY_H
