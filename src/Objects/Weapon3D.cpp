
#include "../../headers/Objects/Weapon3D.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Render/Engine.h"
#include "../../headers/Render/Maths.h"

extern Engine* brakeza3D;

void Weapon3D::setup(float farDistFrom, float XOffsetFrom, float YOffsetFrom)
{
    this->farDist = farDistFrom;
    this->XOffset = XOffsetFrom;
    this->YOffset = YOffsetFrom;
}

void Weapon3D::HeadBob()
{
    // head bob
    if (abs(brakeza3D->camera->velocity.getComponent().getModule()) > 0 ) {
        this->getPosition()->y+= sin( Maths::degreesToRadians(this->head_vertical) ) / 25;
        this->head_vertical += 25;
        if (this->head_vertical > 360) {
            this->head_vertical = 0;
        }
    }
}

void Weapon3D::updateWeaponPosition(Camera3D* camera)
{
    Vertex3D direction = camera->getRotation().getTranspose() * EngineSetup::getInstance()->forward;
    Vertex3D up        = camera->getRotation().getTranspose() * EngineSetup::getInstance()->up;
    Vertex3D right     = camera->getRotation().getTranspose() * EngineSetup::getInstance()->right;

    this->setRotation(camera->getRotation().getTranspose());

    Vertex3D pos = *camera->getPosition();
    pos.x = pos.x + direction.x * farDist + (up.x * YOffset) - (right.x * XOffset);
    pos.y = pos.y + direction.y * farDist + (up.y * YOffset) - (right.y * XOffset);
    pos.z = pos.z + direction.z * farDist + (up.z * YOffset) - (right.z * XOffset);

    this->setPosition(pos);
    this->HeadBob();
}