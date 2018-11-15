//
// Created by darkhead on 2018-11-14.
//

#ifndef BRAKEDA3D_ROTATION3D_H
#define BRAKEDA3D_ROTATION3D_H


#include <string>

class Rotation3D {
public:
    float x, y, z;

    Rotation3D(float x, float y, float z);

    Rotation3D();

    void addRotation(Rotation3D r);
    void consoleInfo(std::string label, bool);

};


#endif //BRAKEDA3D_ROTATION3D_H
