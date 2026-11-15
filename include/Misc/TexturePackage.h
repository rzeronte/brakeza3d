/*
 * Brakeza3D - 3D/2D game engine
 * Author:  Eduardo Rodríguez Álvarez
 * GitHub:  https://github.com/rzeronte/brakeza3d
 * Web:     https://brakeza.com
 * Started: 2018-09-30
 * Copyright (C) 2018-2026 Eduardo Rodríguez Álvarez
 * License: GNU General Public License v3.0 (see LICENSE)
 */
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