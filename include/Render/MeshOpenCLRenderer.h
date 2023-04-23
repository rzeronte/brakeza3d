//
// Created by eduardo on 20/04/23.
//

#ifndef BRAKEZA3D_MESHOPENCLRENDERER_H
#define BRAKEZA3D_MESHOPENCLRENDERER_H


#include <string>
#include <CL/cl.h>
#include <vector>
#include "../Objects/Triangle3D.h"


#define MAX_OPENCL_TRIANGLES 1024

struct OCLPlane {
    OCLPlane() {}

    OCLPlane(OCVertex3D a, OCVertex3D b, OCVertex3D c, OCVertex3D normal) {
        this->A = a;
        this->B = b;
        this->C = c;
        this->normal = normal;
    }
    OCVertex3D A;
    OCVertex3D B;
    OCVertex3D C;
    OCVertex3D normal;
};

struct ObjectData {
    ObjectData(const OCVertex3D &position, const OCVertex3D &rotation, float scale)
    :
    position(position), rotation(rotation), scale(scale)
    {
    }
    OCVertex3D position;  // Posición del objeto
    OCVertex3D rotation;  // Rotación del objeto
    float scale;        // Escala del objeto
};

struct CameraData{
    CameraData(const OCVertex3D &position, const OCVertex3D &rotation) : position(position), rotation(rotation) {}
    OCVertex3D position;  // Posición de la cámara
    OCVertex3D rotation;  // Rotación de la cámara
};

struct FrustumData {
    FrustumData(OCVertex3D vNL, OCVertex3D vNR, OCVertex3D vNT, OCVertex3D vNB, std::vector<OCLPlane> planes)
    :
    vNL(vNL), vNR(vNR), vNT(vNT), vNB(vNB)
    {
        for (size_t i = 0; i < planes.size(); ++i) {
            this->planes[i] = planes[i];
        }
    }
    OCVertex3D vNL;       // Vértice Near Plane Left
    OCVertex3D vNR;       // Vértice Near Plane Right
    OCVertex3D vNT;       // Vértice Near Plane Top
    OCVertex3D vNB;       // Vértice Near Plane Bottom
    OCLPlane planes[4];  // Frustum planes (OCPlanes)
};

struct OCLMeshContext {
    OCLMeshContext(const ObjectData &objectData, const CameraData &cameraData, const FrustumData &frustumData)
    :
    objectData(objectData),
    cameraData(cameraData),
    frustumData(frustumData)
    {
    }

    ObjectData objectData;
    CameraData cameraData;
    FrustumData frustumData;
};

class MeshOpenCLRenderer {
    std::string kernelFilename;

    cl_command_queue clQueue;
    cl_device_id clDeviceId;
    cl_int clRet;
    cl_context context;

    cl_program program;
    cl_kernel kernel;

    cl_mem clBufferTriangles;
    cl_mem clBufferMeshContext;

    std::vector<Triangle*> &triangles;

    void initOpenCLProgram();

public:
    virtual ~MeshOpenCLRenderer();

    MeshOpenCLRenderer(const std::string& kernelFilename, std::vector<Triangle*> &triangles);

    void onUpdate(OCLMeshContext *context);

    void debugKernel() const;

    std::vector<OCTriangle> openCLTriangles();

};


#endif //BRAKEZA3D_MESHOPENCLRENDERER_H
