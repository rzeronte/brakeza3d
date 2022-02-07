//
// Created by eduardo on 5/2/22.
//

#ifndef BRAKEDA3D_VERTEX4D_H
#define BRAKEDA3D_VERTEX4D_H


#include <string>

class Vertex4D {
public:
    float x;
    float y;
    float z;
    float w;

    Vertex4D();
    Vertex4D(float x, float y, float z, float w);

    void consoleInfo(const std::__cxx11::basic_string<char> &label, bool returnLine) const;
};


#endif //BRAKEDA3D_VERTEX4D_H
