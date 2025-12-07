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

class TextureAtlas
{
    int totalWidth = 0;
    int totalHeight = 0;
    int numRows = 0;
    int numColumns = 0;

    bool *mask = nullptr;
    SDL_Surface *atlasSurface = nullptr;
    std::vector<Image *> textures;
    std::vector<TextureAtlasImageInfo> texturesData;

    void AllocateMask(int xpos, int ypos, int width, int height) const;
    [[nodiscard]] bool AllocationCheck(int xpos, int ypos, int width, int height) const;
public:

    TextureAtlas() = default;
    TextureAtlas(int totalWidth, int totalHeight);

    int getIndexByXY(int x, int y) const;

    ~TextureAtlas();
    bool AddToAtlas(Image *texture, const std::string& name);
    void AllocateEmptyMask(int totalWidth, int totalHeight);
    void SavePNG(const std::string& name) const;
    void CreateFromSheet(const std::string &file, int spriteWidth, int spriteHeight);

    [[nodiscard]] int getNumRows() const;

    [[nodiscard]] int getNumColumns() const;

    TextureAtlasImageInfo getAtlasTextureInfoForName(const std::string& name);
    [[nodiscard]] Image *getTextureByIndex(int index) const;

    void setXYByIndex(int index, int &x, int &y) const;

    [[nodiscard]] Image *getTextureByXY(int x, int y) const;

    int getTotalImages() const;
    bool isSafeIconCoords(int x, int y) const;

};


#endif //BRAKEDA3D_TEXTUREATLAS_H


