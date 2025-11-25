
#include "../../include/Render/Triangle3D.h"
#include "../../include/Render/Transforms.h"
#include "../../include/Render/Maths.h"

Triangle::Triangle()
:
    enableLights(false),
    parent(nullptr),
    clipped(false)
{
}

Triangle::Triangle(Vertex3D A, Vertex3D B, Vertex3D C, Object3D *parent)
:
    parent(nullptr),
    clipped(false)
{
    this->A = A;
    this->B = B;
    this->C = C;

    this->parent = parent;
}

void Triangle::updateObjectSpace()
{
    Transforms::objectSpace(Ao, A, parent);
    Transforms::objectSpace(Bo, B, parent);
    Transforms::objectSpace(Co, C, parent);
}

void Triangle::updateNormal()
{
    this->normal = Vector3D(this->Ao, this->Bo).getComponent() % Vector3D(this->Ao, this->Co).getComponent();
}

Vertex3D Triangle::getNormal() const
{
    return this->normal;
}

bool Triangle::isPointInside(Vertex3D v) const {
    return Maths::PointInTriangle(v, Ao, Bo, Co);
}

bool Triangle::isEnableLights() const {
    return this->enableLights;
}

void Triangle::setEnableLights(bool enableLights) {
    this->enableLights = enableLights;
}
