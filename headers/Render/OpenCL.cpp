#include <cstdio>
#include "OpenCL.h"

#define MAX_SOURCE_SIZE (0x100000)

OpenCL* OpenCL::instance = 0;

OpenCL* OpenCL::getInstance()
{
    if (instance == 0) {
        instance = new OpenCL();
    }

    return instance;
}

OpenCL::OpenCL()
{
    this->openCLPrograms = new cl_program[10];

    ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 4, &device_id, &ret_num_devices);

    // Create an OpenCL context
    context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);


    // Create a command queue
    command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    alphaXY_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,  320 * sizeof(float), NULL, &ret);
    thetaXY_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,  320 * sizeof(float), NULL, &ret);
    gammaXY_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,  320 * sizeof(float), NULL, &ret);

    this->createOpenCLProgram("../vector_add_kernel.cl");

    // Build the program
    ret = clBuildProgram(this->openCLPrograms[0], 1, &device_id, NULL, NULL, NULL);

    // Create the OpenCL kernel
    kernel = clCreateKernel(this->openCLPrograms[0], "vector_add", &ret);
    barycentric = clCreateKernel(this->openCLPrograms[0], "barycentric", &ret);
}


void OpenCL::createOpenCLProgram(char *fileProgram)
{
    // Load the kernel source code into the array source_str
    FILE *fp;
    char *source_str;
    size_t source_size;

    fp = fopen(fileProgram, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose( fp );

    // Create a program from the kernel source
    this->openCLPrograms[this->numberOpenCLPrograms] = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    this->numberOpenCLPrograms++;
}