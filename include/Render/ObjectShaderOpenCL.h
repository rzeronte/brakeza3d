
#ifndef BRAKEZA3D_OBJECTSHADEROPENCL_H
#define BRAKEZA3D_OBJECTSHADEROPENCL_H

#include "ShaderOpenCL.h"
#include "../Objects/Object3D.h"

class ObjectShaderOpenCL : public ShaderOpenCL {
protected:
    Object3D *object;
public:
    ObjectShaderOpenCL(bool active, Object3D *object);

    [[nodiscard]] Object3D *getObject() const;

    void setObject(Object3D *object);
};


#endif //BRAKEZA3D_OBJECTSHADEROPENCL_H
