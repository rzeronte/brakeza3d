//
// Created by darkhead on 4/3/21.
//

#include "Grid3D.h"
#include "../Render/Logging.h"
#include "../EngineSetup.h"

Grid3D::Grid3D( std::vector<Triangle*> &triangles, const AABB3D &bounds, int sizeX, int sizeY, int sizeZ)
{
    this->bounds = bounds;
    this->numberCubesX = sizeX;
    this->numberCubesY = sizeY;
    this->numberCubesZ = sizeZ;

    Vertex3D dimensions = this->bounds.max - this->bounds.min;

    float cubeSizeX = dimensions.x / this->numberCubesX;
    float cubeSizeY = dimensions.y / this->numberCubesY;
    float cubeSizeZ = dimensions.z / this->numberCubesZ;

    Vertex3D offsetPosition = Vertex3D::zero();

    for (int x = 0; x < numberCubesX; x++) {
        for (int y = 0; y < numberCubesY; y++) {
            for (int z = 0; z < numberCubesZ; z++) {
                auto *cubeGrid = new CubeGrid3D();
                cubeGrid->box = new AABB3D();
                cubeGrid->box->min = offsetPosition + Vertex3D::zero();
                cubeGrid->box->max = offsetPosition + Vertex3D( cubeSizeX, cubeSizeY, cubeSizeZ );
                cubeGrid->box->updateVertices();

                this->boxes.push_back(cubeGrid);
                offsetPosition.z += cubeSizeZ;
            }
            offsetPosition.z = 0;
            offsetPosition.y += cubeSizeY;
        }
        offsetPosition.y = 0;
        offsetPosition.x += cubeSizeX;
    }

    for (int i = 0; i < this->boxes.size(); i++) {
        this->boxes[i]->box->min = this->boxes[i]->box->min + bounds.min;
        this->boxes[i]->box->max = this->boxes[i]->box->max + bounds.min;
        this->boxes[i]->box->updateVertices();

        if (!this->isEmpty(*this->boxes[i], triangles)) {
            this->boxes[i]->is_empty = false;
        }
    }
}

bool Grid3D::isEmpty(CubeGrid3D &cube, std::vector<Triangle *> &triangles)
{
    for (int i = 0; i < triangles.size(); i++) {
        triangles[i]->updateObjectSpace();
        if (
            this->isVertexInsideAABB(triangles[i]->Ao, cube.box) ||
            this->isVertexInsideAABB(triangles[i]->Bo, cube.box) ||
            this->isVertexInsideAABB(triangles[i]->Co, cube.box)
        ) {
            return false;
        }
    }

    return true;
}

bool Grid3D::isVertexInsideAABB(Vertex3D v, AABB3D *aabb)
{
    aabb->updateVertices();
    std::vector<Plane> planes = aabb->getPlanes();

    for (int i = 0; i < planes.size(); i++) {
        Plane p = planes[i];
        float d = p.distance(v);
        if (d >= EngineSetup::getInstance()->FRUSTUM_CLIPPING_DISTANCE) {
            return false;
        }
    }

    return true;
}