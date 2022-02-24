//
// Created by eduardo on 5/2/22.
//

#include <iostream>
#include "../../include/Render/M4.h"

M4::M4() {
    setMatrixIdentity();
}

M4::M4(
        float m0, float m1, float m2, float m3,
        float m4, float m5, float m6, float m7,
        float m8, float m9, float m10,float m11,
        float m12, float m13, float m14, float m15
) {
    this->setup(m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15);

}

void M4::setMatrixIdentity() {
    this->setup(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}
void M4::setup(
        float m0, float m1, float m2, float m3,
        float m4, float m5, float m6, float m7,
        float m8, float m9, float m10, float m11,
        float m12, float m13, float m14, float m15
) {
    m[0] = m0;
    m[1] = m1;
    m[2] = m2;
    m[3] = m3;
    m[4] = m4;
    m[5] = m5;
    m[6] = m6;
    m[7] = m7;
    m[8] = m8;
    m[9] = m9;
    m[10] = m10;
    m[11] = m11;
    m[12] = m12;
    m[13] = m13;
    m[14] = m14;
    m[15] = m15;
}

M4 M4::operator*(const M4 &pm) {
    return M4();
}

M4 M4::operator*(const float scalar) {
    return M4(
        m[0]   * scalar, m[1]   * scalar, m[2]   * scalar, m[3]   * scalar,
        m[4]   * scalar, m[5]   * scalar, m[6]   * scalar, m[7]   * scalar,
        m[8]   * scalar, m[9]   * scalar, m[10] * scalar, m[11] * scalar,
        m[12] * scalar, m[13] * scalar, m[14] * scalar, m[15] * scalar
    );
}

Vertex4D M4::operator*(const Vertex4D A) {
    return Vertex4D(
        (m[0] * A.x)  + (m[1] * A.y)  + (m[2] * A.z)  + (m[3] * A.w),
        (m[4] * A.x)  + (m[5] * A.y)  + (m[6] * A.z)  + (m[7] * A.w),
        (m[8] * A.x)  + (m[9] * A.y)  + (m[10] * A.z) + (m[11] * A.w),
        (m[12] * A.x) + (m[13] * A.y) + (m[14] * A.z) + (m[15] * A.w)
    );
}

M4 M4::operator+(const M4 &v) {
    return M4(
        m[0]  + v.m[0],  m[1]  + v.m[1],  m[2]  + v.m[2],  + m[3]  + v.m[3],
        m[4]  + v.m[4],  m[5]  + v.m[5],  m[6]  + v.m[6],  + m[7]  + v.m[7],
        m[8]  + v.m[8],  m[9]  + v.m[9],  m[10] + v.m[10], + m[11] + v.m[11],
        m[12] + v.m[12], m[13] + v.m[13], m[14] + v.m[14], + m[15] + v.m[15]
    );
}

M4 M4::operator-(const M4 &v) {
    return M4(
        m[0]  - v.m[0],  m[1]  - v.m[1],  m[2]  - v.m[2],  + m[3]  - v.m[3],
        m[4]  - v.m[4],  m[5]  - v.m[5],  m[6]  - v.m[6],  + m[7]  - v.m[7],
        m[8]  - v.m[8],  m[9]  - v.m[9],  m[10] - v.m[10], + m[11] - v.m[11],
        m[12] - v.m[12], m[13] - v.m[13], m[14] - v.m[14], + m[15] - v.m[15]
    );
}

void M4::consoleInfo(const std::string& label) const {
    printf("%s: \r\n", label.c_str());
    std::cout << m[0] << " " << m[1] << " " << m[2] << " "<< m[3] << std::endl;
    std::cout << m[4] << " " << m[5] << " " << m[6] << " "<< m[7] << std::endl;
    std::cout << m[8] << " " << m[9] << " " << m[10] << " "<< m[11] << std::endl;
    std::cout << m[12] << " " << m[13] << " " << m[14] << " "<< m[15] << std::endl;
}

M4 M4::getMatrixIdentity() {
    return M4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

M4 M4::getFrustumProjection(float f, float n, float l, float r, float b, float t) {
    return M4(
            (2*n) / (r-l), 0, (r+l) / (r-l), 0,
            0, (2*n) / (t-b), (t+b) / (t-b), 0,
            0, 0, -((f+n) / (f-n)), -((2*n*f)/ (f-n) ),
            0, 0, -1, 0
    );
}
M4 M4::getFrustumOrthographic(float f, float n, float l, float r, float b, float t) {
    return M4(
            2 / (r-l), 0, 0, -((r+l) / (r-l)),
            0, 2 / (t-b), 0, -((t+b) / (t-b)),
            0, 0, -2 / (f-n), -((f+n) / (f-n)),
            0, 0, 0, 1
    );
}

