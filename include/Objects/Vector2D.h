//
// Created by eduardo on 12/05/22.
//

#ifndef BRAKEDA3D_VECTOR2D_H
#define BRAKEDA3D_VECTOR2D_H


#include <string>

class Vector2D {
public:
    Vector2D();

    float operator*(const Vector2D &pm) const;
    Vector2D operator-(const Vector2D &pm) const;
    Vector2D operator+(const Vector2D &pm) const;

    [[nodiscard]] Vector2D getNormalize() const;
    float getLength();
    [[nodiscard]] Vector2D getScaled(float) const;
    void consoleInfo(const std::string& a, bool) const;

    Vector2D(float x, float y);

    Vector2D operator/(const Vector2D &pm) const;

    float x;
    float y;
};


#endif //BRAKEDA3D_VECTOR2D_H
