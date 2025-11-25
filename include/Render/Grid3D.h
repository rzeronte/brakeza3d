//
// Created by darkhead on 4/3/21.
//

#ifndef BRAKEDA3D_GRID3D_H
#define BRAKEDA3D_GRID3D_H


#include "AABB3D.h"
#include "Triangle3D.h"
#include "../Misc/PathFinding.h"

struct CubeGrid3D {
    AABB3D box;
    int posX;
    int posY;
    int posZ;
    bool passed = true;
};

class Grid3D {

    int numberCubesX;
    int numberCubesY;
    int numberCubesZ;

    AABB3D bounds;
    std::vector<CubeGrid3D> boxes;
    PathFinding pathFinding;
public:

    Grid3D(AABB3D bounds, int sizeX, int sizeY, int sizeZ);

    bool isEmpty(CubeGrid3D &cube, std::vector<Triangle *> &triangles);

    CubeGrid3D *getCubeFromPosition(int x, int y, int z);

    Vertex3D getClosestPoint(Vertex3D v, std::vector<Vertex3D> path, int &indexVertex);

    void doTestForNonEmptyGeometry(std::vector<Triangle *> &triangles);

    [[nodiscard]] int getNumberCubesX() const;

    [[nodiscard]] int getNumberCubesY() const;

    [[nodiscard]] int getNumberCubesZ() const;

    [[nodiscard]] const std::vector<CubeGrid3D> &getBoxes() const;

    void drawImGuiProperties();

    void reset(int x, int y, int z);

    PathFinding getPathFinding();

    void makeCells();

    void LoadPathFindingBlocksFromGrid();

    cJSON *getJSON();

    std::vector<CubeGrid3D> makeTravelCubesGrid();

    void setTravel(int x1, int y1, int z1, int x2, int y2, int z2);
};


#endif //BRAKEDA3D_GRID3D_H
