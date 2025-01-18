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

TexturePackage::~TexturePackage()
{
    for (auto &item : items) {
        delete item;
    }
}

ImTextureID TexturePackage::getOGLTextureID(TexturePackage &package, std::string label)
{
    return (ImTextureID) package.getTextureByLabel(label)->getOGLTextureID();
}

std::vector<TexturePackageItem *> &TexturePackage::getItems(){
    return items;
}
