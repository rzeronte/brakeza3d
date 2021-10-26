
#include "../../include/Render/Billboard.h"
#include "../../include/Render/Transforms.h"
#include "../../include/EngineSetup.h"

Billboard::Billboard() {
    this->texture = new Texture();

    EngineSetup *setup = EngineSetup::getInstance();
    this->width = setup->BILLBOARD_WIDTH_DEFAULT;
    this->height = setup->BILLBOARD_HEIGHT_DEFAULT;

    this->T1 = Triangle();
    this->T2 = Triangle();
}

void Billboard::updateUnconstrainedQuad(Object3D *o, Vertex3D &U, Vertex3D &R) {
    Vertex3D X;
    X.x = (width / 2) * R.x;
    X.y = (width / 2) * R.y;
    X.z = (width / 2) * R.z;

    Vertex3D Y;
    Y.x = (height / 2) * U.x;
    Y.y = (height / 2) * U.y;
    Y.z = (height / 2) * U.z;

    Q1.x = o->getPosition().x + X.x + Y.x;
    Q1.y = o->getPosition().y + X.y + Y.y;
    Q1.z = o->getPosition().z + X.z + Y.z;

    Q2.x = o->getPosition().x - X.x + Y.x;
    Q2.y = o->getPosition().y - X.y + Y.y;
    Q2.z = o->getPosition().z - X.z + Y.z;

    Q3.x = o->getPosition().x - X.x - Y.x;
    Q3.y = o->getPosition().y - X.y - Y.y;
    Q3.z = o->getPosition().z - X.z - Y.z;

    Q4.x = o->getPosition().x + X.x - Y.x;
    Q4.y = o->getPosition().y + X.y - Y.y;
    Q4.z = o->getPosition().z + X.z - Y.z;

    Q1 = Transforms::objectToLocal(Q1, o);
    Q2 = Transforms::objectToLocal(Q2, o);
    Q3 = Transforms::objectToLocal(Q3, o);
    Q4 = Transforms::objectToLocal(Q4, o);

    Q1.u = 1.0f;
    Q1.v = 1.0f;
    Q2.u = 0.0001;
    Q2.v = 1.0f;
    Q3.u = 0.0001;
    Q3.v = 0.0001;
    Q4.u = 1.0f;
    Q4.v = 0.0001;

    this->T1.A = Q3;
    this->T1.B = Q2;
    this->T1.C = Q1;
    this->T1.parent = o;

    this->T2.A = Q4;
    this->T2.B = Q3;
    this->T2.C = Q1;
    this->T2.parent = o;

    //T1 = Triangle(Q3, Q2, Q1, o);
    //T2 = Triangle(Q4, Q3, Q1, o);
}

void Billboard::loadTexture(const std::string &&fileName) {
    this->texture->loadTGA(fileName.c_str(), 1);
    setTrianglesTexture(this->texture);
}

void Billboard::setTrianglesTexture(Texture *t) {
    this->T1.setTexture(t);
    this->T2.setTexture(t);
}

void Billboard::reassignTexture() {
    setTrianglesTexture(this->texture);

}

void Billboard::setDimensions(float w, float h) {
    this->width = w;
    this->height = h;
}