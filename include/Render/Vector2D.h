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
// Created by eduardo on 12/05/22.
//

#ifndef BRAKEDA3D_VECTOR2D_H
#define BRAKEDA3D_VECTOR2D_H

#include <string>

class Vector2D {
public:
    Vector2D();
    Vector2D(float x, float y);

    float operator*(const Vector2D &pm) const;
    float getLength() const;
    void consoleInfo(const std::string& a, bool) const;
    Vector2D operator-(const Vector2D &pm) const;
    Vector2D operator+(const Vector2D &pm) const;
    Vector2D operator/(const Vector2D &pm) const;
    [[nodiscard]] Vector2D getNormalize() const;
    [[nodiscard]] Vector2D getScaled(float) const;

    float x = 0;
    float y = 0;
};


#endif //BRAKEDA3D_VECTOR2D_H
