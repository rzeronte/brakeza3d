
#ifndef BRAKEDA3D_WADSECTOR_H
#define BRAKEDA3D_WADSECTOR_H


class WADSector {
public:
    signed short floor_height;
    signed short ceiling_height;

    char floor_texture[8];
    char ceiling_texture[8];

    signed short light;
    signed short special;
    signed short tag;
};


#endif //BRAKEDA3D_WADSECTOR_H
