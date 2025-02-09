
#include "../../include/Misc/Grid3D.h"
#include "../../include/Render/Maths.h"

Grid3D::Grid3D(const AABB3D &bounds, int sizeX, int sizeY, int sizeZ)
:
    pathFinding(PathFinding(sizeX, sizeY, sizeZ)),
    bounds(bounds),
    numberCubesX(sizeX),
    numberCubesY(sizeY),
    numberCubesZ(sizeZ)
{
    makeCells();
}

void Grid3D::makeCells()
{
    Vertex3D dimensions = bounds.max - bounds.min;

    float cubeSizeX = dimensions.x / numberCubesX;
    float cubeSizeY = dimensions.y / numberCubesY;
    float cubeSizeZ = dimensions.z / numberCubesZ;

    Vertex3D offsetPosition = Vertex3D::zero();
    for (int x = 0; x < numberCubesX; x++) {
        for (int y = 0; y < numberCubesY; y++) {
            for (int z = 0; z < numberCubesZ; z++) {
                auto cubeGrid = CubeGrid3D();
                cubeGrid.posX = x;
                cubeGrid.posY = y;
                cubeGrid.posZ = z;

                cubeGrid.box = AABB3D();
                cubeGrid.box.min = offsetPosition + Vertex3D::zero();
                cubeGrid.box.max = offsetPosition + Vertex3D(cubeSizeX, cubeSizeY, cubeSizeZ);
                cubeGrid.box.updateVertices();

                cubeGrid.box.min = cubeGrid.box.min + bounds.min;
                cubeGrid.box.max = cubeGrid.box.max + bounds.min;
                cubeGrid.box.updateVertices();
                cubeGrid.passed = true;

                boxes.push_back(cubeGrid);
                offsetPosition.z += cubeSizeZ;
            }
            offsetPosition.z = 0;
            offsetPosition.y += cubeSizeY;
        }
        offsetPosition.y = 0;
        offsetPosition.x += cubeSizeX;
    }
}

void Grid3D::doTestForNonEmptyGeometry(std::vector<Triangle *> &triangles)
{
    for (int i = 0; i < this->boxes.size(); i++) {
        if (!this->isEmpty(this->boxes[i], triangles)) {
            this->boxes[i].passed = true;
        } else {
            this->boxes[i].passed = false;
        }
    }
    LoadPathFindingBlocksFromGrid();
}

bool Grid3D::isEmpty(CubeGrid3D &cube, std::vector<Triangle *> &triangles)
{
    std::vector<Plane> planes = cube.box.getPlanes();

    for (int i = 0; i < triangles.size(); i++) {
        auto t = triangles[i];
        t->updateObjectSpace();

        bool r1 = Plane::isVertex3DClosedByPlanes(t->Ao, planes);
        if (r1) return false;

        bool r2 = Plane::isVertex3DClosedByPlanes(t->Bo, planes);
        if (r2) return false;

        bool r3 = Plane::isVertex3DClosedByPlanes(t->Co, planes);
        if (r3) return false;

        if (Maths::isTriangleIntersectingAABB(*t, cube.box)) {
            return false;
        }
    }

    return true;
}

CubeGrid3D *Grid3D::getCubeFromPosition(int x, int y, int z)
{
    for (int i = 0; (int) boxes.size(); i++) {
        if (boxes[i].posX == x && boxes[i].posY == y && boxes[i].posZ == z) {
            return &boxes[i];
        }
    }

    return nullptr;
}

Vertex3D Grid3D::getClosestPoint(Vertex3D v, std::vector<Vertex3D> path, int &indexVertex)
{
    float min_distance = 9999999999;
    int index = 0;
    for (int i = 0; i < path.size(); i++) {
        float d = Maths::distanceBetweenVertices(v, path[i]);
        if (d < min_distance) {
            min_distance = d;
            index = i;
        }
    }

    indexVertex = index;
    return path[index];
}

int Grid3D::getNumberCubesX() const {
    return numberCubesX;
}

int Grid3D::getNumberCubesY() const {
    return numberCubesY;
}

int Grid3D::getNumberCubesZ() const {
    return numberCubesZ;
}

const std::vector<CubeGrid3D> &Grid3D::getBoxes() const {
    return boxes;
}

void Grid3D::drawImGuiProperties()
{
    if (ImGui::TreeNode("Grid3D setup")) {
        static int sizeX = 1;
        static int sizeY = 1;
        static int sizeZ = 1;
        ImGui::SliderInt("Size X", &sizeX, 1, 50);
        ImGui::SliderInt("Size Y", &sizeY, 1, 50);
        ImGui::SliderInt("Size Z", &sizeZ, 1, 50);

        if (ImGui::Button("Update Grid3D")) {
            reset(sizeX, sizeY, sizeZ);
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("A* setup")) {
        if (ImGui::TreeNode("Cell from")) {
            ImGui::SliderInt("Cell X", &pathFinding.from[0], 0, getNumberCubesX() - 1);
            ImGui::SliderInt("Cell Y", &pathFinding.from[1], 0, getNumberCubesY() - 1);
            ImGui::SliderInt("Cell Z", &pathFinding.from[2], 0, getNumberCubesZ() - 1);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Cell to")) {
            ImGui::SliderInt("Cell X", &pathFinding.to[0], 0, getNumberCubesX()- 1);
            ImGui::SliderInt("Cell Y", &pathFinding.to[1], 0, getNumberCubesY()- 1);
            ImGui::SliderInt("Cell Z", &pathFinding.to[2], 0, getNumberCubesZ()- 1);
            ImGui::TreePop();
        }

        ImGui::TreePop();
    }
}

void Grid3D::reset(int x, int y, int z)
{
    numberCubesX = x;
    numberCubesY = y;
    numberCubesZ = z;
    boxes.clear();
    makeCells();
    pathFinding.reset(x, y, z);
}

PathFinding Grid3D::getPathFinding()
{
    return pathFinding;
}

void Grid3D::LoadPathFindingBlocksFromGrid()
{
    for (int x = 0; x < getNumberCubesX(); x++) {
        for (int y = 0; y < getNumberCubesY(); y++) {
            for (int z = 0; z < getNumberCubesZ(); z++) {
                CubeGrid3D *c = getCubeFromPosition(x, y, z);
                if (c->passed) {
                    pathFinding.setObstacle(x, y, z);
                }
            }
        }
    }
}