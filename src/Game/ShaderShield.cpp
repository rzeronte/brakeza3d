//
// Created by eduardo on 27/3/22.
//

#include "ShaderShield.h"
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
    if (Tools::isPixelInWindow(DP.x, DP.y)) {
        for (int i = 0; i < radius; i+=1) {
            drawCircle(DP.x, DP.y, int(radius)-i);
        }
    }
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

