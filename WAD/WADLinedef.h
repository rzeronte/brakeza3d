
#ifndef BRAKEDA3D_WADLINEDEF_H
#define BRAKEDA3D_WADLINEDEF_H


#include "WADSidedef.h"
#include "WADVertex.h"

class WADLinedef {
public:
    unsigned short start_vertex;
    unsigned short end_vertex;
    unsigned short flags;
    unsigned short type;
    unsigned short sector_tag;
    unsigned short right_sidedef;
    unsigned short left_sidedef;

    bool isTwoSided();
    bool haveBackSide();    // back
    bool haveFrontSide();   // front

    unsigned short getBackSideIndex();
    unsigned short getFrontSideIndex();
};


#endif //BRAKEDA3D_WADLINEDEF_H
