//
// Created by darkhead on 2019-03-14.
//

#ifndef BRAKEDA3D_PIXELFRAGMENT_H
#define BRAKEDA3D_PIXELFRAGMENT_H


#include "../headers/Objects/Point2D.h"

class PixelFragment {

public:

    int x, y;

    float z;

    float tex_u;
    float tex_v;

    float light_u;
    float light_v;

    float w0, w1, w2;
};


#endif //BRAKEDA3D_PIXELFRAGMENT_H
