//
// Created by eduardo on 27/3/22.
//

#include "../../include/Game/ShaderShield.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Render/Transforms.h"
#include "../../include/ComponentsManager.h"
#include "../../include/Brakeza3D.h"

ShaderShield::ShaderShield(float radius, float speed)
{
    this->radius = radius;
    this->speed = speed;
    this->c = Color::cyan();
}


void ShaderShield::onUpdate(Vertex3D position) {
    Shader::update();

    Vertex3D r;
    Transforms::cameraSpace(r, position, ComponentsManager::get()->getComponentCamera()->getCamera());
    r = Transforms::PerspectiveNDCSpace(r, ComponentsManager::get()->getComponentCamera()->getCamera()->frustum);

    Point2D DP;
    Transforms::screenSpace(DP, r);

    for (int y = 0; y < this->h; y++) {
        for (int x = 0; x < this->w; x++) {

            Point2D uv(x, y);
            Point2D center(DP.x, DP.y);

            Color color;

            float radius = 50.5;

            color = paintCircle(uv,center, radius, 5.1);
            EngineBuffers::getInstance()->setVideoBuffer(x, y, color.getColor());
        }
    }
}

Color ShaderShield::paintCircle(Point2D uv, Point2D center, float rad, float width)
{
    Point2D diff = center - uv;
    float len = diff.getLength();

    len += variation(diff, Point2D(0.0, 10000.0), 55500.0, 10000.0);
    len -= variation(diff, Point2D(10000.0, 0.0), 55500.0, 10000.0);

    float circle = smoothstep(rad-width, rad, len) - smoothstep(rad, rad+width, len);

    return Color(circle*255, circle*255, circle*255);
}


float ShaderShield::variation(Point2D v1, Point2D v2, float strength, float speed) {
    return (float) sin(
            v1.getNormalize() * v2.getNormalize() * strength + Brakeza3D::get()->getDeltaTime()*1000 * speed
    );
}

float ShaderShield::smoothstep(float edge0, float edge1, float x)
{
    // Scale, bias and saturate x to 0..1 range
    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}

void ShaderShield::drawCircle(int x0, int y0, int radius)
{
    float x = radius;
    float y = 0;
    int err = 0;
    auto buffer = EngineBuffers::getInstance();
    offsetY = cos(executionTime * radius * speed);

    Color newColor = Tools::mixColor(c, Color(buffer->getVideoBuffer(x, y)), abs(offsetY));

    while (x >= y) {
        buffer->setVideoBuffer(std::fmin(x0 + x, w), std::fmin(y0 + y, h), newColor.getColor());
        buffer->setVideoBuffer(std::fmin(x0 + y, w), std::fmin(y0 + x, h), newColor.getColor());
        buffer->setVideoBuffer(std::fmax(x0 - y, 0), std::fmin(y0 + x, h), newColor.getColor());
        buffer->setVideoBuffer(std::fmax(x0 - x, 0), std::fmin(y0 + y, h), newColor.getColor());
        buffer->setVideoBuffer(std::fmax(x0 - x, 0), std::fmax(y0 - y, 0), newColor.getColor());
        buffer->setVideoBuffer(std::fmax(x0 - y, 0), std::fmax(y0 - x, 0), newColor.getColor());
        buffer->setVideoBuffer(std::fmin(x0 + y, w), std::fmax(y0 - x, 0), newColor.getColor());
        buffer->setVideoBuffer(std::fmin(x0 + x, w), std::fmax(y0 - y, 0), newColor.getColor());

        if (err <= 0) {
            y += 1 - offsetY + 0.3;
            err += 2*y + 1;
        }

        if (err > 0) {
            x -= 1 ;
            err -= 2*x + 1;
        }
    }
}

