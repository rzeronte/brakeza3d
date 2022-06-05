//
// Created by eduardo on 5/3/22.
//

#ifndef BRAKEDA3D_SHADERBLINK_H
#define BRAKEDA3D_SHADERBLINK_H
#include "../Render/Shader.h"
#include "../Misc/Color.h"
#include "../Misc/Tools.h"
#include "../EngineBuffers.h"
#include "../Render/Transforms.h"
#include "../Render/Drawable.h"
#include "../Render/Logging.h"

#define DEFAULT_BLINK_SECONDS 1
class ShaderBlink: public Shader {
    bool isBlinking = false;
    Camera3D *camera;
public:
    ShaderBlink(Camera3D *camera) {
        this->screenHeight = EngineSetup::get()->screenHeight;
        this->screenWidth = EngineSetup::get()->screenWidth;
        this->object = nullptr;
        this->color = Color::green();
        setStep(DEFAULT_BLINK_SECONDS);
        setPhaseRender(EngineSetup::ShadersPhaseRender::POSTUPDATE);
        this->counter.setStep(step);
        this->counter.setEnabled(true);
        this->camera = camera;
    }

    ShaderBlink(Camera3D *camera, Object3D *o) {
        this->object = o;
        this->screenHeight = EngineSetup::get()->screenHeight;
        this->screenWidth = EngineSetup::get()->screenWidth;
        this->color = Color::green();
        setStep(DEFAULT_BLINK_SECONDS);
        setPhaseRender(EngineSetup::ShadersPhaseRender::POSTUPDATE);
        this->camera = camera;
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

            mesh->updateBoundingBox();

            Point2D screenMinPoint;
            Point2D screenMaxPoint;
            this->getScreenCoordinatesForBoundingBox(screenMinPoint, screenMaxPoint, mesh);

            auto buffer = EngineBuffers::getInstance();
            for (int y = screenMinPoint.y; y < screenMaxPoint.y - 1 ; y++) {
                for (int x = screenMinPoint.x; x < screenMaxPoint.x - 1 ; x++) {
                    if (this->object->getStencilBufferValue(x, y)) {
                        buffer->setVideoBuffer(x, y, this->color.getColor());
                    }
                }
            }
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

    void setStep(float s) {
        this->step = s;
        this->counter.setStep(step);
        this->counter.setEnabled(true);
    }

    float step;
private:
    int screenWidth;
    int screenHeight;

    Object3D* object;
    Color color;
    Counter counter;
};
#endif //BRAKEDA3D_SHADERBLINK_H
