//
// Created by eduardo on 16/10/23.
//

#include "ObjectShaderOpenCL.h"

ObjectShaderOpenCL::ObjectShaderOpenCL(bool active, const std::string &kernelFilename, Object3D *object)
:
    ShaderOpenCL(active, kernelFilename),
    object(object)
{
}

ObjectShaderOpenCL::ObjectShaderOpenCL(bool active, Object3D *object)
:
    ShaderOpenCL(active),
    object(object)
{
}

Object3D *ObjectShaderOpenCL::getObject() const {
    return object;
}

void ObjectShaderOpenCL::setObject(Object3D *object) {
    ObjectShaderOpenCL::object = object;
}

