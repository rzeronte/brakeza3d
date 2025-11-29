
#ifndef SDL2_3D_ENGINE_MATRIX3D_H
#define SDL2_3D_ENGINE_MATRIX3D_H

#include <LinearMath/btMatrix3x3.h>
#include <glm/mat3x3.hpp>
#include "../3D/Vertex3D.h"

class M3 {
public:
    M3();

    M3(float, float, float, float, float, float, float, float, float);

    M3(float, float, float);

    M3 operator*(const M3 &v);

    M3 operator*(float scalar);

    Vertex3D operator*(Vertex3D);

    M3 operator+(const M3 &pm);

    M3 operator-(const M3 &pm);

    static M3 getMatrixIdentity();

    static M3 getMatrixRotationForEulerAngles(float x, float y, float z);

    M3 getTranspose();

    void setMatrixIdentity();

    void setMatrixRotationForEulerAngles(float x, float y, float z);

    static M3 RX(float deg);

    static M3 RY(float deg);

    static M3 RZ(float deg);

    static M3 ScaleMatrix(float scale);

    static M3 ScaleMatrix(float, float, float);

    void setup(float, float, float, float, float, float, float, float, float);

    float getPitch();

    float getYaw();

    float getRoll();

    float getYawDegree();

    float getPitchDegree();

    float getRollDegree();

    Vertex3D X();

    Vertex3D Y();

    Vertex3D Z();

    void setX(float, float, float);

    void setY(float, float, float);

    void setZ(float, float, float);

    static M3 getFromVectors(Vertex3D ZAxis, Vertex3D YAxis);

    float m[9];

    static M3 arbitraryAxis(Vertex3D A, float degrees);

    static M3 interpolateLinear(const M3 &m1, const M3 &m2, float t);

    btMatrix3x3 toBulletMat3();

    static M3 fromMat3GLM(const glm::mat3& glmMatrix);
    static M3 fromMat3Bullet(const btMatrix3x3& m);


    glm::mat3 toGLMMat3();

    static void normalize(M3 &matrix);

    Vertex3D getVertex3DAngles();
};


#endif //SDL2_3D_ENGINE_MATRIX3D_H
