//
// Created by darkhead on 4/3/21.
//

#include "Grid3D.h"
#include "../EngineSetup.h"
#include "../Render/Maths.h"
#include "Tools.h"
#include <iostream>
#include <fstream>
#include <SDL_image.h>

Grid3D::Grid3D(std::vector<Triangle*> *triangles, const AABB3D &bounds, int sizeX, int sizeY, int sizeZ, EmptyStrategies strategy)
{
    this->strategy = strategy;

    this->bounds = bounds;
    this->numberCubesX = sizeX;
    this->numberCubesY = sizeY;
    this->numberCubesZ = sizeZ;

    this->triangles = triangles;

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
}

void Grid3D::applyEmptyStrategy()
{
    switch (strategy) {
        case Grid3D::EmptyStrategies::RAY_INTERSECTION:
            this->fillEmptiesWithVector3DvsTriangles( this->rayIntersectionDirection, *this->triangles);
            break;
        case Grid3D::EmptyStrategies::CONTAIN_TRIANGLES:
            this->fillEmptiesWithAABBvsTriangles(*this->triangles);
            break;
        case Grid3D::EmptyStrategies::IMAGE_FILE:
            this->fillEmptiesWithImageData(this->imageFilename, this->fixedYImageData);
            break;
        default:
            break;
    }
}

void Grid3D::fillEmptiesWithAABBvsTriangles(std::vector<Triangle*> &triangles)
{
    for (int i = 0; i < this->boxes.size(); i++) {
        if (this->isEmpty(*this->boxes[i], triangles)) {
            this->boxes[i]->is_empty = true;
        } else {
            this->boxes[i]->is_empty = false;
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

void Grid3D::fillEmptiesWithImageData(std::string filename, int fixedY)
{
    SDL_Surface *s = IMG_Load(filename.c_str());

    int sx = s->h;
    int sy = s->w;

    this->numberCubesX = sx;
    this->numberCubesZ = sy;
    this->numberCubesY = fixedY;    // no height in grids from images

    for (int x = 0; x < numberCubesX; x++) {
        for (int z = 0; z < numberCubesZ; z++) {
            CubeGrid3D *c = getFromPosition(z, fixedY, x);

            Uint32 color = Tools::getSurfacePixel(s, x, z);
            Uint8 pred, pgreen, pblue, palpha;
            SDL_GetRGBA(color, s->format, &pred, &pgreen, &pblue, &palpha);
            if (pred == 0 && pgreen == 0 && pblue == 0) {
                c->is_empty = false;
            } else {
                c->is_empty = true;
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

void Grid3D::setRayIntersectionDirection(Vertex3D rayIntersectionDirection)
{
    Grid3D::rayIntersectionDirection = rayIntersectionDirection;
}

CubeGrid3D *Grid3D::getFromPosition(int x, int y, int z)
{
    for (int i = 0; i < this->boxes.size(); i++) {
        if (this->boxes[i]->posX == x && this->boxes[i]->posY == y &&this->boxes[i]->posZ == z) {
            return this->boxes[i];
        }
    }

    return NULL;
}

void Grid3D::setImageFilename(const std::string &imageFilename)
{
    Grid3D::imageFilename = imageFilename;
}

void Grid3D::setFixedYImageData(int fixedYImageData) {
    Grid3D::fixedYImageData = fixedYImageData;
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

