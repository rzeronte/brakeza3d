
#include "../../headers/Objects/LightPoint3D.h"
#include "../../headers/Render/Billboard.h"
#include "../../headers/EngineBuffers.h"
#include "../../headers/Render/Maths.h"

LightPoint3D::LightPoint3D()
{
    cam = new Camera3D();
    //cam->setPosition(*this->getPosition());
    //cam->setRotation(*this->getRotation());

    this->sizeBuffer = EngineSetup::getInstance()->screenWidth * EngineSetup::getInstance()->screenHeight;
    shadowMappingBuffer = new float[sizeBuffer];
}

void LightPoint3D::syncFrustum()
{
    //this->cam->setPosition(*this->getPosition());
    //this->cam->setRotation(*this->getRotation());

    //this->cam->frustum->position  = *this->cam->getPosition();
    this->cam->frustum->direction = this->cam->AxisForward();
    this->cam->frustum->up        = this->cam->AxisUp();
    this->cam->frustum->right     = this->cam->AxisRight();

    this->cam->frustum->updateCenters();
    this->cam->frustum->updatePoints();
    this->cam->frustum->updatePlanes();
}

void LightPoint3D::clearShadowMappingBuffer()
{
    std::fill(shadowMappingBuffer, shadowMappingBuffer + sizeBuffer, NULL);
}

float LightPoint3D::getShadowMappingBuffer(int x, int y)
{
    return shadowMappingBuffer[(y * EngineSetup::getInstance()->screenWidth ) + x ];
}

float LightPoint3D::setShadowMappingBuffer(int x, int y, float value)
{
    shadowMappingBuffer[(y * EngineSetup::getInstance()->screenWidth ) + x ] = value;
}

void LightPoint3D::setColor(int r, int g, int b)
{
    Uint32 color = ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);

    LightPoint3D::color = color;

    ImVec4 c = ImColor(r, g, b, 255);

    this->imgui_color = c;
}

Uint32 LightPoint3D::mixColor(Uint32 c, Vertex3D Q)
{
    float distance = Maths::distanceBetweenVertices(this->getPosition(), Q);

    Vertex3D P = this->getPosition();
    Vertex3D R = this->AxisForward();

    Vector3D L = Vector3D(P, Q);
    Vertex3D Lv = L.normal();

    const float min = R * Lv;

    float max = fmaxf(min, 0);
    float pow = powf(max, this->p);

    float intensity = pow / (this->kc + this->kl*distance + this->kq * (distance * distance));

    return Tools::mixColor(c, this->color, intensity);

}