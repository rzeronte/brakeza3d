
#ifndef BRAKEDA3D_WADSIDEDEF_H
#define BRAKEDA3D_WADSIDEDEF_H


class WADSidedef {
public:
    signed short x_offset;
    signed short y_offset;

    char texture_upper[8];
    char texture_middle[8];
    char texture_lower[8];

    signed short sector;
};


#endif //BRAKEDA3D_WADSIDEDEF_H
