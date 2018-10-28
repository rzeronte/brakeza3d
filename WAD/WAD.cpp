#include "WAD.h"
#include "../headers/Render/EngineBuffers.h"
#include "../headers/Render/Color.h"

#include <fstream>
#include <memory>
#include <string>
#include <SDL_surface.h>

namespace Biendeo {
	namespace WAD {
	    char *wadFile;

		WAD::WAD(char* wadLocation) {
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

		bool WAD::Testing() {

            std::unique_ptr<byte[]> wadBinary(ReadFile(wadFile));
            //Directory d = WAD::getIndexLumpByName("STDISK");
            //DoomPicture(d.offData);

            int d = WAD::getIndexLumpByName("E1M1");
            Directory marker_map = directories.at(d);
            Directory things     = directories.at(d+1);
            Directory linedefs   = directories.at(d+2);
            Directory sidedefs   = directories.at(d+3);
            Directory vertexes   = directories.at(d+4);
            Directory segs       = directories.at(d+5);
            Directory ssectors   = directories.at(d+6);
            Directory nodes      = directories.at(d+7);
            Directory sectors    = directories.at(d+8);
            Directory reject     = directories.at(d+9);
            Directory blockmap   = directories.at(d+10);

            parseVERTEXES(vertexes);

            return true;
        }

        void WAD::parseVERTEXES(Directory linedef_vertexes)
        {
            std::unique_ptr<byte[]> wadBinary(ReadFile(wadFile));

            byte *binaryLinedef = &wadBinary.get()[linedef_vertexes.offData];

            linedef_vertexes.consoleInfo();

            int linedef_size_bytes = 4;
            int cont = 0;
            // 4 bytes es el tamaño de un VERTEX
            for (int i = 0; i < linedef_vertexes.sizeData; i+=linedef_size_bytes) {
                signed short x_pos = (signed short) LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryLinedef, i, 2)).get());
                signed short y_pos = (signed short) LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryLinedef, i+2, 2)).get());

                printf("%d) x: %d, y: %d\r\n", cont, x_pos, y_pos);

                cont++;
            }

            signed short int a = -1152;
            printf("%d\r\n", a);

            printf("Size: %d", linedef_vertexes.sizeData/linedef_size_bytes);
        }

        void WAD::parseLINEDEFS(Directory linedefs_lump)
        {
            std::unique_ptr<byte[]> wadBinary(ReadFile(wadFile));

            byte *binaryLinedef = &wadBinary.get()[linedefs_lump.offData];
            int linedef_size_bytes = 14;
            for (int i = 0; i < linedefs_lump.sizeData; i+=linedef_size_bytes) {    // 14 bytes es el tamaño de una LINEDEF
                unsigned short s_vertex = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryLinedef, i, 1)).get());
                unsigned short e_vertex = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(binaryLinedef, i+2, 1)).get());
                printf("V1: %d, V2: %d\r\n", s_vertex, e_vertex);
            }

            printf("Linedefs: %d\r\n", linedefs_lump.sizeData/linedef_size_bytes);
		}

        bool WAD::Write(char* wadLocation) {
            std::fstream wadFile(wadLocation, std::fstream::out | std::fstream::binary);

            wadFile.write((char*)std::unique_ptr<byte[]>(signature.ToBytes()).get(), 12);

            for (Directory& directory : directories) {
                wadFile.write((char*)std::unique_ptr<byte[]>(directory.ToBytes()).get(), 16);
            }

            wadFile.close();

            return true;
        }

        byte* WAD::ReadFile(char* fileLocation) {
            std::fstream wadFile(fileLocation, std::fstream::binary | std::fstream::in | std::fstream::ate);
            wadFile.seekg(0, std::fstream::end);
            int wadSize = static_cast<int>(wadFile.tellg());

            byte* wadBinary = new byte[wadSize];

            wadFile.seekg(0, std::fstream::beg);
            wadFile.read((char*)wadBinary, wadSize);
            wadFile.close();

            return wadBinary;
        }

        int WAD::getIndexLumpByName(std::string lump_name) {
		    int i = 0;
            for (Directory& directory : directories) {
                char	name[9];
                name[8] = 0;

                int j;
                memcpy (name, directory.name, 8);

                for (j=0 ; j<8 ; j++)
                    if (!name[j])
                        break;

                if (strcmp(name, lump_name.c_str()) == 0) {
                    return i;
                }
                i++;
            }
        }

        void WAD::DoomPicture(wadAddress OffsetImage) {
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
    }
}