#ifndef BRAKEZA3D_TEXTUREPACKAGE_H
#define BRAKEZA3D_TEXTUREPACKAGE_H

#include <vector>
#include "../Render/Image.h"

struct TexturePackageItem {
    TexturePackageItem(Image *texture, std::string label, std::string srcPath);
    ~TexturePackageItem();

    Image *texture;
    std::string label;
    std::string srcPath;
};

class TexturePackage {
public:
    void addItem(const std::string &srcTexture, const std::string& label);

    [[nodiscard]] Image *getTextureByLabel(const std::string &label) const;

    [[nodiscard]] int size() const                      { return (int) items.size(); }
    std::vector<TexturePackageItem *> &getItems()       { return items; }

    virtual ~TexturePackage();
    static ImTextureID getOGLTextureID(TexturePackage &package, const std::string &label);
    void Clear();
private:
    std::vector<TexturePackageItem *> items;
};

#endif //BRAKEZA3D_TEXTUREPACKAGE_H