#include <SDL2/SDL_mixer.h>
#include "../../include/Components/ComponentSound.h"
#include "../../include/Render/Logging.h"
#include "../../include/Misc/Tools.h"

ComponentSound::ComponentSound()
{
    initSoundSystem();
}

void ComponentSound::onStart()
{
    Logging::Message("ComponentSound onStart");
    LoadSoundsConfigFile();
}

void ComponentSound::preUpdate()
{
}

void ComponentSound::onUpdate() {

}

void ComponentSound::onEnd()
{
    Mix_Quit();
}

void ComponentSound::postUpdate() {

}

void ComponentSound::onSDLPollEvent(SDL_Event *e, bool &finish) {
}

void ComponentSound::initSoundSystem()
{
    Logging::Message("Init Sound System");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }

    Mix_AllocateChannels(16);
    Mix_VolumeMusic((int) SETUP->SOUND_VOLUME_MUSIC);
    Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, (int) SETUP->SOUND_CHANNEL_GLOBAL);
}


void ComponentSound::LoadSoundsConfigFile()
{
    auto filePath = EngineSetup::get()->CONFIG_FOLDER + EngineSetup::get()->DEFAULT_SOUNDS_FILE;
    Logging::Message("Loading Sounds (%s)", filePath.c_str());

    size_t file_size;
    auto contentFile = Tools::readFile(filePath, file_size);

    cJSON *myDataJSON = cJSON_Parse(contentFile);

    if (myDataJSON == nullptr) {
        Logging::Message("Cannot load sounds JSON file!");
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

        Logging::Message("Loading sound file: %s", file->valuestring);

        soundPackage.addItem(EngineSetup::get()->SOUNDS_FOLDER + file->valuestring, label->valuestring, selectedType);
    }

    cJSON_Delete(myDataJSON);
    free(contentFile);
}

int ComponentSound::playChunk(Mix_Chunk *chunk, int channel, int times)
{
    if (chunk == nullptr) {
        Logging::Message("Error loading chunk playSound");
        return -1;
    }

    const int resultPlaying = Mix_PlayChannel(channel, chunk, times);

    if (resultPlaying < 0) {
        Logging::Message("No channel available for playSound...");
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

void ComponentSound::stopMusic()
{
    Mix_HaltMusic();
}

void ComponentSound::stopChannel(int channel)
{
    Mix_HaltChannel(channel);
}

float ComponentSound::soundDuration(const std::string& sound)
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

void ComponentSound::addSound(const std::string &soundFile, const std::string &label)
{
    soundPackage.addItem(soundFile, label, SOUND);
}

void ComponentSound::addMusic(const std::string &soundFile, const std::string &label)
{
    soundPackage.addItem(soundFile, label, MUSIC);
}

void ComponentSound::playMusic(const std::string& sound)
{
    playMusicMix(
        soundPackage.getMusicByLabel(sound),
        -1
    );
}

void ComponentSound::playSound(const std::string& sound, int channel, int times)
{
     playChunk(
        soundPackage.getByLabel(sound),
        channel,
        times
    );
}

void ComponentSound::setMusicVolume(int v)
{
    Mix_VolumeMusic(static_cast<int>(EngineSetup::get()->SOUND_VOLUME_MUSIC));
}

void ComponentSound::setSoundsVolume(int v)
{
    Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, static_cast<int>(EngineSetup::get()->SOUND_CHANNEL_GLOBAL));
}
