//
// Created by darkhead on 4/3/21.
//

#ifndef BRAKEDA3D_GRID3D_H
#define BRAKEDA3D_GRID3D_H


#include "../Render/AABB3D.h"
#include "../Objects/Triangle3D.h"

struct CubeGrid3D {
    AABB3D *box;
    int posX;
    int posY;
    int posZ;
    bool is_empty = true;
};

class Grid3D {

private:
public:
    Grid3D(std::vector<Triangle*> &triangles, const AABB3D &bounds, int sizeX, int sizeY, int sizeZ);
    AABB3D bounds;
    std::vector<CubeGrid3D*> boxes;

    bool isEmpty(CubeGrid3D &cube, std::vector<Triangle*> &triangles);
    void fillEmptiesWithAABBvsTriangles(std::vector<Triangle*> &triangles);
    void fillEmptiesWithVector3DvsTriangles(Vertex3D ray, std::vector<Triangle*> &triangles);

    bool saveToFile(std::string filename);
private:
    int numberCubesX;
    int numberCubesY;
    int numberCubesZ;
};


#endif //BRAKEDA3D_GRID3D_H
