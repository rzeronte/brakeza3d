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
    void doTestForNonEmptyGeometry(std::vector<Triangle *> &triangles);
    void DrawImGuiProperties();
    void Reset(int x, int y, int z);
    void MakeCells();
    void LoadPathFindingBlocksFromGrid();
    void setTravel(int x1, int y1, int z1, int x2, int y2, int z2);
    std::vector<CubeGrid3D> MakeTravelCubesGrid();
    std::vector<CubeGrid3D> computePath(int gx1, int gz1, int gx2, int gz2);
    void fillGrid3DFromImage(const std::string& imagePath, int threshold = 128);
    void drawDebug(Color color = Color::green());
    CubeGrid3D *getCubeFromPosition(int x, int y, int z);
    bool isCellWalkable(int x, int z) const;
    std::tuple<int,int> snapToWalkable(int gx, int gz, int maxRadius) const;
    Vertex3D getClosestPoint(Vertex3D v, std::vector<Vertex3D> path, int &indexVertex);
    PathFinding getPathFinding();
    cJSON *getJSON();
    [[nodiscard]] int getNumberCubesX() const;
    [[nodiscard]] int getNumberCubesY() const;
    [[nodiscard]] int getNumberCubesZ() const;
    [[nodiscard]] const std::vector<CubeGrid3D> &getBoxes() const;
};


#endif //BRAKEDA3D_GRID3D_H
