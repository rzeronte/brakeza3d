
#ifndef BRAKEDA3D_DECAL_H
#define BRAKEDA3D_DECAL_H


#include "../headers/Objects/Object3D.h"
#include "../headers/Objects/Mesh3D.h"

class Decal : public Mesh3D {
public:
    Vertex3D P; // Decal position
    Vertex3D N; // normal surface (up)
    Vertex3D T; // direction of decal (forward)
    Vertex3D B; // N x T (right)

    float w = 20;
    float h = 20;

    Frustum *frustum;

    Texture *texture;

    Decal(Vertex3D position);

    void updateFrustum();
    void setupFromAxis();
    void setDimensions(float w, float h);

    void getTriangles(Triangle *soupTriangles, int numSoupTriangles, Camera3D *camera );

    float getSCoord(Vertex3D v);
    float getTCoord(Vertex3D v);
};


#endif //BRAKEDA3D_DECAL_H
