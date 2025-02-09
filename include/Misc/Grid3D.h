//
// Created by darkhead on 4/3/21.
//

#ifndef BRAKEDA3D_GRID3D_H
#define BRAKEDA3D_GRID3D_H


#include "../Render/AABB3D.h"
#include "../Objects/Triangle3D.h"

struct CubeGrid3D {
    AABB3D box;
    int posX;
    int posY;
    int posZ;
    bool passed = true;
};

enum TestsType {
    EMPTY = -1,
    RAY_INTERSECTION = 0,
    CONTAIN_TRIANGLES = 1,
    IMAGE_FILE = 2,
};

class Grid3D {
private:

    int numberCubesX;
    int numberCubesY;
    int numberCubesZ;

    AABB3D bounds;
    std::vector<CubeGrid3D *> boxes;
public:

    Grid3D(
        const AABB3D &bounds,
        int sizeX,
        int sizeY,
        int sizeZ
    );

    bool isEmpty(CubeGrid3D &cube, std::vector<Triangle *> &triangles);

    CubeGrid3D *getCubeFromPosition(int x, int y, int z);

    Vertex3D getClosestPoint(Vertex3D v, std::vector<Vertex3D> path, int &indexVertex);

    void fillTestWithImageData(const std::string& filename, int fixedY);

    void fillEmptiesWithAABBvsTriangles(std::vector<Triangle *> &triangles);

    void fillEmptiesWithVector3DvsTriangles(Vertex3D dir, std::vector<Triangle *> &triangles);

    int getNumberCubesX() const;

    int getNumberCubesY() const;

    int getNumberCubesZ() const;

    const std::vector<CubeGrid3D *> &getBoxes() const;
};


#endif //BRAKEDA3D_GRID3D_H
