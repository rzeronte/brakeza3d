//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_SHADERLASERS_H
#define BRAKEZA3D_SHADERLASERS_H


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

class ShaderLasers : public ShaderOpenCL {
private:
    float intensity;
    float reach;
    float speed;
    float damage;

    Image *image;

    cl_mem opencl_buffer_pixels_image;
    cl_mem opencl_buffer_lasers;
    std::vector<OCLaser> lasers;
public:
    ShaderLasers();

    void update() override;

    void addLaser(int x1, int y1, int x2, int y2, int r, int g, int b, float intensity);
    void addLaserFromRay(ProjectileRay *pRay);

    void executeKernelOpenCL();
};


#endif //BRAKEZA3D_SHADERLASERS_H
