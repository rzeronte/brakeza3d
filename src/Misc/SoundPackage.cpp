
#include <cassert>
#include "../../include/Misc/SoundPackage.h"

void SoundPackage::addItem(const std::string &srcSound, std::string label, SoundPackageItemType type)
{
    auto *item = new SoundPackageItem();
    item->type = type;
    item->label = std::move(label);

    Mix_Chunk *sound;
    Mix_Music *music;

    switch (type) {
        case SoundPackageItemType::SOUND:
            sound = Mix_LoadWAV(srcSound.c_str());
            item->sound = sound;
            break;
        case SoundPackageItemType::MUSIC:
            music = Mix_LoadMUS(srcSound.c_str());
            item->music = music;
            break;
        default:
            assert(0);
    }
    this->items.push_back(item);
}

Mix_Chunk *SoundPackage::getByLabel(const std::string &label) {
    for (unsigned int i = 0; i < this->items.size(); i++) {
        if (items[i]->label == label) {
            return items[i]->sound;
        }
    }
    return nullptr;
}

Mix_Music *SoundPackage::getMusicByLabel(const std::string &label) {
    for (unsigned int i = 0; i < this->items.size(); i++) {
        if (items[i]->label == label) {
            return items[i]->music;
        }
    }
    return nullptr;
}
