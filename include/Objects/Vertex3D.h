
#ifndef SDL2_3D_ENGINE_VERTEX_H
#define SDL2_3D_ENGINE_VERTEX_H

#include <SDL2/SDL_surface.h>
#include <string>
#include <bullet/LinearMath/btVector3.h>
#include <glm/vec3.hpp>
#include <vector3.h>

class Vertex3D {
public:

    float x;
    float y;
    float z;
    float u;
    float v;

    Vertex3D();

    Vertex3D operator+(const Vertex3D &pm) const;

    Vertex3D operator-(const Vertex3D &pm) const;

    Vertex3D operator%(const Vertex3D &pm) const;

    float operator*(const Vertex3D &pm) const;

    bool operator!=(const Vertex3D &pm) const;

    bool operator==(const Vertex3D &pm) const;

    Vertex3D(float, float, float);

    explicit Vertex3D(const float[3]);

    [[nodiscard]] Vertex3D getNormalize() const;

    [[nodiscard]] Vertex3D getInverse() const;

    [[nodiscard]] float getModule() const;

    [[nodiscard]] float getSquaredLength() const;

    [[nodiscard]] Vertex3D getScaled(float) const;

    [[nodiscard]] Vertex3D getScaled(float xs, float ys, float zs) const;

    void setScaled(float);

    float distance(Vertex3D);

    [[nodiscard]] Vertex3D divide(float value) const;

    static Vertex3D zero();

    [[nodiscard]] btVector3 toBullet() const;

    [[nodiscard]] glm::vec3 toGLM() const;

    [[nodiscard]] aiVector3D toAssimp() const;

    static Vertex3D fromBullet(const btVector3 &v);

    static Vertex3D fromAssimp(const aiVector3D &v);

    static Vertex3D fromGLM(const glm::vec3 &v);

    static Vertex3D randomVertex();

    void toFloat(float *v) const;
};

#endif //SDL2_3D_ENGINE_VERTEX_H
