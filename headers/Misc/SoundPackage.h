
#ifndef BRAKEZA3D_SOUNDPACKAGE_H
#define BRAKEZA3D_SOUNDPACKAGE_H

#include <vector>
#include <string>
#include <SDL2/SDL_mixer.h>

typedef enum { SOUND, MUSIC } SoundPackageItemType;

struct SoundPackageItem {
    std::string          label;
    Mix_Music            *music;
    Mix_Chunk            *sound;
    SoundPackageItemType type;
};

class SoundPackage {
    std::vector<SoundPackageItem *> items;
public:
    void addItem(const std::string& srcSound, std::string label, SoundPackageItemType type);
    Mix_Chunk *getSoundByLabel(const std::string &label);
    Mix_Music *getMusicByLabel(const std::string &label);
};


#endif //BRAKEZA3D_SOUNDPACKAGE_H
