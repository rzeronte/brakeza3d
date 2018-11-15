
#include "../../headers/Objects/Weapon3D.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Transforms.h"
#include "../../headers/Render/Drawable.h"


void Weapon3D::setWeaponPosition(Camera3D *cam)
{
    Point2D p(300, 360);
    Vertex3D v = Transforms::Point2DToWorld(p, cam);

    *this->getPosition() = v;
}
