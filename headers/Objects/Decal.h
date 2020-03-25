
#ifndef BRAKEDA3D_DECAL_H
#define BRAKEDA3D_DECAL_H


#include "Object3D.h"
#include "Mesh3D.h"
#include "Cube3D.h"
#include "Sprite3D.h"

class Decal : public Mesh3D {
public:
    Vertex3D P; // Decal position
    Vertex3D N; // normal surface (up)
    Vertex3D T; // direction of decal (forward)
    Vertex3D B; // N x T (right)

    float w = 5;
    float h = 5;

    Cube3D *cube;
    Sprite3D *sprite;

    bool drawWireframe = false;

    Decal();

    void setupCube(float, float, float);
    void setupFromAxis();
    void setDimensions(float w, float h);

    void getTriangles(std::vector<Triangle*> &triangles, Camera3D *camera );

    float getSCoord(Vertex3D v);
    float getTCoord(Vertex3D v);

    Sprite3D *getSprite() const;

    void setSprite(Sprite3D *sprite);
};


#endif //BRAKEDA3D_DECAL_H
