//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_FXLASER_H
#define BRAKEZA3D_FXLASER_H

#include "../../include/Misc/Image.h"
#include "../../include/OpenGL/FXEffectOpenGL.h"
#include "../Weapons/ProjectileRay.h"

struct OCLaser
{
    glm::vec2 from;
    glm::vec2 to;
    glm::vec3 color;
    float intensity;
    float weight;
    float speed;
};

class FXLaser : public FXEffectOpenGL {
private:
    Image *mask;
    std::vector<OCLaser> lasers;
public:
    explicit FXLaser(Image *mask);

    void update() override;

    void addLaser(glm::vec2 from, glm::vec2 to, glm::vec3 color, float intensity, float weight, float maskSpeed);
    void addLaserFromRay(ProjectileRay *pRay);
};


#endif //BRAKEZA3D_FXLASER_H
