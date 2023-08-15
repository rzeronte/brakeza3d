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

class MeshOpenCLRenderer {

    cl_command_queue clQueue;
    cl_device_id clDeviceId;
    cl_int clRet;
    cl_context context;

    cl_mem clBufferTriangles;
    cl_mem clBufferMeshContext;

    std::vector<Triangle*> &triangles;

    std::vector<OCTriangle> oclTriangles;

    Object3D *object;

    OCLMeshContext meshContext;
public:
    virtual ~MeshOpenCLRenderer();

    MeshOpenCLRenderer(Object3D *parent, std::vector<Triangle*> &triangles);

    void makeOCLTriangles();

    void onUpdate(Image *texture);

    void debugKernel() const;

    void updateTriangles();

    cl_mem clBufferStencil;

    cl_mem *getClBufferTriangles();

    cl_mem *getClBufferMeshContext();

    [[nodiscard]] const std::vector<OCTriangle> &getOclTriangles() const;
};


#endif //BRAKEZA3D_MESHOPENCLRENDERER_H
