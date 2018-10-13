
#include "../headers/Billboard.h"
#include "../headers/Render.h"
#include "../headers/Tools.h"
#include "../headers/Drawable.h"
#include "../headers/Triangle.h"
#include "../headers/Core/Logging.h"
#include "../headers/M3.h"

Billboard::Billboard()
{
    this->texture = new Texture();
}

void Billboard::updateUnconstrainedQuad(float w, float h, Object3D *o, Vertex U, Vertex R)
{
    this->width  = w;
    this->height = h;

    Vertex X;
    X.x = (width/2) * R.x;
    X.y = (width/2) * R.y;
    X.z = (width/2) * R.z;

    Vertex Y;
    Y.x = (height/2) * U.x;
    Y.y = (height/2) * U.y;
    Y.z = (height/2) * U.z;

    Q1.x = o->position.x + X.x + Y.x;
    Q1.y = o->position.y + X.y + Y.y;
    Q1.z = o->position.z + X.z + Y.z;

    Q2.x = o->position.x - X.x + Y.x;
    Q2.y = o->position.y - X.y + Y.y;
    Q2.z = o->position.z - X.z + Y.z;

    Q3.x = o->position.x - X.x - Y.x;
    Q3.y = o->position.y - X.y - Y.y;
    Q3.z = o->position.z - X.z - Y.z;

    Q4.x = o->position.x + X.x - Y.x;
    Q4.y = o->position.y + X.y - Y.y;
    Q4.z = o->position.z + X.z - Y.z;

    Q1 = Render::objectToLocal(Q1, o);
    Q2 = Render::objectToLocal(Q2, o);
    Q3 = Render::objectToLocal(Q3, o);
    Q4 = Render::objectToLocal(Q4, o);

    Q1.u = 1; Q1.v = 1;
    Q2.u = 0; Q2.v = 1;
    Q3.u = 0; Q3.v = 0;
    Q4.u = 1; Q4.v = 0;

    T1 = Triangle(Q3, Q2, Q1, o);
    T2 = Triangle(Q3, Q1, Q4, o);

}

void Billboard::loadTexture(std::string fileName) {
    this->texture->loadTGA( fileName.c_str() );
    setTrianglesTexture(this->texture);
}

bool Billboard::isDrawable() const {
    return drawable;
}

void Billboard::setDrawable(bool drawable) {
    Billboard::drawable = drawable;
}

void Billboard::setTrianglesTexture(Texture *t)
{
    this->T1.setTexture(t);
    this->T2.setTexture(t);
}