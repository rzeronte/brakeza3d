//
// Created by darkhead on 21/5/20.
//

#ifndef BRAKEDA3D_TEXTUREATLAS_H
#define BRAKEDA3D_TEXTUREATLAS_H

#include <vector>
#include "../Render/Image.h"

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

    bool addTexture(Image *texture, const std::string& name);

    std::vector<Image *> textures;
    std::vector<TextureAtlasImageInfo> textures_info;

    TextureAtlasImageInfo getAtlasTextureInfoForName(const std::string& name);

    void saveJPG(const std::string& name) const;

    int total_width;
    int total_height;
    SDL_Surface *atlas_surface;

private:

    bool checkForAllocate(int xpos, int ypos, int width, int height) const;

    void allocateMask(int xpos, int ypos, int width, int height);

public:
    [[nodiscard]] SDL_Surface *getAtlasSurface() const;
};


#endif //BRAKEDA3D_TEXTUREATLAS_H


