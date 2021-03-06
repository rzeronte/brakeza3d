//
// Created by darkhead on 4/3/21.
//

#include "Grid3D.h"
#include "../EngineSetup.h"
#include "../Render/Maths.h"
#include <iostream>
#include <fstream>

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
                cubeGrid->posX = x;
                cubeGrid->posY = y;
                cubeGrid->posZ = z;

                cubeGrid->box = new AABB3D();
                cubeGrid->box->min = offsetPosition + Vertex3D::zero();
                cubeGrid->box->max = offsetPosition + Vertex3D( cubeSizeX, cubeSizeY, cubeSizeZ );
                cubeGrid->box->updateVertices();

                cubeGrid->box->min = cubeGrid->box->min + bounds.min;
                cubeGrid->box->max = cubeGrid->box->max + bounds.min;
                cubeGrid->box->updateVertices();
                cubeGrid->is_empty = true;

                this->boxes.push_back(cubeGrid);
                offsetPosition.z += cubeSizeZ;
            }
            offsetPosition.z = 0;
            offsetPosition.y += cubeSizeY;
        }
        offsetPosition.y = 0;
        offsetPosition.x += cubeSizeX;
    }

    this->fillEmptiesWithVector3DvsTriangles(Vertex3D(0, 1, 0), triangles);
}

void Grid3D::fillEmptiesWithAABBvsTriangles(std::vector<Triangle*> &triangles)
{
    for (int i = 0; i < this->boxes.size(); i++) {
        if (this->isEmpty(*this->boxes[i], triangles)) {
            this->boxes[i]->is_empty = true;
        }
    }
}

void Grid3D::fillEmptiesWithVector3DvsTriangles(Vertex3D dir, std::vector<Triangle*> &triangles)
{
    float rayLength = 50;

    for (int i = 0; i < this->boxes.size(); i++) {

        std::vector<Vertex3D> testVertices;
        testVertices.push_back( this->boxes[i]->box->getCenter() );
        testVertices.push_back( this->boxes[i]->box->min );
        testVertices.push_back( this->boxes[i]->box->max );

        for (int x = 0 ; x < triangles.size(); x++) {
            Plane trianglePlane = Plane(triangles[x]->A, triangles[x]->B, triangles[x]->C);

            bool intersect = false;

            for (int j = 0; j < testVertices.size(); j++) {
                Vertex3D origin  = testVertices[j] - dir.getScaled(rayLength);
                Vertex3D destiny = testVertices[j] + dir.getScaled(rayLength);
                Vector3D v(origin, destiny);

                if ( Maths::isVector3DClippingPlane( trianglePlane, v ) ) {
                    float t;
                    Vertex3D intersectionPoint = trianglePlane.getPointIntersection(v.vertex1, v.vertex2, t);
                    if (triangles[x]->isPointInside(intersectionPoint)) {
                        this->boxes[i]->is_empty = false;
                        intersect = true;
                    }
                }
            }

            if (intersect) {
                break;
            }
        }
    }
}

bool Grid3D::isEmpty(CubeGrid3D &cube, std::vector<Triangle *> &triangles)
{
    for (int i = 0; i < triangles.size(); i++) {
        triangles[i]->updateObjectSpace();
        std::vector<Plane> planes = cube.box->getPlanes();

        bool r1 = Plane::isVertex3DClosedByPlanes(triangles[i]->Ao, planes);
        if (r1) return false;

        bool r2 = Plane::isVertex3DClosedByPlanes(triangles[i]->Bo, planes);
        if (r2) return false;

        bool r3 = Plane::isVertex3DClosedByPlanes(triangles[i]->Co, planes);
        if (r3) return false;
    }

    return true;
}

bool Grid3D::saveToFile(std::string filename)
{
    std::ofstream file(filename, std::ios::out);

    if(file.is_open()) {

        for (int i = 0; i < this->boxes.size(); i++) {
            std::string l =
                "{ x: " + std::to_string(this->boxes[i]->posX) +
                ", y: " + std::to_string(this->boxes[i]->posY) +
                ", z: " + std::to_string(this->boxes[i]->posZ) +
                ", empty: " + std::to_string(this->boxes[i]->is_empty) +
                ", min: " +
                    "{ x: " + std::to_string(this->boxes[i]->box->min.x) +
                    ", y: " + std::to_string(this->boxes[i]->box->min.y) +
                    ", z: " + std::to_string(this->boxes[i]->box->min.z) +
                    "}, "
                ", max: " +
                    "{ x: " + std::to_string(this->boxes[i]->box->max.x) +
                    ", y: " + std::to_string(this->boxes[i]->box->max.y) +
                    ", z: " + std::to_string(this->boxes[i]->box->max.z) +
                    "}"
                "}"
            ;
            file << l << std::endl;
        }
        file.close();
    } else  {
        std::cerr<<"Unable to open file";
    }

    return false;
}
