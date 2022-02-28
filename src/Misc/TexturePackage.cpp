#include "../../include/Misc/TexturePackage.h"

void TexturePackage::addItem(const std::string &srcTexture, std::string label) {
    auto *item = new TexturePackageItem();
    auto *t = new Texture(srcTexture);

    item->texture = t;
    item->label = std::move(label);

    this->items.push_back(item);
}

Texture *TexturePackage::getTextureByLabel(const std::string &label) {
    for (int i = 0; i < this->items.size(); i++) {
        if (items[i]->label == label) {
            return items[i]->texture;
        }
    }
    return nullptr;
}

int TexturePackage::size() {
    return items.size();
}

Texture *TexturePackage::getTextureByIndex(int i) {
    return items[i]->texture;
}
