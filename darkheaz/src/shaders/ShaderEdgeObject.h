//
// Created by eduardo on 14/11/22.
//

#ifndef BRAKEZA3D_SHADEREDGEOBJECT_H
#define BRAKEZA3D_SHADEREDGEOBJECT_H


#include "../../../include/Render/ShaderOpenCL.h"
#include "../../../include/Misc/Image.h"
#include "../../../include/Objects/Object3D.h"
#include "../../../include/Objects/Mesh3D.h"

class ShaderEdgeObject : public ShaderOpenCL {
    Image *image;
    cl_mem opencl_buffer_stencil;

    Object3D* object;
    Color color;

public:
    ShaderEdgeObject();

    void update() override;

    ~ShaderEdgeObject() override;

    void executeKernelOpenCL();

    void setObject(Object3D *o);

    void setColor(Color c);

};


#endif //BRAKEZA3D_SHADEREDGEOBJECT_H
