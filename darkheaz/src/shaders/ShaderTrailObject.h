//
// Created by eduardo on 26/11/22.
//

#ifndef BRAKEZA3D_SHADERTRAILOBJECT_H
#define BRAKEZA3D_SHADERTRAILOBJECT_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Objects/Object3D.h"
#include "../../../include/Misc/Image.h"

class ShaderTrailObject: public ShaderOpenCL {
    cl_mem opencl_buffer_stencil;
    cl_mem opencl_buffer_image;
    Object3D* object;
    Color color;
public:
    explicit ShaderTrailObject(Object3D *o, Color c);

    void update() override;

    void executeKernelOpenCL();

    ~ShaderTrailObject() override;

};


#endif //BRAKEZA3D_SHADERTRAILOBJECT_H
