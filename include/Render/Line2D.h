
#ifndef SDL2_3D_ENGINE_LINE2D_H
#define SDL2_3D_ENGINE_LINE2D_H

#include "Point2D.h"

class Line2D {
    int x1, y1, x2, y2 = 0;
    bool debug = false;
public:
    Line2D(int, int, int, int);
    [[nodiscard]] Point2D middlePoint() const;
};


#endif //SDL2_3D_ENGINE_LINE2D_H
