/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
//
// Created by eduardo on 20/10/23.
//

#ifndef BRAKEZA3D_TILEMANAGER_H
#define BRAKEZA3D_TILEMANAGER_H

#include <vector>
#include "../Render/ParticlesContext.h"

struct OCTileRender {
    OCTileRender(bool draw, int id, int idX, int idY, int startX, int startY, int w, int h)
    :
    draw(draw),
    id(id),
    id_x(idX),
    id_y(idY),
    start_x(startX),
    start_y(startY),
    width(w),
    height(h)
    {
    }

    bool draw;
    int id;
    int id_x;
    int id_y;
    int start_x;
    int start_y;
    int width;
    int height;
};

class TileManager {
public:
    TileManager(int sizeTileWidth, int sizeTileHeight);

    std::vector<OCTileRender> tiles;
    int sizeTileWidth;
    int sizeTileHeight;
    int numberTilesHorizontal;
    int numberTilesVertical;
    int numberTiles;
    int tilePixelsBufferSize;
};


#endif //BRAKEZA3D_TILEMANAGER_H
