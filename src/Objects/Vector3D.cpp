
#include <SDL2/SDL_quit.h>
#include "../../include/Objects/Vector3D.h"

class string;

Vector3D::Vector3D() = default;

Vector3D::Vector3D(Vertex3D &A, Vertex3D &B)
{
    this->vertex1 = A;
    this->vertex2 = B;
}

Vertex3D Vector3D::getComponent() const {
    return Vertex3D(
        this->vertex2.x - this->vertex1.x,
        this->vertex2.y - this->vertex1.y,
        this->vertex2.z - this->vertex1.z
    );
}

Vertex3D Vector3D::normal() const {
    Vertex3D c = this->getComponent();

    float modulo = sqrt((c.x * c.x) + (c.y * c.y) + (c.z * c.z));

    return Vertex3D(
        c.x / modulo,
        c.y / modulo,
        c.z / modulo
    );
}

Vertex3D Vector3D::origin() const {
    return this->vertex1;
}

Vertex3D Vector3D::end() const {
    return this->vertex2;
}

Vertex3D Vector3D::end(float t) const {
    return origin() + normal().getScaled(t);
}

Vertex3D Vector3D::middlePoint() const {
    return Vertex3D((vertex1.x + vertex2.x) / 2, (vertex1.y + vertex2.y) / 2, (vertex1.z + vertex2.z) / 2);
}

void Vector3D::clear() {
    this->vertex1 = Vertex3D::zero();
    this->vertex2 = Vertex3D::zero();
}
