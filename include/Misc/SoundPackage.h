
#ifndef BRAKEZA3D_SOUNDPACKAGE_H
#define BRAKEZA3D_SOUNDPACKAGE_H

#include <vector>
#include <string>
#include "miniaudio.h"

typedef enum {
    SOUND, MUSIC
} SoundPackageItemType;

// buffer: PCM decodificado una vez en memoria (ma_audio_buffer no copia pDecodedData,
// solo lo referencia -- ver SoundPackage::addItem). loaded=false si el decode fallo
// (buffer.ref queda a cero, pData=nullptr) -- getByLabel() sigue devolviendo el item
// (igual que antes con un Mix_Chunk nulo) para que el caller decida cómo fallar.
struct SoundPackageItem {
    std::string label;
    SoundPackageItemType type;
    ma_audio_buffer buffer{};
    void* pDecodedData = nullptr;
    bool  loaded = false;
};

class SoundPackage {
    std::vector<SoundPackageItem *> items;
public:
    ~SoundPackage();

    void addItem(const std::string &srcSound, std::string label, SoundPackageItemType type);

    SoundPackageItem *getByLabel(const std::string &label);
};


#endif //BRAKEZA3D_SOUNDPACKAGE_H
