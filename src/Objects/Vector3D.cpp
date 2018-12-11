

#include <SDL_quit.h>
#include <SDL_surface.h>
#include "../../headers/Objects/Vector3D.h"
#include "../../headers/Objects/Point2D.h"
#include "../../headers/Render/Tools.h"
#include "../../headers/Objects/Line2D.h"
#include "../../headers/Render/Color.h"
#include "../../headers/Objects/Triangle3D.h"
#include "../../headers/Render/EngineBuffers.h"
#include <stdio.h>

class string;

Vector3D::Vector3D()
{

}

Vector3D::Vector3D(Vertex3D A, Vertex3D B)
{
    this->vertex1 = A;
    this->vertex2 = B;
}

Vertex3D Vector3D::getComponent()
{
    Vertex3D componente = Vertex3D(
        this->vertex2.x - this->vertex1.x,
        this->vertex2.y - this->vertex1.y,
        this->vertex2.z - this->vertex1.z
    );

    return componente;
}

Vertex3D Vector3D::getUnitVector() {
    Vertex3D c = this->getComponent();

    float modulo_v = sqrt( (c.x*c.x) + (c.y*c.y) + (c.z*c.z));
    Vertex3D final_vertex;

    final_vertex.x = c.x / modulo_v;
    final_vertex.y = c.y / modulo_v;
    final_vertex.z = c.z / modulo_v;

    return final_vertex;
}


Vertex3D Vector3D::consoleInfo(std::string label) {
    printf("%s: ", label.c_str());
    this->vertex1.consoleInfo("v1", false);
    this->vertex2.consoleInfo("v2", false);
    printf("\r\n");
}
