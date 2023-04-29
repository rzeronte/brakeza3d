
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
    int screenWidth;
    int screenHeight;
    Mesh3D* object;
    Color color;
    Counter counter;
public:
    ShaderBlink(bool active, Mesh3D *o, float step, Color c) :
        ShaderOpenCL(active, "blink.opencl"),
        isBlinking(false),
        screenWidth(EngineSetup::get()->screenWidth),
        screenHeight(EngineSetup::get()->screenHeight),
        object(o),
        color(c),
        counter(Counter(step))
    {
    }

    ~ShaderBlink() override {
    }

    void update() override
    {
        if (!isEnabled()) return;
        if (this->object == nullptr) return;
        if (object->isRemoved()) return;

        counter.update();

        if (!this->object->isStencilBufferEnabled()) return;

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
        clSetKernelArg(kernel, 0, sizeof(int), &screenWidth);
        clSetKernelArg(kernel, 1, sizeof(int), &screenHeight);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&EngineBuffers::get()->videoBufferOCL);
        clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&object->getOpenClRenderer()->clBufferStencil);
        clSetKernelArg(kernel, 4, sizeof(float), &this->color.r);
        clSetKernelArg(kernel, 5, sizeof(float), &this->color.g);
        clSetKernelArg(kernel, 6, sizeof(float), &this->color.b);

        size_t global_item_size = EngineBuffers::get()->sizeBuffers;
        size_t local_item_size = 256;

        clRet = clEnqueueNDRangeKernel(clQueue, kernel, 1, nullptr, &global_item_size, &local_item_size, 0, nullptr, nullptr );

        this->debugKernel("ShaderBlink");
    }
};
#endif //BRAKEDA3D_SHADERBLINK_H
