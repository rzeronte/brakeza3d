#include "WAD.h"
#include "../headers/Render/EngineBuffers.h"
#include "../headers/Render/Color.h"
#include "../headers/Objects/Line2D.h"
#include "../headers/Render/EngineSetup.h"

#include <fstream>
#include <memory>
#include <string>
#include <SDL_surface.h>


char *wadFile;

WAD::WAD(char* wadLocation) {
    this->num_vertices = 0;
    wadFile = wadLocation;
    // Is it a good thing to load the whole WAD into memory?
    std::unique_ptr<byte[]> wadBinary(ReadFile(wadFile));

    signature = Signature(wadBinary.get());
    {
        wadAddress lumpAddress = signature.infoTableOfS;
        for (size_t i = 0; i < signature.numLumps; ++i) {
            directories.emplace_back(&wadBinary.get()[lumpAddress]);
            lumpAddress += 16;
        }
    }
}

WAD::~WAD() {

}

bool WAD::loadMap(std::string map_name) {

    //std::unique_ptr<byte[]> wadBinary(ReadFile(wadFile));
    //Directory lump_index = WAD::getIndexLumpByName("STDISK");
    //DoomPicture(lump_index.offData);

    int lump_index = WAD::getIndexLumpByName(map_name);
    Directory marker_map = directories.at(lump_index);
    Directory things     = directories.at(lump_index+1);
    Directory linedefs   = directories.at(lump_index+2);
    Directory sidedefs   = directories.at(lump_index+3);
    Directory vertexes   = directories.at(lump_index+4);
    Directory segs       = directories.at(lump_index+5);
    Directory ssectors   = directories.at(lump_index+6);
    Directory nodes      = directories.at(lump_index+7);
    Directory sectors    = directories.at(lump_index+8);
    Directory reject     = directories.at(lump_index+9);
    Directory blockmap   = directories.at(lump_index+10);

    parseVERTEXES(vertexes);
    parseLINEDEFS(linedefs);
    parseSIDEDEFS(sidedefs);
    parseSECTORS(sectors);

    return true;
}

void WAD::parseVERTEXES(Directory linedef_vertexes)
{
    std::unique_ptr<byte[]> wadBinary(ReadFile(wadFile));

    byte *binaryLinedef = &wadBinary.get()[linedef_vertexes.offData];

    int linedef_size_bytes = 4;
    int cont = 0;
    // 4 bytes es el tamaño de un VERTEX
    for (int i = 0; i < linedef_vertexes.sizeData; i+=linedef_size_bytes) {
        signed short x_pos = (signed short) LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryLinedef, i, 2)).get());
        signed short y_pos = (signed short) LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryLinedef, i+2, 2)).get());

        //printf("%d) x: %d, y: %d\r\n", cont, x_pos, y_pos);

        this->vertices[this->num_vertices] = new Point2D(x_pos, y_pos);
        this->num_vertices++;
        cont++;
    }

    //printf("Size: %d", linedef_vertexes.sizeData/linedef_size_bytes);
}

void WAD::parseLINEDEFS(Directory linedefs_lump)
{
    std::unique_ptr<byte[]> wadBinary(ReadFile(wadFile));

    byte *binaryLinedef = &wadBinary.get()[linedefs_lump.offData];

    int linedef_size_bytes = 14;
    int cont = 0;

    for (int i = 0; i < linedefs_lump.sizeData; i+=linedef_size_bytes) {    // 14 bytes es el tamaño de una LINEDEF
        unsigned short s_vertex = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryLinedef, i, 2)).get());
        unsigned short e_vertex = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryLinedef, i+2, 2)).get());

        unsigned short flags = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryLinedef, i+4, 2)).get());

        Point2D *p1 = this->vertices[s_vertex];
        Point2D *p2 = this->vertices[e_vertex];

        //printf("%d) LINEDEF: V1(index): %d, V2(index): %d | V1(%d, %d), V2(%d, %d) | Flags: %d | ", cont, s_vertex, e_vertex, (int)p1->x, (int)p1->y, (int)p2->x, (int)p2->y, flags);

        int reducer = 10;
        Line2D l1 = Line2D(
            p1->x/reducer + EngineSetup::getInstance()->SCREEN_WIDTH/2,
            p1->y/reducer + EngineSetup::getInstance()->SCREEN_HEIGHT/2,
            p2->x/reducer + EngineSetup::getInstance()->SCREEN_WIDTH/2,
            p2->y/reducer + EngineSetup::getInstance()->SCREEN_HEIGHT/2
        );

        short int bits[8];
        for ( int j = 0; j<8; j++ ) {
            bits[j] = (flags >> j) & 1;
        }

        //printf("\r\n");

        l1.draw();
        cont++;
    }

    //printf("Linedefs: %d\r\n", linedefs_lump.sizeData/linedef_size_bytes);
}

void WAD::parseSIDEDEFS(Directory linedef_sidedefs)
{
    std::unique_ptr<byte[]> wadBinary(ReadFile(wadFile));

    byte *binarySIDEDEFS = &wadBinary.get()[linedef_sidedefs.offData];

    int linedef_size_bytes = 30;
    int cont = 0;

    for (int i = 0; i < linedef_sidedefs.sizeData; i+=linedef_size_bytes) {    // 14 bytes es el tamaño de una LINEDEF
        signed short x_offset = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i, 2)).get());
        signed short y_offset = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i+2, 2)).get());

        char texture_upper[8];
        char texture_middle[8];
        char texture_lower[8];

        memcpy(texture_upper, (char*)std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i+4, 8)).get(), 8);
        memcpy(texture_middle, (char*)std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i+12, 8)).get(), 8);
        memcpy(texture_lower, (char*)std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i+20, 8)).get(), 8);

        signed short sector = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i+28, 2)).get());

        printf("%d) SIDEDEF: X Offset: %d, Y Offset: %d: Textures: %s | %s | %s | Sector: %d\r\n", cont, x_offset, y_offset, texture_upper, texture_middle, texture_lower, sector);
        cont++;
    }

    //printf("Linedefs: %d\r\n", linedefs_lump.sizeData/linedef_size_bytes);

}

void WAD::parseSECTORS(Directory linedef_sectors)
{
    std::unique_ptr<byte[]> wadBinary(ReadFile(wadFile));

    byte *binarySIDEDEFS = &wadBinary.get()[linedef_sectors.offData];

    int sector_size_bytes = 26;
    int cont = 0;

    for (int i = 0; i < linedef_sectors.sizeData; i+=sector_size_bytes) {
        signed short floor_height   = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i, 2)).get());
        signed short ceiling_height = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i+2, 2)).get());

        char floor_texture[8];
        char ceiling_texture[8];

        memcpy(floor_texture, (char*)std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i+4, 8)).get(), 8);
        memcpy(ceiling_texture, (char*)std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i+12, 8)).get(), 8);

        signed short light = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i+20, 2)).get());
        signed short special = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i+22, 2)).get());
        signed short tag = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binarySIDEDEFS, i+24, 2)).get());

        printf("%d) SECTOR: Floor H: %d | Ceiling H: %d | Floor T: %s | Ceiling T: %s | Light: %d | Special: %d | Tag: %d \r\n", cont, floor_height, ceiling_height, (char *) floor_texture, (char *) ceiling_texture, light, special, tag);
        cont++;
    }
}

byte* WAD::ReadFile(char* fileLocation)
{
    std::fstream wadFile(fileLocation, std::fstream::binary | std::fstream::in | std::fstream::ate);
    wadFile.seekg(0, std::fstream::end);
    int wadSize = static_cast<int>(wadFile.tellg());

    byte* wadBinary = new byte[wadSize];

    wadFile.seekg(0, std::fstream::beg);
    wadFile.read((char*)wadBinary, wadSize);
    wadFile.close();

    return wadBinary;
}

int WAD::getIndexLumpByName(std::string lump_name)
{
    int i = 0;
    for (Directory& directory : directories) {
        char	name[9];
        name[8] = 0;

        int j;
        memcpy (name, directory.name, 8);

        for (j=0 ; j<8 ; j++)
            if (!name[j])
                break;

        printf("%s\r\n", name);
        if (strcmp(name, lump_name.c_str()) == 0) {
            return i;
        }
        i++;
    }
}

void WAD::DoomPicture(wadAddress OffsetImage)
{
    std::unique_ptr<byte[]> wadBinary(ReadFile(wadFile));

    byte *binaryImage = &wadBinary.get()[OffsetImage];

    int byte_counter = 0;
    int w = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryImage, 0, 2)).get());
    byte_counter+=2;
    int h = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryImage, byte_counter, 2)).get());
    byte_counter+=2;
    int off_x = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryImage, byte_counter, 2)).get());
    byte_counter+=2;
    int off_y = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryImage, byte_counter, 2)).get());
    byte_counter+=2;

    wadAddress columns[w];

    printf("\n\nDoom Image:  width: %d, height: %d\r\n", w, h);

    for (int i = 0; i < w ; i++) {
        columns[i] = (wadAddress) LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryImage, byte_counter, 4)).get());
        byte_counter+=4;
    }

    for (int i = 0; i < w ; i++) {
        uint8_t rowstart = 0;
        int cont = 0;

        rowstart = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryImage, columns[i]+cont, 1)).get());
        if (rowstart == 255)
            break;

        cont++;
        int npixels = (uint8_t) LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryImage, columns[i]+cont, 1)).get());
        cont++;
        int garbage = (uint8_t) LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryImage, columns[i]+cont, 1)).get());
        cont++;

        printf("%d) Offset: %d, NumPixels: %d\r\n", i, rowstart, npixels);

        for (int j = 0 ; j < npixels; j++) {
            int pixel = (int) LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryImage, columns[i]+cont+j, 1)).get());
            printf("Pixel: %d\r\n", pixel);
        }
    }
}
