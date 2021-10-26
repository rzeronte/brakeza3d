//
// Created by darkhead on 21/5/20.
//

#ifndef BRAKEDA3D_TEXTUREATLAS_H
#define BRAKEDA3D_TEXTUREATLAS_H

#include "../Render/Texture.h"
#include <vector>

struct TextureAtlasImageInfo {
    std::string name;
    float x;
    float y;
    float width;
    float height;
};


class TextureAtlas {

    bool *mask;

public:
    TextureAtlas(int totalWidth, int totalHeight);

    bool addTexture(Texture *texture, bool lightmap, const std::string& name);

    std::vector<Texture *> textures;
    std::vector<TextureAtlasImageInfo> textures_info;

    TextureAtlasImageInfo getAtlasTextureInfoForName(const std::string& name);

    void saveJPG(const std::string& name) const;

    int total_width;
    int total_height;
    SDL_Surface *atlas_surface;

private:

    bool checkForAllocate(int xpos, int ypos, int width, int height);

    void allocateMask(int xpos, int ypos, int width, int height);


public:
    SDL_Surface *getAtlasSurface() const;
};


#endif //BRAKEDA3D_TEXTUREATLAS_H


