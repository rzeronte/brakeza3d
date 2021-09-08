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

public:

    enum EmptyStrategies {
        EMPTY = -1,
        RAY_INTERSECTION = 0,
        CONTAIN_TRIANGLES = 1,
        IMAGE_FILE = 2,
    };

    Grid3D(std::vector<Triangle*> *triangles, const AABB3D &bounds, int sizeX, int sizeY, int sizeZ, EmptyStrategies strategy);
    AABB3D bounds;
    std::vector<CubeGrid3D*> boxes;

    void applyCheckCellEmptyStrategy();

    bool isEmpty(CubeGrid3D &cube, std::vector<Triangle*> &triangles);
    void fillEmptiesWithAABBvsTriangles(std::vector<Triangle*> &triangles);
    void fillEmptiesWithVector3DvsTriangles(Vertex3D ray, std::vector<Triangle*> &triangles);
    void fillEmptiesWithImageData(std::string filename, int fixedY);

    void setRayIntersectionDirection(Vertex3D rayIntersectionDirection);

    void setImageFilename(const std::string &imageFilename);
    void setFixedYImageData(int fixedYImageData);

    CubeGrid3D *getFromPosition(int x, int y, int z);
    Vertex3D getClosestPoint(Vertex3D v, std::vector<Vertex3D> path, int &indexVertex);

    int numberCubesX;
    int numberCubesY;
    int numberCubesZ;
private:
    std::vector<Triangle*> *triangles;
    EmptyStrategies strategy;
    Vertex3D rayIntersectionDirection;
    std::string imageFilename;
    int fixedYImageData = 0;
};


#endif //BRAKEDA3D_GRID3D_H
