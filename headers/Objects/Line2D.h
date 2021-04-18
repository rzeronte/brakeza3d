
#ifndef SDL2_3D_ENGINE_LINE2D_H
#define SDL2_3D_ENGINE_LINE2D_H


#include <SDL2/SDL.h>
#include "Point2D.h"

class Line2D {
public:
    int x1, y1, x2, y2;

    bool debug = false;

    Line2D();
    Line2D(int, int, int, int);
    Point2D middlePoint() ;

};


#endif //SDL2_3D_ENGINE_LINE2D_H
