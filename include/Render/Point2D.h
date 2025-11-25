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
