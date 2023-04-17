
#ifndef BRAKEDA3D_SHADERBLINK_H
#define BRAKEDA3D_SHADERBLINK_H
#include "../Misc/Color.h"
#include "../Misc/Tools.h"
#include "../EngineBuffers.h"
#include "../Render/Transforms.h"
#include "../Render/Drawable.h"
#include "../Render/Logging.h"
#include "../Render/ShaderOpenCL.h"

class ShaderBlink : public ShaderOpenCL {
    bool isBlinking;
    cl_mem opencl_buffer_stencil;
    int screenWidth;
    int screenHeight;
    Object3D* object;
    Color color;
    Counter counter;
public:
    ShaderBlink(bool active, Object3D *o, float step, Color c) :
        ShaderOpenCL(active, "blink.opencl"),
        isBlinking(false),
        screenWidth(EngineSetup::get()->screenWidth),
        screenHeight(EngineSetup::get()->screenHeight),
        object(o),
        color(c),
        counter(Counter(step))
    {
        opencl_buffer_stencil = clCreateBuffer(
            context,
            CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
            EngineBuffers::getInstance()->sizeBuffers * sizeof(bool),
            this->object->getStencilBuffer(),
            nullptr
        );
    }

    ~ShaderBlink() override {
        clReleaseMemObject(opencl_buffer_stencil);
    }

    void update() override
    {
        if (!isEnabled()) {
            return;
        }

        if (this->object == nullptr) {
            return;
        }

        counter.update();

        if (!this->object->isStencilBufferEnabled()) {
            return;
        }

        if (isBlinking) {
            if (counter.isFinished()) {
                isBlinking = false;
                counter.setEnabled(true);
            }
        } else {
            if (counter.isFinished()) {
                isBlinking = true;
                counter.setEnabled(true);
            }
        }

        if (isBlinking) {
            auto mesh = dynamic_cast<Mesh3D*> (object);
            if (mesh == nullptr) {
                return;
            }
            executeKernelOpenCL();
        }
    }

    void executeKernelOpenCL()
    {
            clEnqueueWriteBuffer(
            clCommandQueue,
            openClBufferMappedWithVideoInput,
            CL_TRUE,
            0,
            this->bufferSize * sizeof(Uint32),
            EngineBuffers::getInstance()->videoBuffer,
            0,
            nullptr,
            nullptr
        );

        clEnqueueWriteBuffer(
            clCommandQueue,
            opencl_buffer_stencil,
            CL_TRUE,
            0,
            this->bufferSize * sizeof(bool),
            object->getStencilBuffer(),
            0,
            nullptr,
            nullptr
        );

        clSetKernelArg(kernel, 0, sizeof(int), &screenWidth);
        clSetKernelArg(kernel, 1, sizeof(int), &screenHeight);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&openClBufferMappedWithVideoInput);
        clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&opencl_buffer_stencil);
        clSetKernelArg(kernel, 4, sizeof(float), &this->color.r);
        clSetKernelArg(kernel, 5, sizeof(float), &this->color.g);
        clSetKernelArg(kernel, 6, sizeof(float), &this->color.b);

        // Process the entire lists
        size_t global_item_size = EngineBuffers::getInstance()->sizeBuffers;
        // Divide work items into groups of 64
        size_t local_item_size = 64;

        clRet = clEnqueueNDRangeKernel(
            clCommandQueue,
            kernel,
            1,
            nullptr,
            &global_item_size,
            &local_item_size,
            0,
            nullptr,
            nullptr
        );

        clEnqueueReadBuffer(
            clCommandQueue,
            openClBufferMappedWithVideoInput,
            CL_TRUE,
            0,
            this->bufferSize * sizeof(Uint32),
            EngineBuffers::getInstance()->videoBuffer,
            0,
            nullptr,
            nullptr
        );

        this->debugKernel();
    }
};
#endif //BRAKEDA3D_SHADERBLINK_H
