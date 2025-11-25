
#ifndef SDL2_3D_ENGINE_VERTEX_H
#define SDL2_3D_ENGINE_VERTEX_H

#include <bullet/LinearMath/btVector3.h>
#include <glm/vec3.hpp>
#include <vector3.h>
#include <glm/vec4.hpp>

class Vertex3D {
public:

    float x;
    float y;
    float z;
    float u;
    float v;

    Vertex3D();
    explicit Vertex3D(const float[3]);
    Vertex3D(float, float, float);

    Vertex3D operator+(const Vertex3D &pm) const;
    Vertex3D operator-(const Vertex3D &pm) const;
    Vertex3D operator%(const Vertex3D &pm) const;
    float operator*(const Vertex3D &pm) const;
    bool operator!=(const Vertex3D &pm) const;
    bool operator==(const Vertex3D &pm) const;

    void toFloat(float *v) const;
    void setScaled(float);
    float distance(Vertex3D);
    [[nodiscard]] Vertex3D divide(float value) const;
    [[nodiscard]] btVector3 toBullet() const;
    [[nodiscard]] glm::vec3 toGLM() const;
    [[nodiscard]] glm::vec4 toGLM4() const;
    [[nodiscard]] aiVector3D toAssimp() const;
    [[nodiscard]] Vertex3D getNormalize() const;
    [[nodiscard]] Vertex3D getInverse() const;
    [[nodiscard]] float getModule() const;
    [[nodiscard]] float getSquaredLength() const;
    [[nodiscard]] Vertex3D getScaled(float) const;
    [[nodiscard]] Vertex3D getScaled(float xs, float ys, float zs) const;
    static Vertex3D zero();
    static Vertex3D fromBullet(const btVector3 &v);
    static Vertex3D fromAssimp(const aiVector3D &v);
    static Vertex3D fromGLM(const glm::vec3 &v);
    static Vertex3D randomVertex();
};

#endif //SDL2_3D_ENGINE_VERTEX_H
