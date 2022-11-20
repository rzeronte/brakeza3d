
#ifndef BRAKEDA3D_SHADERBLINK_H
#define BRAKEDA3D_SHADERBLINK_H
#include "../Misc/Color.h"
#include "../Misc/Tools.h"
#include "../EngineBuffers.h"
#include "../Render/Transforms.h"
#include "../Render/Drawable.h"
#include "../Render/Logging.h"
#include "../Render/ShaderOpenCL.h"

#define DEFAULT_BLINK_SECONDS 1
class ShaderBlink : public ShaderOpenCL {
    bool isBlinking = false;
    Camera3D *camera;
    Image *image;
    cl_mem opencl_buffer_stencil;
    int screenWidth;
    int screenHeight;
    Object3D* object;
    Color color;
    Counter counter;
public:
    ShaderBlink(Camera3D *camera) : ShaderOpenCL("blink.opencl") {
        this->screenHeight = EngineSetup::get()->screenHeight;
        this->screenWidth = EngineSetup::get()->screenWidth;
        this->object = nullptr;
        this->color = Color::green();
        setStep(DEFAULT_BLINK_SECONDS);
        setPhaseRender(EngineSetup::ShadersPhaseRender::POSTUPDATE);
        this->counter.setStep(step);
        this->counter.setEnabled(true);
        this->camera = camera;

        this->image = new Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png");

        opencl_buffer_stencil = clCreateBuffer(
            context,
            CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
            EngineBuffers::getInstance()->sizeBuffers * sizeof(bool),
            this->image->pixels(),
            &clRet
        );
    }

    ShaderBlink(Camera3D *camera, Object3D *o): ShaderOpenCL("blink.opencl") {
        this->object = o;
        this->screenHeight = EngineSetup::get()->screenHeight;
        this->screenWidth = EngineSetup::get()->screenWidth;
        this->color = Color::green();
        setStep(DEFAULT_BLINK_SECONDS);
        setPhaseRender(EngineSetup::ShadersPhaseRender::POSTUPDATE);
        this->camera = camera;
        this->image = new Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png");

        opencl_buffer_stencil = clCreateBuffer(
                context,
                CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                EngineBuffers::getInstance()->sizeBuffers * sizeof(bool),
                this->image->pixels(),
                &clRet
        );
    }

    void getScreenCoordinatesForBoundingBox(Point2D &min, Point2D &max, Mesh3D *mesh)
    {
        min.x = screenWidth;
        min.y = screenHeight;
        max.x = -1;
        max.y = -1;

        for (auto vertex : mesh->aabb.vertices) {
            Point2D screenPoint = Transforms::WorldToPoint(vertex, camera);
            min.x = std::min(min.x, screenPoint.x);
            min.y = std::min(min.y, screenPoint.y);
            max.x = std::max(max.x, screenPoint.x);
            max.y = std::max(max.y, screenPoint.y);
        }

        min.x = std::clamp(min.x, 0, screenWidth);
        min.y = std::clamp(min.y, 0, screenHeight);
        max.x = std::clamp(max.x, 0, screenWidth);
        max.y = std::clamp(max.y, 0, screenHeight);
    }

    void update()
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

    void setObject(Object3D *o) {
        this->object = o;
    }

    void setColor(Color c) {
        this->color = c;
    }

    Color getColor() {
        return this->color;
    }

    void executeKernelOpenCL()
    {
        clEnqueueWriteBuffer(
                clCommandQueue,
                opencl_buffer_video_shader,
                CL_TRUE,
                0,
                EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
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
                EngineBuffers::getInstance()->sizeBuffers * sizeof(bool),
                object->getStencilBuffer(),
                0,
                nullptr,
                nullptr
        );

        clSetKernelArg(kernel, 0, sizeof(int), &EngineSetup::get()->screenWidth);
        clSetKernelArg(kernel, 1, sizeof(int), &EngineSetup::get()->screenHeight);
        clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&opencl_buffer_video_screen);
        clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&opencl_buffer_video_shader);
        clSetKernelArg(kernel, 4, sizeof(cl_mem), (void *)&opencl_buffer_stencil);
        clSetKernelArg(kernel, 5, sizeof(float), &this->color.r);
        clSetKernelArg(kernel, 6, sizeof(float), &this->color.g);
        clSetKernelArg(kernel, 7, sizeof(float), &this->color.b);

        // Process the entire lists
        size_t global_item_size = EngineBuffers::getInstance()->sizeBuffers;
        // Divide work items into groups of 64
        size_t local_item_size = 16;

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
            opencl_buffer_video_screen,
            CL_TRUE,
            0,
            EngineBuffers::getInstance()->sizeBuffers * sizeof(Uint32),
            EngineBuffers::getInstance()->videoBuffer,
            0,
            nullptr,
            nullptr
        );

        this->debugKernel();
    }

    void setStep(float s) {
        this->step = s;
        this->counter.setStep(step);
        this->counter.setEnabled(true);
    }

    float step;
};
#endif //BRAKEDA3D_SHADERBLINK_H
