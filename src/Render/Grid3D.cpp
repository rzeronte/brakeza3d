
#include "../../include/Render/Grid3D.h"
#include "../../include/Render/Maths.h"

Grid3D::Grid3D(AABB3D bounds, int sizeX, int sizeY, int sizeZ)
:
    numberCubesX(sizeX),
    numberCubesY(sizeY),
    numberCubesZ(sizeZ),
    bounds(bounds),
    pathFinding(PathFinding(sizeX, sizeY, sizeZ))
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
    for (unsigned int i = 0; boxes.size(); i++) {
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
    if (ImGui::TreeNode("A* setup cells")) {
        if (ImGui::TreeNode("From")) {
            ImGui::SliderInt("X", &pathFinding.from[0], 0, getNumberCubesX() - 1);
            ImGui::SliderInt("Y", &pathFinding.from[1], 0, getNumberCubesY() - 1);
            ImGui::SliderInt("Z", &pathFinding.from[2], 0, getNumberCubesZ() - 1);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("To")) {
            ImGui::SliderInt("X", &pathFinding.to[0], 0, getNumberCubesX() - 1);
            ImGui::SliderInt("Y", &pathFinding.to[1], 0, getNumberCubesY() - 1);
            ImGui::SliderInt("Z", &pathFinding.to[2], 0, getNumberCubesZ() - 1);
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }

    ImGui::Separator();

    static int sizeX = getNumberCubesX();
    static int sizeY = getNumberCubesY();
    static int sizeZ = getNumberCubesZ();
    ImGui::SliderInt("Size X", &sizeX, 1, 10);
    ImGui::SliderInt("Size Y", &sizeY, 1, 10);
    ImGui::SliderInt("Size Z", &sizeZ, 1, 10);

    if (ImGui::Button("Update Grid3D")) {
        reset(sizeX, sizeY, sizeZ);
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
    for (int x = 0; x < getNumberCubesX() - 1; x++) {
        for (int y = 0; y < getNumberCubesY() - 1; y++) {
            for (int z = 0; z < getNumberCubesZ() - 1; z++) {
                CubeGrid3D *c = getCubeFromPosition(x, y, z);
                if (c->passed) {
                    pathFinding.setObstacle(x, y, z);
                }
            }
        }
    }
}

cJSON *Grid3D::getJSON()
{
    cJSON *gridJSON = cJSON_CreateObject();

    cJSON_AddNumberToObject(gridJSON, "x", getNumberCubesX());
    cJSON_AddNumberToObject(gridJSON, "y", getNumberCubesY());
    cJSON_AddNumberToObject(gridJSON, "z", getNumberCubesZ());

    return gridJSON;
}

std::vector<CubeGrid3D> Grid3D::makeTravelCubesGrid()
{
    auto path = getPathFinding().makeTravelIndexes();

    if (path.empty()) return {};
    std::vector<CubeGrid3D> output;
    for (const auto& step : path) {
        int x = std::get<0>(step);
        int y = std::get<1>(step);
        int z = std::get<2>(step);

        auto cube = getCubeFromPosition(x, y, z);
        if (cube!= nullptr) {
            output.push_back(*cube);
        }
    }

    return output;
}

void Grid3D::setTravel(int x1, int y1, int z1, int x2, int y2, int z2)
{
    pathFinding.setTravel(x1, y1, z1, x2, y2, z2);
}
