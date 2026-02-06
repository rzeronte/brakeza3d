#include <utility>
#include "../../include/Misc/TexturePackage.h"

TexturePackageItem::TexturePackageItem(Image *texture, std::string label, std::string srcPath)
    : texture(texture), label(std::move(label)), srcPath(std::move(srcPath))
{
}

TexturePackageItem::~TexturePackageItem()
{
    delete texture;
}

void TexturePackage::addItem(const std::string &srcTexture, const std::string& label)
{
    this->items.emplace_back(new TexturePackageItem(new Image(srcTexture), label, srcTexture));
}

Image *TexturePackage::getTextureByLabel(const std::string &label) const
{
    for (unsigned int i = 0; i < this->items.size(); i++) {
        if (items[i]->label == label) {
            return items[i]->texture;
        }
    }
    return nullptr;
}

TexturePackage::~TexturePackage()
{
    Clear();
}

ImTextureID TexturePackage::getOGLTextureID(TexturePackage &package, const std::string &label)
{
    auto* img = package.getTextureByLabel(label);
    if (!img) return nullptr;
    return reinterpret_cast<ImTextureID>(img->getOGLTextureID());
}

void TexturePackage::Clear()
{
    for (auto &item : items) {
        delete item;
    }
    items.clear();
}