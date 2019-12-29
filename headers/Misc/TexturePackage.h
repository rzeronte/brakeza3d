#ifndef BRAKEZA3D_TEXTUREPACKAGE_H
#define BRAKEZA3D_TEXTUREPACKAGE_H


#include "../Render/Texture.h"
#include <vector>

struct TexturePackageItem {
    Texture *texture;
    std::string label;
};

class TexturePackage {
public:
    TexturePackage();

    void addItem(const std::string& srcTexture, std::string label);

    Texture *getTextureByLabel(const std::string &label);

private:
    std::vector<TexturePackageItem *> items;
};


#endif //BRAKEZA3D_TEXTUREPACKAGE_H
