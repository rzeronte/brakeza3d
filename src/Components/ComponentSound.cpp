// Undefine Windows API macro that conflicts with our PlaySound method
#ifdef PlaySound
#undef PlaySound
#endif

#include <cmath>
#include "../../include/Components/ComponentSound.h"

#include "../../include/Components/Components.h"
#include "../../include/Misc/Logging.h"
#include "../../include/Misc/Tools.h"

ComponentSound::ComponentSound()
{
    InitSoundSystem();
}

void ComponentSound::onStart()
{
    Component::onStart();
    setEnabled(true);
    LoadSoundsConfigFile();
}

void ComponentSound::preUpdate()
{
    Component::preUpdate();
}

void ComponentSound::onUpdate()
{
    Component::onUpdate();
}

void ComponentSound::onEnd()
{
    for (auto &v : voices) {
        if (v.soundInitialized) ma_sound_uninit(&v.sound);
        if (v.refInitialized)   ma_audio_buffer_ref_uninit(&v.ref);
    }
    if (musicSoundInitialized) ma_sound_uninit(&musicVoice);
    if (musicRefInitialized)   ma_audio_buffer_ref_uninit(&musicRef);

    if (engineInitialized) {
        ma_engine_uninit(&engine);
        engineInitialized = false;
    }
}

void ComponentSound::postUpdate()
{
    Component::postUpdate();
}

void ComponentSound::onSDLPollEvent(SDL_Event *e, bool &finish)
{
}

void ComponentSound::InitSoundSystem()
{
    LOG_MESSAGE("[Sound] Init Sound System...");

    ma_result result = ma_engine_init(nullptr, &engine);
    if (result != MA_SUCCESS) {
        printf("miniaudio could not initialize engine! result=%d\n", (int)result);
        return;
    }
    engineInitialized = true;

    fxVolumeScale    = SETUP->SOUND_VOLUME_FX / 128.0f;
    musicVolumeScale = SETUP->SOUND_VOLUME_MUSIC / 128.0f;
}

void ComponentSound::LoadSoundsConfigFile()
{
    auto filePath = Config::get()->CONFIG_FOLDER + Config::get()->DEFAULT_SOUNDS_FILE;
    LOG_MESSAGE("[Sound] Loading Sounds file: (%s)", filePath.c_str());

    auto contentFile = Tools::ReadFile(filePath);

    cJSON *myDataJSON = cJSON_Parse(contentFile);

    if (myDataJSON == nullptr) {
        LOG_MESSAGE("Sound] ERROR: Cannot load sounds JSON file!");
        return;
    }

    cJSON *currentSound;
    cJSON_ArrayForEach(currentSound, cJSON_GetObjectItemCaseSensitive(myDataJSON, "sounds")) {
        cJSON *file = cJSON_GetObjectItemCaseSensitive(currentSound, "file");
        cJSON *label = cJSON_GetObjectItemCaseSensitive(currentSound, "label");
        cJSON *type = cJSON_GetObjectItemCaseSensitive(currentSound, "type");

        SoundPackageItemType selectedType = SOUND;

        if (strcmp(type->valuestring, "music") == 0) selectedType = MUSIC;
        if (strcmp(type->valuestring, "playSound") == 0) selectedType = SOUND;

        LOG_MESSAGE("[Sound] Loading sound file: %s", file->valuestring);

        soundPackage.addItem(Config::get()->SOUNDS_FOLDER + file->valuestring, label->valuestring, selectedType);
    }

    cJSON_Delete(myDataJSON);
    free(contentFile);
}

void ComponentSound::PlayBuffer(SoundPackageItem* item, int channel, int times)
{
    if (!Components::get()->Sound()->isEnabled()) return;
    if (!engineInitialized) return;
    if (channel < 0 || channel >= MAX_CHANNELS) return;

    if (item == nullptr || !item->loaded) {
        LOG_MESSAGE("No channel available for playSound...");
        return;
    }

    Voice &v = voices[channel];
    if (v.soundInitialized) { ma_sound_uninit(&v.sound); v.soundInitialized = false; }
    if (v.refInitialized)   { ma_audio_buffer_ref_uninit(&v.ref); v.refInitialized = false; }

    if (ma_audio_buffer_ref_init(item->buffer.ref.format, item->buffer.ref.channels,
                                  item->buffer.ref.pData, item->buffer.ref.sizeInFrames, &v.ref) != MA_SUCCESS) {
        return;
    }
    v.refInitialized = true;

    if (ma_sound_init_from_data_source(&engine, (ma_data_source*)&v.ref,
                                        MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &v.sound) != MA_SUCCESS) {
        ma_audio_buffer_ref_uninit(&v.ref);
        v.refInitialized = false;
        return;
    }
    v.soundInitialized = true;
    v.item = item;

    ma_sound_set_looping(&v.sound, times == -1 ? MA_TRUE : MA_FALSE);
    ma_sound_set_volume(&v.sound, fxVolumeScale);
    ma_sound_start(&v.sound);
}

void ComponentSound::StopMusic()
{
    if (musicSoundInitialized) ma_sound_stop(&musicVoice);
    musicPausedFlag = false;
}

void ComponentSound::PauseMusic()
{
    if (musicSoundInitialized && ma_sound_is_playing(&musicVoice)) {
        ma_sound_stop(&musicVoice);
        musicPausedFlag = true;
    }
}

void ComponentSound::ResumeMusic()
{
    if (musicSoundInitialized && musicPausedFlag) {
        ma_sound_start(&musicVoice);
        musicPausedFlag = false;
    }
}

bool ComponentSound::isMusicPaused()
{
    return musicPausedFlag;
}

void ComponentSound::StopChannel(int channel)
{
    if (channel < 0 || channel >= MAX_CHANNELS) return;
    Voice &v = voices[channel];
    if (v.soundInitialized) ma_sound_stop(&v.sound);
}

float ComponentSound::getSoundDuration(const std::string& sound)
{
    auto* item = soundPackage.getByLabel(sound);

    if (!item || !item->loaded) {
        return 0.0;
    }

    ma_uint32 sampleRate = item->buffer.ref.sampleRate;
    if (sampleRate == 0) return 0.0;

    return (float)item->buffer.ref.sizeInFrames / (float)sampleRate;
}

void ComponentSound::LoadSoundsFromFile(const std::string& filePath)
{
    LOG_MESSAGE("[Sound] Loading sounds from: %s", filePath.c_str());

    auto contentFile = Tools::ReadFile(filePath);
    cJSON *root = cJSON_Parse(contentFile);

    if (root == nullptr) {
        LOG_ERROR("[Sound] Cannot parse JSON: %s", filePath.c_str());
        free(contentFile);
        return;
    }

    cJSON *currentSound;
    cJSON_ArrayForEach(currentSound, cJSON_GetObjectItemCaseSensitive(root, "sounds")) {
        cJSON *file  = cJSON_GetObjectItemCaseSensitive(currentSound, "file");
        cJSON *label = cJSON_GetObjectItemCaseSensitive(currentSound, "label");
        cJSON *type  = cJSON_GetObjectItemCaseSensitive(currentSound, "type");

        if (!file || !label || !type) continue;

        SoundPackageItemType selectedType = (strcmp(type->valuestring, "music") == 0) ? MUSIC : SOUND;

        LOG_MESSAGE("[Sound] Loading: %s -> %s", file->valuestring, label->valuestring);
        soundPackage.addItem(Config::get()->SOUNDS_FOLDER + file->valuestring, label->valuestring, selectedType);
    }

    cJSON_Delete(root);
    free(contentFile);
}

void ComponentSound::AddSound(const std::string &soundFile, const std::string &label)
{
    soundPackage.addItem(soundFile, label, SOUND);
}

void ComponentSound::AddMusic(const std::string &soundFile, const std::string &label)
{
    soundPackage.addItem(soundFile, label, MUSIC);
}

void ComponentSound::PlayMusic(const std::string& sound)
{
    if (!Components::get()->Sound()->isEnabled()) return;
    if (!engineInitialized) return;

    auto* item = soundPackage.getByLabel(sound);
    if (item == nullptr || !item->loaded) {
        LOG_MESSAGE("[Sound] PlayMusic: '%s' not loaded", sound.c_str());
        return;
    }

    if (musicSoundInitialized) { ma_sound_uninit(&musicVoice); musicSoundInitialized = false; }
    if (musicRefInitialized)   { ma_audio_buffer_ref_uninit(&musicRef); musicRefInitialized = false; }

    if (ma_audio_buffer_ref_init(item->buffer.ref.format, item->buffer.ref.channels,
                                  item->buffer.ref.pData, item->buffer.ref.sizeInFrames, &musicRef) != MA_SUCCESS) {
        return;
    }
    musicRefInitialized = true;

    if (ma_sound_init_from_data_source(&engine, (ma_data_source*)&musicRef,
                                        MA_SOUND_FLAG_NO_SPATIALIZATION, nullptr, &musicVoice) != MA_SUCCESS) {
        ma_audio_buffer_ref_uninit(&musicRef);
        musicRefInitialized = false;
        return;
    }
    musicSoundInitialized = true;

    ma_sound_set_looping(&musicVoice, MA_TRUE);
    ma_sound_set_volume(&musicVoice, musicVolumeScale);
    ma_sound_start(&musicVoice);
    musicPausedFlag = false;
}

void ComponentSound::PlaySound(const std::string& sound, int channel, int times)
{
    if (!Components::get()->Sound()->isEnabled()) return;

    PlayBuffer(
        soundPackage.getByLabel(sound),
        channel,
        times
    );
}

bool ComponentSound::isSoundPlaying(const std::string& label)
{
    auto* item = soundPackage.getByLabel(label);
    if (!item || !item->loaded) return false;

    for (auto &v : voices) {
        if (v.soundInitialized && v.item == item && ma_sound_is_playing(&v.sound)) {
            return true;
        }
    }
    return false;
}

void ComponentSound::setMusicVolume(int v)
{
    if (!Components::get()->Sound()->isEnabled()) {
        LOG_WARNING("[Sound] setMusicVolume called but Sound component is disabled");
        return;
    }

    LOG_MESSAGE("[Sound] setMusicVolume: %d", v);
    Config::get()->SOUND_VOLUME_MUSIC = static_cast<float>(v);
    musicVolumeScale = v / 128.0f;
    if (musicSoundInitialized) ma_sound_set_volume(&musicVoice, musicVolumeScale);
}

void ComponentSound::setSoundsVolume(int v)
{
    if (!Components::get()->Sound()->isEnabled()) {
        LOG_WARNING("[Sound] setSoundsVolume called but Sound component is disabled");
        return;
    }

    LOG_MESSAGE("[Sound] setSoundsVolume: %d", v);
    Config::get()->SOUND_VOLUME_FX = static_cast<float>(v);
    fxVolumeScale = v / 128.0f;
}

void ComponentSound::setChannelFrequency(int channel, int freq)
{
    // Referencia 44100Hz -- misma que usa SoundPackage para decodificar todo el catalogo.
    setChannelPitch(channel, freq / 44100.0f);
}

void ComponentSound::setChannelPitch(int channel, float pitch)
{
    if (!isEnabled()) return;
    if (channel < 0 || channel >= MAX_CHANNELS) return;
    Voice &v = voices[channel];
    if (v.soundInitialized) ma_sound_set_pitch(&v.sound, pitch);
}

void ComponentSound::setChannelVolume(int channel, int vol)
{
    if (!isEnabled()) return;
    if (channel < 0 || channel >= MAX_CHANNELS) return;
    Voice &v = voices[channel];
    if (v.soundInitialized) ma_sound_set_volume(&v.sound, (vol / 128.0f) * fxVolumeScale);
}

void ComponentSound::setChannelPosition(int channel, int angle, int distance)
{
    if (!isEnabled()) return;
    if (channel < 0 || channel >= MAX_CHANNELS) return;
    Voice &v = voices[channel];
    if (!v.soundInitialized) return;

    // El volumen por distancia ya lo reduce Lua via setChannelVolume (ver miniaudio-migration.md)
    // -- aqui solo se traduce el angulo (convencion SDL_mixer: 0=frente, 90=derecha,
    // 180=detras, 270=izquierda) a pan estereo -1..1.
    (void)distance;
    float rad = angle * (3.14159265f / 180.0f);
    ma_sound_set_pan(&v.sound, sinf(rad));
}

bool ComponentSound::isChannelPlaying(int channel)
{
    if (!isEnabled()) return false;
    if (channel < 0 || channel >= MAX_CHANNELS) return false;
    Voice &v = voices[channel];
    return v.soundInitialized && ma_sound_is_playing(&v.sound);
}
