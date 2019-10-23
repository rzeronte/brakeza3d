
#include "Cube3D.h"
#include "../Render/Logging.h"
#include "../Render/Transforms.h"

Cube3D::Cube3D(float size) {
    this->scaleX = size;
    this->scaleY = size;
    this->scaleZ = size;

    makeVertices();
    makeTriangles();
    makePlanes();
}

Cube3D::Cube3D(float scaleX, float scaleY, float scaleZ)
{
    this->scaleX = scaleX;
    this->scaleY = scaleY;
    this->scaleZ = scaleZ;

    makeVertices();
    makeTriangles();
    makePlanes();
}

void Cube3D::update()
{
    makeVertices();
    makeTriangles();
    makePlanes();
}

void Cube3D::makeVertices()
{
    for (int i = 0; i < 36 ; i++) {
        this->vertices[i] = Vertex3D(
            (cubeVertexBuffer[i*3+0] * scaleX)/2,
            (cubeVertexBuffer[i*3+1] * scaleY)/2,
            (cubeVertexBuffer[i*3+2] * scaleZ)/2
        );
    }
}

void Cube3D::makeTriangles()
{
    for (int i = 0; i < 12 ; i++) {

        Triangle *t = new Triangle(
                vertices[i*3+0],
                vertices[i*3+1],
                vertices[i*3+2],
                this
        );
        t->drawed = true;

        modelTriangles.push_back( t );
    }
}

void Cube3D::makePlanes()
{
    planes[0] = Plane(
            Transforms::objectSpace(modelTriangles[1]->A, this),
            Transforms::objectSpace(modelTriangles[1]->B, this),
            Transforms::objectSpace(modelTriangles[1]->C, this)
    );
    planes[1] = Plane(
            Transforms::objectSpace(modelTriangles[2]->A, this),
            Transforms::objectSpace(modelTriangles[2]->B, this),
            Transforms::objectSpace(modelTriangles[2]->C, this)
    );
    planes[2] = Plane(
            Transforms::objectSpace(modelTriangles[4]->A, this),
            Transforms::objectSpace(modelTriangles[4]->B, this),
            Transforms::objectSpace(modelTriangles[4]->C, this)
    );
    planes[3] = Plane(
            Transforms::objectSpace(modelTriangles[6]->A, this),
            Transforms::objectSpace(modelTriangles[6]->B, this),
            Transforms::objectSpace(modelTriangles[6]->C, this)
    );
    planes[4] = Plane(
            Transforms::objectSpace(modelTriangles[8]->A, this),
            Transforms::objectSpace(modelTriangles[8]->B, this),
            Transforms::objectSpace(modelTriangles[8]->C, this)
    );
    planes[5] = Plane(
            Transforms::objectSpace(modelTriangles[10]->A, this),
            Transforms::objectSpace(modelTriangles[10]->B, this),
            Transforms::objectSpace(modelTriangles[10]->C, this)
    );
}

bool Cube3D::isPointInside(Vertex3D v)
{
    bool result = true;
    int plane_init = 0;
    int plane_end = 6;

    for(int i = plane_init; i < plane_end; i++) {
        if (planes[i].distance(v) >= EngineSetup::getInstance()->FRUSTUM_CLIPPING_DISTANCE) {
            return false;
        }
    }

    return result;
}

