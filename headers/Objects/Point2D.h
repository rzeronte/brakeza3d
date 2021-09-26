#ifndef SDL2_3D_ENGINE_POINT2D_H
#define SDL2_3D_ENGINE_POINT2D_H

#include <iostream>

class Point2D {
public:
    Point2D();

    Point2D(float x, float y);

    void consoleInfo(std::string a, bool);

    float x, y;
};


#endif //SDL2_3D_ENGINE_POINT2D_H
