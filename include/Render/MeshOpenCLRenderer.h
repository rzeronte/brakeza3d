//
// Created by eduardo on 20/04/23.
//

#ifndef BRAKEZA3D_MESHOPENCLRENDERER_H
#define BRAKEZA3D_MESHOPENCLRENDERER_H


#include <string>
#include <CL/cl.h>
#include <vector>
#include "../Objects/Triangle3D.h"
#include "../Objects/LightPoint3D.h"
#include "../Objects/OpenCLTypes.h"


#define MAX_OPENCL_TRIANGLES 10240
#define MAX_OPENCL_LIGHTS 16


class MeshOpenCLRenderer {

    cl_command_queue clQueue;
    cl_device_id clDeviceId;
    cl_int clRet;
    cl_context context;

    cl_mem clBufferTriangles;
    cl_mem clBufferLights;
    cl_mem clBufferMeshContext;

    std::vector<Triangle*> &triangles;

    std::vector<OCTriangle> oclTriangles;
    std::vector<OCLight> oclLights;

    Object3D *object;

    OCLMeshContext meshContext;
public:
    virtual ~MeshOpenCLRenderer();

    MeshOpenCLRenderer(Object3D *parent, std::vector<Triangle*> &triangles);

    void makeOCLTriangles();

    void onUpdate(Texture *texture);

    void debugKernel() const;

    void updateTriangles();

    cl_mem clBufferStencil;

    void updateLights();
};


#endif //BRAKEZA3D_MESHOPENCLRENDERER_H
