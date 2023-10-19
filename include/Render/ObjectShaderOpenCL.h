//
// Created by eduardo on 16/10/23.
//

#ifndef BRAKEZA3D_OBJECTSHADEROPENCL_H
#define BRAKEZA3D_OBJECTSHADEROPENCL_H


#include "ShaderOpenCL.h"
#include "../Objects/Object3D.h"

class ObjectShaderOpenCL : public ShaderOpenCL {

protected:
    Object3D *object;
public:
    ObjectShaderOpenCL(bool active, const std::string &kernelFilename, Object3D *object);

    ObjectShaderOpenCL(bool active, Object3D *object);

    [[nodiscard]] Object3D *getObject() const;

    void setObject(Object3D *object);
};


#endif //BRAKEZA3D_OBJECTSHADEROPENCL_H
