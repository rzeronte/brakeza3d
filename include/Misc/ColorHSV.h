
#ifndef BRAKEDA3D_COLORHSV_H
#define BRAKEDA3D_COLORHSV_H

#include <cmath>
#include "Color.h"

class ColorHSV {
public:
    ColorHSV();

    ColorHSV(int i, int i1, int i2);

    float h;       // angle in degrees
    float s;       // a fraction between 0 and 1
    float v;       // a fraction between 0 and 1

};




#endif //BRAKEDA3D_COLORHSV_H
