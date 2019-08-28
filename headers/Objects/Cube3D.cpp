
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

void Cube3D::makeVertices()
{
    for (int i = 0; i < 36 ; i++) {
        this->vertices[i] = Vertex3D(
            (cubeVertexBuffer[i*3+0] * scaleX)/2,
            (cubeVertexBuffer[i*3+1] * scaleY)/2,
            (cubeVertexBuffer[i*3+2] * scaleZ)/2
        );
        this->vertices[i] = Transforms::objectSpace(this->vertices[i], this);
    }
}

void Cube3D::makeTriangles()
{
    for (int i = 0; i < 12 ; i++) {
        this->modelTriangles[i] = Triangle(
            vertices[i*3+0],
            vertices[i*3+1],
            vertices[i*3+2],
            this
        );

    }

    this->numTriangles = 12;
}

void Cube3D::makePlanes()
{
    planes[0] = Plane(modelTriangles[1].A, modelTriangles[1].B, modelTriangles[1].C);
    planes[1] = Plane(modelTriangles[2].A, modelTriangles[2].B, modelTriangles[2].C);
    planes[2] = Plane(modelTriangles[4].A, modelTriangles[4].B, modelTriangles[4].C);
    planes[3] = Plane(modelTriangles[6].A, modelTriangles[6].B, modelTriangles[6].C);
    planes[4] = Plane(modelTriangles[8].A, modelTriangles[8].B, modelTriangles[8].C);
    planes[5] = Plane(modelTriangles[10].A, modelTriangles[10].B, modelTriangles[10].C);
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

