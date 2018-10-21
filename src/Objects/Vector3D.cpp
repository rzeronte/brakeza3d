

#include <SDL_quit.h>
#include <SDL_surface.h>
#include "../../headers/Vector3D.h"
#include "../../headers/Point2D.h"
#include "../../headers/Tools.h"
#include "../../headers/Line2D.h"
#include "../../headers/Color.h"
#include "../../headers/Triangle.h"
#include "../../headers/EngineBuffers.h"
#include <stdio.h>

class string;

Vector3D::Vector3D()
{

}

Vector3D::Vector3D(Vertex A, Vertex B)
{
    this->vertex1 = A;
    this->vertex2 = B;
}

/**
 * Obtiene el vector componente
 *
 * En la práctica equivale a situar punto inicial del vector en el eje 0, 0, 0 manteniendo el resto de información
 * @return
 */
Vertex Vector3D::getComponent()
{
    Vertex componente = Vertex(
        this->vertex2.x - this->vertex1.x,
        this->vertex2.y - this->vertex1.y,
        this->vertex2.z - this->vertex1.z
    );

    return componente;
}

Vertex Vector3D::getUnitVector() {
    Vertex c = this->getComponent();

    float modulo_v = sqrt( (c.x*c.x) + (c.y*c.y) + (c.z*c.z));
    Vertex final_vertex;

    final_vertex.x = c.x / modulo_v;
    final_vertex.y = c.y / modulo_v;
    final_vertex.z = c.z / modulo_v;

    return final_vertex;
}


Vertex Vector3D::consoleInfo(std::string label) {
    printf("%s: ", label.c_str());
    this->vertex1.consoleInfo("v1", false);
    this->vertex2.consoleInfo("v2", false);
    printf("\r\n");
}
