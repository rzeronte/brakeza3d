#ifndef BRAKEZA3D_TEXTUREPACKAGE_H
#define BRAKEZA3D_TEXTUREPACKAGE_H


#include <vector>
#include "../Render/Image.h"

struct TexturePackageItem {
    TexturePackageItem(Image *texture, std::string label);
    Image *texture;
    std::string label;
};

class TexturePackage {
public:
    void addItem(const std::string &srcTexture, const std::string& label);
    int size() const;

    Image *getTextureByLabel(const std::string &label) const;

    virtual ~TexturePackage();

    std::vector<TexturePackageItem *> &getItems();

    static ImTextureID getOGLTextureID(TexturePackage &package, const std::string &label);

private:
    std::vector<TexturePackageItem *> items;
};


#endif //BRAKEZA3D_TEXTUREPACKAGE_H
