//
// Created by eduardo on 19/01/23.
//

#ifndef BRAKEZA3D_SHADERPROJECTILES_H
#define BRAKEZA3D_SHADERPROJECTILES_H


#include <CL/cl.h>
#include "../../../include/Misc/Image.h"
#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Physics/ProjectileRay.h"

#define MAX_PROJECTILES 250
#define MAX_LASERS 100

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

class ShaderProjectiles : public ShaderOpenCL {
private:
    Image image;

    cl_mem opencl_buffer_pixels_image;
    cl_mem clBufferLasers;
    cl_mem clBufferProjectiles;

    std::vector<OCLaser> lasers;
    std::vector<OCProjectile> projectiles;
public:
    ShaderProjectiles();

    void update() override;

    void addLaser(int x1, int y1, int x2, int y2, int r, int g, int b, float intensity, bool startCircle, bool endCircle);
    void addProjectile(Vertex3D position, Color color, float i);
    void addLaserFromRay(ProjectileRay *pRay);

    void executeKernelOpenCL();
};


#endif //BRAKEZA3D_SHADERPROJECTILES_H
