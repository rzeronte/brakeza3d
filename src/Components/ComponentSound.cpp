#include <SDL2/SDL_mixer.h>

// Undefine Windows API macro that conflicts with our PlaySound method
#ifdef PlaySound
#undef PlaySound
#endif

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
    Mix_Quit();
}

void ComponentSound::postUpdate()
{
    Component::postUpdate();
}

void ComponentSound::onSDLPollEvent(SDL_Event *e, bool &finish)
{
}

void ComponentSound::InitSoundSystem() const
{
    LOG_MESSAGE("[Sound] Init Sound System...");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }

    Mix_AllocateChannels(32);
    Mix_ReserveChannels(1);  // canal 0 reservado para UI hover (no roba auto-allocate)
    Mix_VolumeMusic((int) SETUP->SOUND_VOLUME_MUSIC);
    Mix_Volume(Config::SoundChannels::SND_GLOBAL, (int) SETUP->SOUND_VOLUME_FX);
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

int ComponentSound::PlayChunk(Mix_Chunk *chunk, int channel, int times)
{
    if (!Components::get()->Sound()->isEnabled()) return -1;

    if (chunk == nullptr) {
        LOG_ERROR("[Sound] loading chunk playSound");
        return -1;
    }

    const int resultPlaying = Mix_PlayChannel(channel, chunk, times);

    if (resultPlaying < 0) {
        LOG_MESSAGE("No channel available for playSound...");
    }

    return resultPlaying;
}

void ComponentSound::playMusicMix(Mix_Music *music, int loops = -1)
{
    Mix_PlayMusic(music, loops);
}

void ComponentSound::fadeInMusic(Mix_Music *music, int loops, int ms)
{
    Mix_FadeInMusic(music, loops, ms);
}

void ComponentSound::StopMusic()
{
    Mix_HaltMusic();
}

void ComponentSound::PauseMusic()
{
    if (Mix_PlayingMusic()) {
        Mix_PauseMusic();
    }
}

void ComponentSound::ResumeMusic()
{
    if (Mix_PausedMusic()) {
        Mix_ResumeMusic();
    }
}

bool ComponentSound::isMusicPaused()
{
    return Mix_PausedMusic() != 0;
}

void ComponentSound::StopChannel(int channel)
{
    Mix_HaltChannel(channel);
}

float ComponentSound::getSoundDuration(const std::string& sound)
{
    auto chunk = soundPackage.getByLabel(sound);

    if (!chunk) {
        return 0.0;
    }

    int frequency, channels;
    Uint16 format;

    // Obtiene el formato actual de audio
    if (Mix_QuerySpec(&frequency, &format, &channels) == 0) {
        return 0.0; // No se pudo obtener el formato de audio
    }

    int bytesPerSample;
    switch (format) {
        case AUDIO_U8:
        case AUDIO_S8:
            bytesPerSample = 1;
            break;
        case AUDIO_U16SYS:
        case AUDIO_S16SYS:
            bytesPerSample = 2;
            break;
        case AUDIO_S32SYS:
            bytesPerSample = 4;
            break;
        case AUDIO_F32SYS:
            bytesPerSample = 4;
            break;
        default:
            return 0.0; // Formato no soportado
    }

    // Calcula la duración en segundos
    double totalSamples = chunk->alen / (bytesPerSample * channels);
    return totalSamples / frequency;
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

    playMusicMix(
        soundPackage.getMusicByLabel(sound),
        -1
    );
}

void ComponentSound::PlaySound(const std::string& sound, int channel, int times)
{
    if (!Components::get()->Sound()->isEnabled()) return;

    PlayChunk(
        soundPackage.getByLabel(sound),
        channel,
        times
    );
}

bool ComponentSound::isSoundPlaying(const std::string& label)
{
    Mix_Chunk* chunk = soundPackage.getByLabel(label);
    if (!chunk) return false;
    int numChannels = Mix_AllocateChannels(-1);
    for (int ch = 0; ch < numChannels; ++ch) {
        if (Mix_Playing(ch) && Mix_GetChunk(ch) == chunk)
            return true;
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
    Mix_VolumeMusic(v);
}

void ComponentSound::setSoundsVolume(int v)
{
    if (!Components::get()->Sound()->isEnabled()) {
        LOG_WARNING("[Sound] setSoundsVolume called but Sound component is disabled");
        return;
    }

    LOG_MESSAGE("[Sound] setSoundsVolume: %d", v);
    Config::get()->SOUND_VOLUME_FX = static_cast<float>(v);
    Mix_Volume(Config::SoundChannels::SND_GLOBAL, v);
}

void ComponentSound::setChannelFrequency(int channel, int freq)
{
    // Mix_SetFrequency does not exist in SDL2_mixer — per-channel rate change unsupported
}

void ComponentSound::setChannelVolume(int channel, int vol)
{
    if (!isEnabled()) return;
    Mix_Volume(channel, vol);
}

void ComponentSound::setChannelPosition(int channel, int angle, int distance)
{
    if (!isEnabled()) return;
    Mix_SetPosition(channel, angle, distance);
}

bool ComponentSound::isChannelPlaying(int channel)
{
    if (!isEnabled()) return false;
    return Mix_Playing(channel) != 0;
}
