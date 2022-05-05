#ifndef SDL2_3D_ENGINE_POINT2D_H
#define SDL2_3D_ENGINE_POINT2D_H

#include <iostream>

class Point2D {
public:
    Point2D();

    Point2D(int x, int y);

    float operator*(const Point2D &pm) const;
    Point2D operator-(const Point2D &pm) const;
    Point2D operator+(const Point2D &pm) const;

    [[nodiscard]] Point2D getNormalize() const;
    float getLength();
    [[nodiscard]] Point2D getScaled(float) const;
    void consoleInfo(const std::string& a, bool) const;

    int x, y;
};


#endif //SDL2_3D_ENGINE_POINT2D_H
