#ifndef BRAKEZA3D_THREADJOBPATHFINDING_H
#define BRAKEZA3D_THREADJOBPATHFINDING_H

#include "ThreadJobBase.h"
#include "../Render/Grid3D.h"
#include "../Brakeza.h"
#include <string>

class ThreadJobPathfinding : public ThreadJobBase
{
    Grid3D*     grid;
    std::string unitName;
    int gx1, gz1, gx2, gz2;

    std::string encodedPath;

public:
    ThreadJobPathfinding(Grid3D* grid, const std::string& unitName, int gx1, int gz1, int gx2, int gz2)
    :
        grid(grid),
        unitName(unitName),
        gx1(gx1), gz1(gz1), gx2(gx2), gz2(gz2)
    {
        function = [this](){ fnProcess(); };
        callback = [this](){ fnCallback(); };
    }

    void fnProcess()
    {
        auto cubes = grid->computePath(gx1, gz1, gx2, gz2);

        if (cubes.empty()) {
            encodedPath = "";
            return;
        }

        const auto& b     = grid->getBounds();
        const float minX  = b.min.x;
        const float minZ  = b.min.z;
        const float sizeX = b.max.x - b.min.x;
        const float sizeZ = b.max.z - b.min.z;
        const int   gridX = grid->getNumberCubesX();
        const int   gridZ = grid->getNumberCubesZ();

        std::string parts;
        for (const auto& cube : cubes) {
            float wx = (cube.posX + 0.5f) / gridX * sizeX + minX;
            float wz = (cube.posZ + 0.5f) / gridZ * sizeZ + minZ;
            char buf[64];
            snprintf(buf, sizeof(buf), "%.3f,%.3f", wx, wz);
            if (!parts.empty()) parts += ';';
            parts += buf;
        }
        encodedPath = parts;
    }

    void fnCallback()
    {
        auto obj = Brakeza::get()->getObjectByName(unitName);
        if (obj == nullptr) return;

        auto& lua = Components::get()->Scripting()->getLua();
        if (encodedPath.empty()) {
            obj->setLocalScriptVar("pathWaypoints", sol::make_object(lua, std::string("NONE")));
            LOG_MESSAGE("[ThreadJobPathfinding] %s -> no path found", unitName.c_str());
        } else {
            obj->setLocalScriptVar("pathWaypoints", sol::make_object(lua, encodedPath));
            LOG_MESSAGE("[ThreadJobPathfinding] %s -> path ready", unitName.c_str());
        }
    }
};

#endif //BRAKEZA3D_THREADJOBPATHFINDING_H
