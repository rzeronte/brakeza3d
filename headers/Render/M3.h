
#ifndef SDL2_3D_ENGINE_MATRIX3D_H
#define SDL2_3D_ENGINE_MATRIX3D_H


#include "../Objects/Vertex3D.h"
#include "../Objects/Object3D.h"

class M3 {
public:
    M3();
    M3(float, float, float, float, float, float, float, float, float);

    M3 operator *(const M3 pm);
    M3 operator *(const float scalar);
    Vertex3D   operator *(const Vertex3D);
    M3 operator +(const M3 &pm);
    M3 operator -(const M3 &pm);

    float m[9];

    const static M3 MatrixNULL();
    const static M3 MatrixIdentity();
    const static M3 MatrixModel(Vertex3D, Object3D *);
    const static M3 RX(float deg);
    const static M3 RY(float deg);
    const static M3 RZ(float deg);
    const static M3 ScaleMatrix(float scale);

    void setup (float, float, float, float, float, float, float, float, float);
    void consoleInfo(std::string label) const;
};


#endif //SDL2_3D_ENGINE_MATRIX3D_H
