
#ifndef SDL2_3D_ENGINE_MATRIX3D_H
#define SDL2_3D_ENGINE_MATRIX3D_H

#include <LinearMath/btMatrix3x3.h>
#include <glm/mat3x3.hpp>
#include "../3D/Vertex3D.h"

class M3 {
public:
    float m[9];
    M3();

    M3(float, float, float, float, float, float, float, float, float);
    M3(float, float, float);
    M3 operator*(const M3 &v);
    M3 operator*(float scalar);
    M3 operator+(const M3 &pm);
    M3 operator-(const M3 &pm);

    float getPitch() const;
    float getYaw() const;
    float getRoll() const;
    float getYawDegree() const;
    float getPitchDegree() const;
    float getRollDegree() const;
    void setMatrixIdentity();
    void setMatrixRotationForEulerAngles(float x, float y, float z);
    void setup(float, float, float, float, float, float, float, float, float);
    void setX(float, float, float);
    void setY(float, float, float);
    void setZ(float, float, float);
    M3 getTranspose();
    btMatrix3x3 toBulletMat3() const;
    glm::mat3 toGLMMat3() const;
    Vertex3D X() const;
    Vertex3D Y() const;
    Vertex3D Z() const;
    Vertex3D operator*(Vertex3D);
    Vertex3D getVertex3DAngles() const;
    static void normalize(M3 &matrix);
    static M3 RX(float deg);
    static M3 RY(float deg);
    static M3 RZ(float deg);
    static M3 getMatrixIdentity();
    static M3 getMatrixRotationForEulerAngles(float x, float y, float z);
    static M3 ScaleMatrix(float scale);
    static M3 ScaleMatrix(float, float, float);
    static M3 getFromVectors(const Vertex3D &ZAxis, const Vertex3D &YAxis);
    static M3 arbitraryAxis(const Vertex3D &A, float degrees);
    static M3 interpolateLinear(const M3 &m1, const M3 &m2, float t);
    static M3 fromMat3GLM(const glm::mat3& glmMatrix);
    static M3 fromMat3Bullet(const btMatrix3x3& m);
};


#endif //SDL2_3D_ENGINE_MATRIX3D_H
