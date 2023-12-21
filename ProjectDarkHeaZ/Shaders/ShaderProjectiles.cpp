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
    for(auto l: lasers) {
        ComponentsManager::get()->getComponentWindow()->getShaderOGLLine()->render(
            Point2D(l.from),
            Point2D(l.to),
            Color(l.color.r, l.color.g, l.color.b),
            l.intensity
        );
    }

    this->lasers.clear();
    this->projectiles.clear();
}

void ShaderProjectiles::addLaser(glm::vec2 from, glm::vec2 to, glm::vec3 color, float i, bool startCircle, bool endCircle)
{
    this->lasers.emplace_back(OCLaser {from, to, color, i, startCircle, endCircle });
}

void ShaderProjectiles::addLaserFromRay(ProjectileRay *ray)
{
    auto color = ray->getColor();

    Point2D screenPoint = Transforms::WorldToPoint(ray->getPosition());

    Vertex3D end = ray->getPosition() + ray->getRay();
    Point2D middlePoint = Transforms::WorldToPoint(end);

    this->addLaser(
        glm::vec2(screenPoint.x, screenPoint.y),
        glm::vec2(middlePoint.x, middlePoint.y),
        color.toGLM(),
        ray->getIntensity(),
        false,
        false
    );
}

void ShaderProjectiles::addProjectile(Vertex3D position, Color color, float i)
{
    Point2D screenPoint = Transforms::WorldToPoint(position);

    this->projectiles.emplace_back(OCProjectile {
        screenPoint.x,
        screenPoint.y,
        (int) color.r,
        (int) color.g,
        (int) color.b,
        i
    });
}
