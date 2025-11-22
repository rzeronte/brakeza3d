
#ifndef BRAKEDA3D_DECAL_H
#define BRAKEDA3D_DECAL_H


#include "Object3D.h"
#include "Mesh3D.h"
#include "Cube3D.h"
#include "Image3DAnimation.h"

class Decal : public Mesh3D {
public:
    Vertex3D P; // Decal position
    Vertex3D N; // normal surface (up)
    Vertex3D T; // direction of decal (forward)
    Vertex3D B; // N x T (right)

    float w = 5;
    float h = 5;

    Cube3D *cube;
    Image3DAnimation *sprite;

    bool drawWireframe = false;

    Decal();

    void setupCube(float, float, float);

    void setupFromAxis();

    void setDimensions(float w, float h);

    void getTriangles(std::vector<Triangle *> &triangles, Camera3D *camera);

    [[nodiscard]] float getSCoord(Vertex3D v) const;

    [[nodiscard]] float getTCoord(Vertex3D v) const;

    [[nodiscard]] Image3DAnimation *getSprite() const;

    void setSprite(Image3DAnimation *sprite);

    void onUpdate() override;
};


#endif //BRAKEDA3D_DECAL_H
