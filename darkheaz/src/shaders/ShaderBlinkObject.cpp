#include "ShaderBlinkObject.h"
#include "../../../include/EngineSetup.h"
#include "../../../include/EngineBuffers.h"
#include "../../../include/Brakeza3D.h"

ShaderBlinkObject::ShaderBlinkObject() : ShaderOpenCL("blink.opencl")
{
    this->image = new Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png");

    opencl_buffer_stencil = clCreateBuffer(
        context,
        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
        EngineBuffers::getInstance()->sizeBuffers * sizeof(bool),
        this->image->pixels(),
        &clRet
    );
}

void ShaderBlinkObject::update()
{
    Shader::update();

    if (!isEnabled()) return;

    executeKernelOpenCL();
}

void ShaderBlinkObject::executeKernelOpenCL()
{

}

void ShaderBlinkObject::setObject(Object3D *o) {

}

void ShaderBlinkObject::setColor(Color c) {

}

ShaderBlinkObject::~ShaderBlinkObject() {

}
