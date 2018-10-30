#pragma once

#include <vector>

#include "Constants.h"
#include "Directory.h"
#include "Identification.h"
#include "Signature.h"
#include "../headers/Objects/Point2D.h"
#include "WADVertex.h"
#include "WADLinedef.h"
#include "WADSector.h"
#include "WADSidedef.h"

// The base WAD class, it should contain all the information of a WAD.
class WAD {

    WADVertex  vertexes[65536];
    WADLinedef linedefs[65536];
    WADSidedef sidedefs[65536];
    WADSector  sectors[65536];

    int num_vertexes;
    int num_linedefs;
    int num_sidedefs;
    int num_sectors;

    public:
    WAD(char* wadLocation);
    ~WAD();

    bool loadMap(std::string);
    int getIndexLumpByName(std::string lump_name);

    void parseLINEDEFS( Directory );
    void parseVERTEXES( Directory );
    void parseSIDEDEFS( Directory );
    void parseSECTORS( Directory );

    void draw2D();
    void render();

    void parseDoomPicture( wadAddress );

    private:
    Signature signature;
    std::vector<Directory> directories;

    byte* ReadFile(char* fileLocation);


};
