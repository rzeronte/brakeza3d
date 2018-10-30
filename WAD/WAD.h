#pragma once

#include <vector>

#include "Constants.h"
#include "Directory.h"
#include "Identification.h"
#include "Signature.h"
#include "../headers/Objects/Point2D.h"

// The base WAD class, it should contain all the information of a WAD.
class WAD {

    Point2D *vertices[60000];
    int num_vertices;

    public:
    // Creates a new WAD from the given WAD path.
    WAD(char* wadLocation);
    // Destroys the WAD object.
    ~WAD();

    bool loadMap(std::string);
    int getIndexLumpByName(std::string lump_name);
    void clearName(char *);

    void parseLINEDEFS(Directory );
    void parseVERTEXES(Directory );
    void parseSIDEDEFS(Directory );
    void parseSECTORS(Directory );

    private:
    Signature signature;
    std::vector<Directory> directories;

    // Reads a file an returns an array of its contents.
    byte* ReadFile(char* fileLocation);

    void DoomPicture(wadAddress);

    };
