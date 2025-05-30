
#include "../../include/Objects/Cube3D.h"
#include "../../include/Render/Logging.h"
#include "../../include/Render/Transforms.h"

Cube3D::Cube3D(float size) {
    this->scaleX = size;
    this->scaleY = size;
    this->scaleZ = size;

    makeVertices();
    makeTriangles();
    makePlanes();
}

Cube3D::Cube3D(float scaleX, float scaleY, float scaleZ) {
    this->scaleX = scaleX;
    this->scaleY = scaleY;
    this->scaleZ = scaleZ;

    makeVertices();
    makeTriangles();
    makePlanes();
}

void Cube3D::updateGeometry() {
    makeVertices();
    makeTriangles();
    makePlanes();
}

void Cube3D::makeVertices() {
    for (int i = 0; i < 36; i++) {
        this->vertices[i] = Vertex3D(
            (cubeVertexBuffer[i * 3 + 0] * scaleX) / 2,
            (cubeVertexBuffer[i * 3 + 1] * scaleY) / 2,
            (cubeVertexBuffer[i * 3 + 2] * scaleZ) / 2
        );
    }
}

void Cube3D::makeTriangles()
{
    meshes.resize(1);

    for (int i = 0; i < 12; i++) {
        Triangle *t = new Triangle(
            vertices[i * 3 + 0],
            vertices[i * 3 + 1],
            vertices[i * 3 + 2],
            this
        );
        meshes[0].modelTriangles.push_back(t);
    }
}

void Cube3D::makePlanes() {
    Vertex3D p01, p02, p03;
    Transforms::objectSpace(p01, meshes[0].modelTriangles[1]->A, this);
    Transforms::objectSpace(p02, meshes[0].modelTriangles[1]->B, this);
    Transforms::objectSpace(p03, meshes[0].modelTriangles[1]->C, this);
    planes[0] = Plane(p01, p02, p03);

    Vertex3D p11, p12, p13;
    Transforms::objectSpace(p11, meshes[0].modelTriangles[2]->A, this);
    Transforms::objectSpace(p12, meshes[0].modelTriangles[2]->B, this);
    Transforms::objectSpace(p13, meshes[0].modelTriangles[2]->C, this);
    planes[1] = Plane(p11, p12, p13);

    Vertex3D p21, p22, p23;
    Transforms::objectSpace(p21, meshes[0].modelTriangles[4]->A, this);
    Transforms::objectSpace(p22, meshes[0].modelTriangles[4]->B, this);
    Transforms::objectSpace(p23, meshes[0].modelTriangles[4]->C, this);
    planes[2] = Plane(p21, p22, p23);

    Vertex3D p31, p32, p33;
    Transforms::objectSpace(p31, meshes[0].modelTriangles[6]->A, this);
    Transforms::objectSpace(p32, meshes[0].modelTriangles[6]->B, this);
    Transforms::objectSpace(p33, meshes[0].modelTriangles[6]->C, this);
    planes[3] = Plane(p31, p32, p33);

    Vertex3D p41, p42, p43;
    Transforms::objectSpace(p41, meshes[0].modelTriangles[8]->A, this);
    Transforms::objectSpace(p42, meshes[0].modelTriangles[8]->B, this);
    Transforms::objectSpace(p43, meshes[0].modelTriangles[8]->C, this);
    planes[4] = Plane(p41, p42, p43);

    Vertex3D p51, p52, p53;
    Transforms::objectSpace(p51, meshes[0].modelTriangles[10]->A, this);
    Transforms::objectSpace(p52, meshes[0].modelTriangles[10]->B, this);
    Transforms::objectSpace(p53, meshes[0].modelTriangles[10]->C, this);
    planes[5] = Plane(p51, p52, p53);
}

bool Cube3D::isPointInside(Vertex3D v) {
    bool result = true;
    int plane_init = 0;
    int plane_end = 6;

    for (int i = plane_init; i < plane_end; i++) {
        if (planes[i].distance(v) >= EngineSetup::get()->FRUSTUM_CLIPPING_DISTANCE) {
            return false;
        }
    }

    return result;
}

void Cube3D::onUpdate() {
}

void Cube3D::postUpdate() {
}
