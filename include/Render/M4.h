//
// Created by eduardo on 5/2/22.
//

#ifndef BRAKEDA3D_M4_H
#define BRAKEDA3D_M4_H

#include "../Objects/Vertex3D.h"
#include "../Objects/Vertex4D.h"

class M4 {
public:
    float m[16];

    M4();
    M4(
        float m0,  float m1,  float m2,  float m3,
        float m4,  float m5,  float m6,  float m7,
        float m8,  float m9,  float m10,  float m11,
        float m12, float m13, float m14, float m15
    );
    static M4 getMatrixIdentity();

    static M4 getFrustumProjection(float f, float n, float l, float r, float b, float t);
    static M4 getFrustumOrthographic(float f, float n, float l, float r, float b, float t);

    void setMatrixIdentity();
    void setup(
        float m0,  float m1,  float m2,  float m3,
        float m4,  float m5,  float m6,  float m7,
        float m8,  float m9,  float m10,  float m11,
        float m12, float m13, float m14, float m15
  );

    M4 operator*(const M4 &pm);
    M4 operator*(const float scalar);
    Vertex4D operator*(const Vertex4D);
    M4 operator+(const M4 &pm);
    M4 operator-(const M4 &pm);

    void consoleInfo(const std::string &label) const;
};


#endif //BRAKEDA3D_M4_H
