#include "Collider.h"
#include "../headers/Objects/Vector3D.h"
#include "../headers/Render/EngineSetup.h"

Collider::Collider()
{
    this->movement = Vector3D(Vertex3D(0, 0, 0), Vertex3D(0, 0, 0));
    this->gravity = Vertex3D(0, 0, 0);
}
