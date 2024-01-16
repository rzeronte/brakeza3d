#include "FXLaser.h"
#include "../../include/LUAManager.h"
#include "../../include/Brakeza3D.h"

FXLaser::FXLaser(Image *mask)
:
    mask(mask),
    FXEffectOpenGL(true)
{
}

void FXLaser::update()
{
    FXEffectBase::update();

    if (!isEnabled()) return;

    auto shader = ComponentsManager::get()->getComponentGame()->getShaderOGLLineLaser();
    auto fb = ComponentsManager::get()->getComponentWindow()->getForegroundFramebuffer();

    for (auto l: lasers) {
        shader->render(
            mask->getOGLTextureID(),
            Point2D(l.from),
            Point2D(l.to),
            normalize(l.to - l.from),
            Color(l.color.r, l.color.g, l.color.b),
            l.speed,
            l.weight,
            l.intensity,
            fb
        );
    }

    this->lasers.clear();
}

void FXLaser::addLaser(glm::vec2 from, glm::vec2 to, glm::vec3 color, float intensity, float weight, float maskSpeed)
{
    this->lasers.emplace_back(OCLaser {from, to, color, intensity, weight, maskSpeed});
}

void FXLaser::addLaserFromRay(ProjectileRay *ray)
{
    auto color = ray->getColor();

    Point2D screenPoint = Transforms::WorldToPoint(ray->getPosition());
    Vertex3D end = ray->getPosition() + ray->getRay();
    Point2D middlePoint = Transforms::WorldToPoint(end);

    this->addLaser(
        screenPoint.toGLM(),
        middlePoint.toGLM(),
        color.toGLM(),
        2.5f,
        ray->getIntensity(),
        0.5f
    );
}