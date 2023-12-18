//
// Created by eduardo on 30/05/22.
//

#ifndef BRAKEZA3D_SHADEROPENCL_H
#define BRAKEZA3D_SHADEROPENCL_H


#include <CL/cl.h>
#include "Shader.h"
#include "../Misc/cJSON.h"

class ShaderOpenCL: public Shader {
protected:
public:
    explicit ShaderOpenCL(bool active);

    virtual void preUpdate();

    virtual void postUpdate();

    virtual void drawImGuiProperties();

    virtual cJSON * getJSON();
};


#endif //BRAKEZA3D_SHADEROPENCL_H
