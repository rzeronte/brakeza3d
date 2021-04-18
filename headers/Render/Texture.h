
#ifndef SDL2_3D_ENGINE_TEXTURE_H
#define SDL2_3D_ENGINE_TEXTURE_H


#include <SDL2/SDL_surface.h>
#include <string>
#include "../Objects/Point2D.h"
#include "../Objects/Vertex3D.h"


// Mip Texture
struct miptex_t
{
    char name[16];			// Name of the texture
    unsigned int width;		// Width of the texture, must be a multiple of 8 (converted from long!)
    unsigned int height;	// Height of the texture, must be a multiple of 8 (converted from long!)
    unsigned int offset1;	// -> byte texture[width   * height] (converted from long!)
    unsigned int offset2;	// -> byte texture[width/2 * height/2] (converted from long!)
    unsigned int offset4;	// -> byte texture[width/4 * height/4] (converted from long!)
    unsigned int offset8;	// -> byte texture[width/8 * height/8] (converted from long!)
};

class Texture {
private:
    SDL_Surface *mip_mapping_1;
    SDL_Surface *mip_mapping_2;
    SDL_Surface *mip_mapping_4;
    SDL_Surface *mip_mapping_8;

    bool lightMapped;
    bool mipMapped;

    std::string filename;
public:
    bool loaded = false;
    bool liquid = false;
    bool animated = false;
    int  numAnimatedFrames = 0;

    // lightmap info
    float mins[2], maxs[2], minu[2], maxv[2];
    float extents[2];

    // Surfaces for lightmaps
    SDL_Surface *lightmap;
    SDL_Surface *lightmap2;
    SDL_Surface *lightmap3;
    SDL_Surface *lightmap4;
    int numLightmaps = 0;

    Texture();
    Texture(int, int);

    void drawFlat(int, int);
    void drawFlatLightMap(int, int);

    void loadBMP(const char *file, int);
    void loadJPG(const char *file, int);
    bool loadTGA(const char *file, int);
    void loadFromRaw(unsigned int *texture, int, int, int);
    void loadLightmapFromRaw(int frame, unsigned int *texture, int, int);

    SDL_Surface *getSurface(int);
    void consoleInfo();

    const std::string &getFilename() const;
    void setFilename(const std::string &filename);

    float getAreaForVertices(Vertex3D, Vertex3D, Vertex3D, int);

    bool isMipMapped() const;
    void setMipMapped(bool mipMapped);

    bool isLightMapped() const;

    void setLightMapped(bool lightMapped);

};


#endif //SDL2_3D_ENGINE_TEXTURE_H
