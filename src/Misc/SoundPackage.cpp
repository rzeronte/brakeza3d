
#include <cassert>
#include "../../include/Misc/SoundPackage.h"
#include "../../include/Misc/Logging.h"

// 44100Hz / stereo fijo para todo el catalogo -- igual que el Mix_OpenAudio(44100, ...,
// 2, ...) que abria el dispositivo antes. setChannelFrequency del bridge Lua asume esta
// misma referencia (freq/44100.0f -> pitch).
static ma_decoder_config decoderConfigFor()
{
    return ma_decoder_config_init(ma_format_f32, 2, 44100);
}

SoundPackage::~SoundPackage()
{
    for (auto *item : items) {
        if (item->loaded) {
            ma_audio_buffer_uninit(&item->buffer);
        }
        if (item->pDecodedData) {
            ma_free(item->pDecodedData, nullptr);
        }
        delete item;
    }
}

void SoundPackage::addItem(const std::string &srcSound, std::string label, SoundPackageItemType type)
{
    auto *item = new SoundPackageItem();
    item->type  = type;
    item->label = std::move(label);

    ma_decoder_config cfg = decoderConfigFor();
    ma_uint64 frameCount  = 0;
    void*     pData       = nullptr;

    ma_result result = ma_decode_file(srcSound.c_str(), &cfg, &frameCount, &pData);
    if (result != MA_SUCCESS) {
        LOG_ERROR("[SoundPackage] ma_decode_file failed for '%s' (label='%s'): result=%d",
                   srcSound.c_str(), item->label.c_str(), (int)result);
        this->items.push_back(item);
        return;
    }

    item->pDecodedData = pData;

    ma_audio_buffer_config bufferConfig = ma_audio_buffer_config_init(cfg.format, cfg.channels, frameCount, pData, nullptr);
    bufferConfig.sampleRate = cfg.sampleRate;

    // ma_audio_buffer_init (no _copy): referencia pData sin duplicarlo -- ownsData queda
    // false, la memoria decodificada la libera este SoundPackage (pDecodedData) en el
    // destructor, no ma_audio_buffer_uninit.
    if (ma_audio_buffer_init(&bufferConfig, &item->buffer) == MA_SUCCESS) {
        item->loaded = true;
    } else {
        LOG_ERROR("[SoundPackage] ma_audio_buffer_init failed for '%s'", item->label.c_str());
    }

    this->items.push_back(item);
}

SoundPackageItem *SoundPackage::getByLabel(const std::string &label)
{
    for (unsigned int i = 0; i < this->items.size(); i++) {
        if (items[i]->label == label) {
            return items[i];
        }
    }
    return nullptr;
}
