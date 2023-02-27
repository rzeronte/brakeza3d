#ifndef BRAKEZA3D_TEXTUREPACKAGE_H
#define BRAKEZA3D_TEXTUREPACKAGE_H


#include "../Render/Texture.h"
#include <vector>

struct TexturePackageItem {
    TexturePackageItem(Texture *texture, std::string label);
    Texture *texture;
    std::string label;
};

class TexturePackage {
public:
    void addItem(const std::string &srcTexture, const std::string& label);
    int size();

    Texture *getTextureByLabel(const std::string &label);
    Texture *getTextureByIndex(int i);

    virtual ~TexturePackage();

private:
    std::vector<TexturePackageItem *> items;
};


#endif //BRAKEZA3D_TEXTUREPACKAGE_H
