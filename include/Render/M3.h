
#ifndef SDL2_3D_ENGINE_MATRIX3D_H
#define SDL2_3D_ENGINE_MATRIX3D_H


#include "../Objects/Vertex3D.h"

class M3 {
public:
    M3();

    M3(
       float, float, float,
       float, float, float,
       float, float, float
   );

    M3(float, float, float);

    M3 operator*(const M3 &pm);

    M3 operator*(const float scalar);

    Vertex3D operator*(const Vertex3D);

    M3 operator+(const M3 &pm);

    M3 operator-(const M3 &pm);


    static M3 getMatrixNULL();

    static M3 getMatrixIdentity();

    static M3 getMatrixRotationForEulerAngles(float x, float y, float z);

    M3 getTranspose();

    void setMatrixNULL();

    void setMatrixIdentity();

    void setMatrixRotationForEulerAngles(float x, float y, float z);

    static M3 RX(float deg);

    static M3 RY(float deg);

    static M3 RZ(float deg);

    static M3 ScaleMatrix(float scale);

    static M3 ScaleMatrix(float, float, float);

    void setup(float, float, float, float, float, float, float, float, float);

    void consoleInfo(const std::string& label) const;

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

};


#endif //SDL2_3D_ENGINE_MATRIX3D_H
