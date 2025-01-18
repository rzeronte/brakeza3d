#ifndef BRAKEZA3D_TEXTUREPACKAGE_H
#define BRAKEZA3D_TEXTUREPACKAGE_H


#include <vector>
#include "Image.h"

struct TexturePackageItem {
    TexturePackageItem(Image *texture, std::string label);
    Image *texture;
    std::string label;
};

class TexturePackage {
public:
    void addItem(const std::string &srcTexture, const std::string& label);
    int size();

    Image *getTextureByLabel(const std::string &label);

    virtual ~TexturePackage();

    std::vector<TexturePackageItem *> &getItems();

    static ImTextureID getOGLTextureID(TexturePackage &package, std::string label);

private:
    std::vector<TexturePackageItem *> items;
};


#endif //BRAKEZA3D_TEXTUREPACKAGE_H
