
#include "WADLinedef.h"


bool WADLinedef::isTwoSided()
{
    short int bits[8];
    for ( int j = 0; j<8; j++ ) {
        bits[j] = (flags >> j) & 1;
    }

    if (bits[2] == 1)
        return true;

    return false;
}



bool WADLinedef::haveBackSide()
{
    if (this->left_sidedef == 0xFFFF) {
        return false;
    }

    return true;
}

bool WADLinedef::haveFrontSide()
{
    if (this->right_sidedef == 0xFFFF) {
        return false;
    }

    return true;
}

unsigned short WADLinedef::getBackSideIndex()
{
    return this->left_sidedef;
}

unsigned short WADLinedef::getFrontSideIndex()
{
    return this->right_sidedef;
}