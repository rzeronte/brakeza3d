
#ifndef BRAKEZA3D_SOUNDPACKAGE_H
#define BRAKEZA3D_SOUNDPACKAGE_H

#include <vector>
#include <string>
#include <SDL_mixer.h>

struct SoundPackageItem {
    Mix_Chunk  *sound;
    std::string label;
};

class SoundPackage {
    std::vector<SoundPackageItem *> items;
public:
    void addItem(const std::string& srcTexture, std::string label);
    Mix_Chunk *getSoundByLabel(const std::string &label);
};


#endif //BRAKEZA3D_SOUNDPACKAGE_H
