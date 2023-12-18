#include "ShaderProjectiles.h"
#include "../../include/EngineBuffers.h"
#include "../../include/Brakeza3D.h"

ShaderProjectiles::ShaderProjectiles()
:
    ShaderOpenCL(true),
    image(Image(EngineSetup::get()->IMAGES_FOLDER + "cloud.png"))
{
}

void ShaderProjectiles::update()
{
    Shader::update();

    if (!isEnabled()) return;

    executeKernelOpenCL();
}

void ShaderProjectiles::executeKernelOpenCL()
{
}

void ShaderProjectiles::addLaser(int x1, int y1, int x2, int y2, Color color, float i, bool startCircle, bool endCircle)
{
    this->lasers.emplace_back(OCLaser {x1, y1, x2, y2, (int) color.r, (int) color.g, (int) color.b, i, startCircle, endCircle });
}

void ShaderProjectiles::addLaserFromRay(ProjectileRay *ray)
{
    auto camera = ComponentsManager::get()->getComponentCamera()->getCamera();
    auto color = ray->getColor();

    Point2D screenPoint = Transforms::WorldToPoint(ray->getPosition(), camera);

    Vertex3D end = ray->getPosition() + ray->getRay();
    Point2D middlePoint = Transforms::WorldToPoint(end, camera);

    this->addLaser(
        screenPoint.x, screenPoint.y,
        middlePoint.x, middlePoint.y,
        color,
        ray->getIntensity(),
        false,
        false
    );
}

void ShaderProjectiles::addProjectile(Vertex3D position, Color color, float i)
{
    Point2D screenPoint = Transforms::WorldToPoint(position, ComponentsManager::get()->getComponentCamera()->getCamera());

    this->projectiles.emplace_back(OCProjectile {
        screenPoint.x,
        screenPoint.y,
        (int) color.r,
        (int) color.g,
        (int) color.b,
        i
    });
}
