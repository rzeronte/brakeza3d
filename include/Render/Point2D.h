/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
#ifndef SDL2_3D_ENGINE_POINT2D_H
#define SDL2_3D_ENGINE_POINT2D_H

#include <glm/detail/type_vec2.hpp>
#include <glm/vec2.hpp>

class Point2D {
public:
    Point2D();
    Point2D(int x, int y);
    explicit Point2D(glm::vec2 v);

    float operator*(const Point2D &pm) const;
    glm::vec2 toGLM() const;
    Point2D operator-(const Point2D &pm) const;
    Point2D operator+(const Point2D &pm) const;
    Point2D operator/(const Point2D &pm) const;
    [[nodiscard]] Point2D getNormalize() const;
    [[nodiscard]] Point2D getScaled(float) const;

    int x, y;
};


#endif //SDL2_3D_ENGINE_POINT2D_H
