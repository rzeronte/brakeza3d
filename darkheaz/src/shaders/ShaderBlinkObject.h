//
// Created by eduardo on 20/11/22.
//

#ifndef BRAKEZA3D_SHADERBLINKOBJECT_H
#define BRAKEZA3D_SHADERBLINKOBJECT_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Misc/Image.h"
#include "../../../include/Objects/Object3D.h"

class ShaderBlinkObject  : public ShaderOpenCL {
    Image *image;
    cl_mem opencl_buffer_stencil;

    Object3D* object;
    Color color;

public:
    ShaderBlinkObject();

    void update() override;

    ~ShaderBlinkObject() override;

    void executeKernelOpenCL();

    void setObject(Object3D *o);

    void setColor(Color c);
};


#endif //BRAKEZA3D_SHADERBLINKOBJECT_H
