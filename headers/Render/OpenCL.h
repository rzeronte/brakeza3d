#ifndef BRAKEDA3D_OPENCL_H
#define BRAKEDA3D_OPENCL_H


#include <OpenCL/opencl.h>

class OpenCL {
private:
    static OpenCL* instance;

    OpenCL();
    ~OpenCL();

public:
    static OpenCL* getInstance();

    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;
    cl_context context;
    cl_command_queue command_queue;

    cl_kernel kernel;
    cl_kernel barycentric;

    cl_program *openCLPrograms;
    int numberOpenCLPrograms = 0;

    cl_mem alphaXY_mem_obj;
    cl_mem thetaXY_mem_obj;
    cl_mem gammaXY_mem_obj;

    void createOpenCLProgram(char *fileProgram);

};


#endif //BRAKEDA3D_OPENCL_H
