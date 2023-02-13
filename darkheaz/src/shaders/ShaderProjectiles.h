//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_SHADERPROJECTILES_H
#define BRAKEZA3D_SHADERPROJECTILES_H


#include <CL/cl.h>
#include "../../../include/Misc/Image.h"
#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Physics/ProjectileRay.h"

struct OCLaser
{
    int x1;
    int y1;
    int x2;
    int y2;
    int r;
    int g;
    int b;
    float intensity;
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

class ShaderProjectiles : public ShaderOpenCL {
private:
    float reach;
    float speed;
    float damage;

    Image *image;

    cl_mem opencl_buffer_pixels_image;
    cl_mem opencl_buffer_lasers;
    cl_mem opencl_buffer_projectiles;
    std::vector<OCLaser> lasers;
    std::vector<OCProjectile> projectiles;
public:
    ShaderProjectiles();

    void update() override;

    void addLaser(int x1, int y1, int x2, int y2, int r, int g, int b, float intensity);
    void addProjectile(Vertex3D position, Color color, float i);
    void addLaserFromRay(ProjectileRay *pRay);

    void executeKernelOpenCL();
};


#endif //BRAKEZA3D_SHADERPROJECTILES_H
