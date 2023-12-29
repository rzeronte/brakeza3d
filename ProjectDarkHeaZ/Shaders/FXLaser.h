//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_FXLASER_H
#define BRAKEZA3D_FXLASER_H


#include <CL/cl.h>
#include "../../include/Misc/Image.h"
#include "../../include/OpenGL/FXEffectOpenGL.h"
#include "../Weapons/ProjectileRay.h"

#define MAX_PROJECTILES 256
#define MAX_LASERS 256

struct OCLaser
{
    glm::vec2 from;
    glm::vec2 to;
    glm::vec3 color;
    float intensity;
    bool startCircle;
    bool endCircle;
};

struct OCProjectile
{
    int x;
    int y;
    int r;
    int g;
    int b;
    float intensity;
};

class FXLaser : public FXEffectOpenGL {
private:
    Image image;

    std::vector<OCLaser> lasers;
    std::vector<OCProjectile> projectiles;
public:
    FXLaser();

    void update() override;

    void addLaser(glm::vec2 from, glm::vec2 to, glm::vec3 color, float intensity, bool startCircle, bool endCircle);
    void addProjectile(Vertex3D position, Color color, float i);
    void addLaserFromRay(ProjectileRay *pRay);

    void executeKernelOpenCL();
};


#endif //BRAKEZA3D_FXLASER_H
