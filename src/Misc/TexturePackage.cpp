#include "../../headers/Misc/TexturePackage.h"

TexturePackage::TexturePackage() {

}

void TexturePackage::addItem(const std::string &srcTexture, std::string label) {
    auto *item = new TexturePackageItem();
    auto *t = new Texture();

    t->loadTGA(srcTexture.c_str(), 1);

    item->texture = t;
    item->label = std::move(label);

    this->items.push_back(item);
}

Texture *TexturePackage::getTextureByLabel(const std::string &label) {
    for (int i = 0; i < this->items.size(); i++) {
        if (!items[i]->label.compare(label)) {
            return items[i]->texture;
        }
    }
    return nullptr;
}
