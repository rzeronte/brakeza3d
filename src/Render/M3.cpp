#include <iostream>
#include "../../include/Render/M3.h"
#include "../../include/Misc/Tools.h"
#include "../../include/Render/Maths.h"

M3::M3() {
    setMatrixIdentity();
}

M3::M3(float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8) {
    this->setup(m0, m1, m2, m3, m4, m5, m6, m7, m8);
}

M3::M3(float x, float y, float z) {
    setMatrixRotationForEulerAngles(x, y, z);
}

void M3::setup(float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8) {
    m[0] = m0;
    m[1] = m1;
    m[2] = m2;
    m[3] = m3;
    m[4] = m4;
    m[5] = m5;
    m[6] = m6;
    m[7] = m7;
    m[8] = m8;
}

M3 M3::operator*(const M3 &v) {
    M3 M = M3();
    M.m[0] = m[0] * v.m[0] + m[1] * v.m[3] + m[2] * v.m[6];
    M.m[3] = m[3] * v.m[0] + m[4] * v.m[3] + m[5] * v.m[6];
    M.m[6] = m[6] * v.m[0] + m[7] * v.m[3] + m[8] * v.m[6];

    M.m[1] = m[0] * v.m[1] + m[1] * v.m[4] + m[2] * v.m[7];
    M.m[4] = m[3] * v.m[1] + m[4] * v.m[4] + m[5] * v.m[7];
    M.m[7] = m[6] * v.m[1] + m[7] * v.m[4] + m[8] * v.m[7];

    M.m[2] = m[0] * v.m[2] + m[1] * v.m[5] + m[2] * v.m[8];
    M.m[5] = m[3] * v.m[2] + m[4] * v.m[5] + m[5] * v.m[8];
    M.m[8] = m[6] * v.m[2] + m[7] * v.m[5] + m[8] * v.m[8];

    return M;
}

M3 M3::operator*(const float scalar) {
    return M3(
            m[0] * scalar, m[1] * scalar, m[2] * scalar,
            m[3] * scalar, m[4] * scalar, m[5] * scalar,
            m[6] * scalar, m[7] * scalar, m[8] * scalar
    );
}

Vertex3D M3::operator*(const Vertex3D A) {
    return Vertex3D(
            (m[0] * A.x) + (m[1] * A.y) + (m[2] * A.z),
            (m[3] * A.x) + (m[4] * A.y) + (m[5] * A.z),
            (m[6] * A.x) + (m[7] * A.y) + (m[8] * A.z)
    );
}

M3 M3::operator+(const M3 &v) {
    return M3(
            m[0] + v.m[0], m[1] + v.m[1], m[2] + v.m[2],
            m[3] + v.m[3], m[4] + v.m[4], m[5] + v.m[5],
            m[6] + v.m[6], m[7] + v.m[7], m[8] + v.m[8]
    );
}

M3 M3::operator-(const M3 &v) {
    return M3(
            m[0] - v.m[0], m[1] - v.m[1], m[2] - v.m[2],
            m[3] - v.m[3], m[4] - v.m[4], m[5] - v.m[5],
            m[6] - v.m[6], m[7] - v.m[7], m[8] - v.m[8]
    );
}

void M3::consoleInfo(const std::string& label) const {
    printf("%s: \r\n", label.c_str());
    std::cout << m[0] << " " << m[1] << " " << m[2] << std::endl;
    std::cout << m[3] << " " << m[4] << " " << m[5] << std::endl;
    std::cout << m[6] << " " << m[7] << " " << m[8] << std::endl;
}

M3 M3::getMatrixIdentity() {
    M3 M(
            1, 0, 0,
            0, 1, 0,
            0, 0, 1);

    return M;
}

M3 M3::getMatrixNULL() {
    M3 M(
            0, 0, 0,
            0, 0, 0,
            0, 0, 0
    );

    return M;
}

M3 M3::RX(float degrees) {
    float rads = Maths::degreesToRadians(degrees);
    return M3(
            1, 0, 0,
            0, cos(rads), -sin(rads),
            0, sin(rads), cos(rads)
    );
}

M3 M3::RY(float degrees) {
    float rads = Maths::degreesToRadians(degrees);
    return M3(
            cos(rads), 0, sin(rads),
            0, 1, 0,
            -sin(rads), 0, cos(rads)
    );
}

M3 M3::RZ(float degrees) {
    float rads = Maths::degreesToRadians(degrees);
    return M3(
            cos(rads), -sin(rads), 0,
            sin(rads), cos(rads), 0,
            0, 0, 1
    );
}

M3 M3::ScaleMatrix(float scale) {
    M3 M(
            scale, 0, 0,
            0, scale, 0,
            0, 0, scale
    );

    return M;
}

M3 M3::ScaleMatrix(float xScale, float yScale, float zScale) {
    M3 M(
        xScale, 0, 0,
        0, yScale, 0,
        0, 0, zScale
    );

    return M;
}

float M3::getPitch() {
    return atan2f(m[7], m[8]);
}

float M3::getYaw() {
    float c2 = sqrtf(m[7] * m[7] + m[8] * m[8]);
    return atan2f(-m[6], c2);
}

float M3::getRoll() {
    return atan2f(m[3], m[0]);
}

M3 M3::getMatrixRotationForEulerAngles(float x, float y, float z) {
    M3 MRX = M3::RX(x);
    M3 MRY = M3::RY(y);
    M3 MRZ = M3::RZ(z);

    return MRX * MRY * MRZ;
}

void M3::setMatrixIdentity() {
    this->setup(
            1, 0, 0,
            0, 1, 0,
            0, 0, 1
    );
}

void M3::setMatrixNULL() {
    this->setup(0, 0, 0, 0, 0, 0, 0, 0, 0);
}

void M3::setMatrixRotationForEulerAngles(float x, float y, float z) {
    M3 m = M3::getMatrixRotationForEulerAngles(x, y, z);
    this->setup(
            m.m[0], m.m[1], m.m[2],
            m.m[3], m.m[4], m.m[5],
            m.m[6], m.m[7], m.m[8]
    );
}

M3 M3::getTranspose() {
    M3 m(
            this->m[0], this->m[3], this->m[6],
            this->m[1], this->m[4], this->m[7],
            this->m[2], this->m[5], this->m[8]
    );

    return m;
}

float M3::getYawDegree() {
    return Maths::radiansToDegrees(this->getYaw());
}

float M3::getPitchDegree() {
    return Maths::radiansToDegrees(this->getPitch());
}

float M3::getRollDegree() {
    return Maths::radiansToDegrees(this->getRoll());
}

M3 M3::getFromVectors(Vertex3D ZAxis, Vertex3D YAxis) {
    M3 m;

    Vertex3D Xi = (ZAxis % YAxis).getNormalize();
    Vertex3D Yi = (Xi % ZAxis).getNormalize();

    m.setX(Xi.x, Xi.y, Xi.z);
    m.setY(Yi.x, Yi.y, Yi.z);
    m.setZ(ZAxis.x, ZAxis.y, ZAxis.z);

    return m;
}


Vertex3D M3::X() {
    return Vertex3D(m[0], m[1], m[2]);
}

Vertex3D M3::Y() {
    return Vertex3D(m[3], m[4], m[5]);
}

Vertex3D M3::Z() {
    return Vertex3D(m[6], m[7], m[8]);
}

void M3::setX(float x, float y, float z) {
    m[0] = x;
    m[1] = y;
    m[2] = z;
}

void M3::setY(float x, float y, float z) {
    m[3] = x;
    m[4] = y;
    m[5] = z;
}

void M3::setZ(float x, float y, float z) {
    m[6] = x;
    m[7] = y;
    m[8] = z;
}
