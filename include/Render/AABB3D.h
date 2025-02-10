#ifndef BRAKEDA3D_AABB3D_H
#define BRAKEDA3D_AABB3D_H


#include <vector>
#include "../Objects/Vertex3D.h"
#include "../Render/Plane.h"
#include "../include/Misc/cJSON.h"

class AABB3D {
public:
    Vertex3D max;
    Vertex3D min;
    Vertex3D vertices[8];

    void operator=(const AABB3D &bounds) {
        this->min = bounds.min;
        this->max = bounds.max;
        this->updateVertices();
    }

    Vertex3D size() {
        return max - min;
    }

    bool isColliding(AABB3D *other) const {
        return !(max.x < other->min.x || min.x > other->max.x ||
                 max.y < other->min.y || min.z > other->max.y ||
                 max.z < other->min.z || min.z > other->max.z
        );
    }

    void setScale(float scale) {
        min = min - Vertex3D(1, 1, 1).getScaled(scale);
        max = max + Vertex3D(1, 1, 1).getScaled(scale);
        updateVertices();
    }

    void updateVertices() {
        this->vertices[0] = this->max;
        this->vertices[1] = this->min;
        this->vertices[2] = Vertex3D(this->max.x, this->max.y, this->min.z);
        this->vertices[3] = Vertex3D(this->max.x, this->min.y, this->max.z);
        this->vertices[4] = Vertex3D(this->min.x, this->max.y, this->max.z);
        this->vertices[5] = Vertex3D(this->max.x, this->min.y, this->min.z);
        this->vertices[6] = Vertex3D(this->min.x, this->max.y, this->min.z);
        this->vertices[7] = Vertex3D(this->min.x, this->min.y, this->max.z);
    }

    std::vector<Plane> getPlanes() {
        std::vector<Plane> planes;
        planes.resize(6);

        planes[0] = Plane(this->vertices[0], this->vertices[4], this->vertices[3]);
        planes[1] = Plane(this->vertices[2], this->vertices[0], this->vertices[5]);
        planes[2] = Plane(this->vertices[6], this->vertices[2], this->vertices[1]);
        planes[3] = Plane(this->vertices[4], this->vertices[6], this->vertices[1]);
        planes[4] = Plane(this->vertices[2], this->vertices[4], this->vertices[0]);
        planes[5] = Plane(this->vertices[5], this->vertices[3], this->vertices[1]);

        return planes;
    }

    Vertex3D getCenter() {
        Vertex3D dimensions = (this->max - this->min).getScaled(0.5);

        return Vertex3D(
            this->min.x + dimensions.x,
            this->min.y + dimensions.y,
            this->min.z + dimensions.z
        );
    }

    [[nodiscard]] bool isPointInside(const Vertex3D& point) const
    {
        return (point.x >= min.x && point.x <= max.x &&
                point.y >= min.y && point.y <= max.y &&
                point.z >= min.z && point.z <= max.z);
    }

    cJSON *getJSON()
    {
        cJSON *aabbJSON = cJSON_CreateObject();

        cJSON *minJSON = cJSON_CreateObject();
        cJSON_AddNumberToObject(minJSON, "x", (float) min.x);
        cJSON_AddNumberToObject(minJSON, "y", (float) min.y);
        cJSON_AddNumberToObject(minJSON, "z", (float) min.z);

        cJSON *maxJSON = cJSON_CreateObject();
        cJSON_AddNumberToObject(maxJSON, "x", (float) max.x);
        cJSON_AddNumberToObject(maxJSON, "y", (float) max.y);
        cJSON_AddNumberToObject(maxJSON, "z", (float) max.z);

        cJSON_AddItemToObject(aabbJSON, "min", minJSON);
        cJSON_AddItemToObject(aabbJSON, "max", maxJSON);

        return aabbJSON;
    }
};


#endif //BRAKEDA3D_AABB3D_H
