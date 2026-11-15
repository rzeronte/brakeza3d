/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#include "../../include/Render/Line2D.h"


Line2D::Line2D(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2)
{
}

Point2D Line2D::middlePoint() const
{
    return {(x1 + x2) / 2, (y1 + y2) / 2};
}