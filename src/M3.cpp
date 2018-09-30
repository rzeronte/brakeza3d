#include <iostream>
#include "../headers/M3.h"
#include "../headers/Tools.h"

M3::M3() {
}

M3::M3 (float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8) {
    this->setup(m0, m1, m2, m3, m4, m5, m6, m7, m8);
}

void M3::setup (float m0, float m1, float m2, float m3, float m4, float m5, float m6, float m7, float m8) {
    m[0] = m0  ;  m[1] = m1  ;  m[2] = m2;
    m[3] = m3  ;  m[4] = m4  ;  m[5] = m5;
    m[6] = m6  ;  m[7] = m7  ;  m[8] = m8;
}

M3 M3::operator *(const M3 v)
{
    M3 M = M3();
    M.m[0] = m[0]*v.m[0] + m[1]*v.m[3] + m[2]*v.m[6]  ;  M.m[1] = m[0]*v.m[1] + m[1]*v.m[4] + m[2]*v.m[7]  ;  M.m[2] = m[0]*v.m[2] + m[1]*v.m[5] + m[2]*v.m[8];
    M.m[3] = m[3]*v.m[0] + m[4]*v.m[3] + m[5]*v.m[6]  ;  M.m[4] = m[3]*v.m[1] + m[4]*v.m[4] + m[5]*v.m[7]  ;  M.m[5] = m[3]*v.m[2] + m[4]*v.m[5] + m[5]*v.m[8];
    M.m[6] = m[6]*v.m[0] + m[7]*v.m[3] + m[8]*v.m[6]  ;  M.m[7] = m[6]*v.m[1] + m[7]*v.m[4] + m[8]*v.m[7]  ;  M.m[8] = m[6]*v.m[2] + m[7]*v.m[5] + m[8]*v.m[8];

    return M;
}

M3 M3::operator *(const float scalar)
{
    M3 M = M3();
    M.m[0] = m[0] * scalar  ;  M.m[1] = m[1] * scalar  ;  M.m[2] = m[3] * scalar;
    M.m[3] = m[3] * scalar  ;  M.m[4] = m[4] * scalar  ;  M.m[5] = m[5] * scalar;
    M.m[6] = m[6] * scalar  ;  M.m[7] = m[7] * scalar  ;  M.m[8] = m[8] * scalar;

    return M;
}

Vertex M3::operator *(const Vertex A)
{
    Vertex V = Vertex();
    V.x = (m[0] * A.x) + (m[1] * A.y) + (m[2] * A.z);
    V.y = (m[3] * A.x) + (m[4] * A.y) + (m[5] * A.z);
    V.z = (m[6] * A.x) + (m[7] * A.y) + (m[8] * A.z);

    return V;
}

M3 M3::operator +(const M3 &v)
{
    M3 M = M3();

    M.m[0] = m[0] + v.m[0]  ;  M.m[1] = m[1] + v.m[1]  ;  M.m[2] = m[3] + v.m[2];
    M.m[3] = m[3] + v.m[3]  ;  M.m[4] = m[4] + v.m[4]  ;  M.m[5] = m[5] + v.m[5];
    M.m[6] = m[6] + v.m[6]  ;  M.m[7] = m[7] + v.m[7]  ;  M.m[8] = m[8] + v.m[8];

    return M;
}

M3 M3::operator -(const M3 &v)
{
    M3 M = M3();

    M.m[0] = m[0] - v.m[0]  ;  M.m[1] = m[1] - v.m[1]  ;  M.m[2] = m[3] - v.m[2];
    M.m[3] = m[3] - v.m[3]  ;  M.m[4] = m[4] - v.m[4]  ;  M.m[5] = m[5] - v.m[5];
    M.m[6] = m[6] - v.m[6]  ;  M.m[7] = m[7] - v.m[7]  ;  M.m[8] = m[8] - v.m[8];

    return M;
}

void M3::consoleInfo(std::string label) const {
    printf("%s: \r\n", label.c_str());
    std::cout << m[0] << " " << m[1] << " " << m[2] << std::endl;
    std::cout << m[3] << " " << m[4] << " " << m[5] << std::endl;
    std::cout << m[6] << " " << m[7] << " " << m[8] << std::endl;
}

const M3 M3::MatrixIdentity() {
    M3 M(
    1, 0, 0,
    0, 1, 0,
    0, 0, 1);

    return M;
}

const M3 M3::MatrixModel(Vertex V, Object3D *o) {
}

const M3 M3::MatrixNULL() {
    M3 M(
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    );

    return M;
}

const M3 M3::RX(float degrees) {
    float rads = Tools::degreesToRadians(degrees);
    M3 M(
        1, 0        , 0         ,
        0, cos(rads), -sin(rads),
        0, sin(rads), cos(rads)
    );

    return M;
}

const M3 M3::RY(float degrees) {
    float rads = Tools::degreesToRadians(degrees);
    M3 M(
        cos(rads) , 0 , sin(rads),
        0         , 1 , 0        ,
        -sin(rads), 0 , cos(rads)
    );

    return M;
}

const M3 M3::RZ(float degrees) {
    float rads = Tools::degreesToRadians(degrees);
    M3 M(
        cos(rads) , -sin(rads) , 0,
        sin(rads) , cos(rads)  , 0,
        0         , 0          , 1
    );

    return M;
}

const M3 M3::ScaleMatrix(float scale) {
    M3 M(
        scale, 0    , 0,
        0    , scale, 0,
        0    , 0    , scale
    );

    return M;
}