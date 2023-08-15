#include <utility>

#include "../../include/Misc/TexturePackage.h"

TexturePackageItem::TexturePackageItem(Image *texture, std::string label) : texture(texture), label(std::move(label)) {
}


void TexturePackage::addItem(const std::string &srcTexture, const std::string& label) {
    this->items.emplace_back(
        new TexturePackageItem(new Image(srcTexture), label)
    );
}

Image *TexturePackage::getTextureByLabel(const std::string &label)
{
    for (int i = 0; i < (int) this->items.size(); i++) {
        if (items[i]->label == label) {
            return items[i]->texture;
        }
    }
    return nullptr;
}

int TexturePackage::size() {
    return (int) items.size();
}

Image *TexturePackage::getTextureByIndex(int i) {
    return items[i]->texture;
}

TexturePackage::~TexturePackage()
{
    for (auto &item : items) {
        delete item;
    }
}
