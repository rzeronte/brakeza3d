
#include "SoundPackage.h"

void SoundPackage::addItem(const std::string &srcTexture, std::string label)
{
    auto *item = new SoundPackageItem();
    Mix_Chunk *snd = Mix_LoadWAV( srcTexture.c_str() );

    item->sound = snd;
    item->label = std::move(label);

    this->items.push_back( item );
}

Mix_Chunk *SoundPackage::getSoundByLabel(const std::string &label)
{
    for (int i = 0; i < this->items.size(); i++ ) {
        if (!items[i]->label.compare(label)) {
            return items[i]->sound;
        }
    }
}
