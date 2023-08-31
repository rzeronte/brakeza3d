#include <SDL2/SDL_mixer.h>
#include "../../include/Components/ComponentSound.h"
#include "../../include/Misc/cJSON.h"
#include "../../include/Render/Logging.h"
#include "../../include/Misc/Tools.h"

ComponentSound::ComponentSound()
{
    initSoundSystem();
}

void ComponentSound::onStart()
{
    Logging::Log("ComponentSound onStart");
    loadSoundsJSON();
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
    Logging::Log("Init Sound System");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }

    Mix_AllocateChannels(16);
    Mix_VolumeMusic((int) SETUP->SOUND_VOLUME_MUSIC);
    Mix_Volume(EngineSetup::SoundChannels::SND_GLOBAL, (int) SETUP->SOUND_CHANNEL_GLOBAL);
}


void ComponentSound::loadSoundsJSON()
{
    Logging::Log("Loading Sounds in package...");

    size_t file_size;
    auto contentFile = Tools::readFile(EngineSetup::get()->ROOT_FOLDER + EngineSetup::get()->CFG_SOUNDS, file_size);

    cJSON *myDataJSON = cJSON_Parse(contentFile);

    if (myDataJSON == nullptr) {
        Logging::Log("Cannot load sounds JSON file!");
        return;
    }

    cJSON *currentSound;
    cJSON_ArrayForEach(currentSound, cJSON_GetObjectItemCaseSensitive(myDataJSON, "sounds")) {
        cJSON *file = cJSON_GetObjectItemCaseSensitive(currentSound, "file");
        cJSON *label = cJSON_GetObjectItemCaseSensitive(currentSound, "label");
        cJSON *type = cJSON_GetObjectItemCaseSensitive(currentSound, "type");

        SoundPackageItemType selectedType = SoundPackageItemType::SOUND;

        if (strcmp(type->valuestring, "music") == 0) selectedType = SoundPackageItemType::MUSIC;
        if (strcmp(type->valuestring, "sound") == 0) selectedType = SoundPackageItemType::SOUND;

        Logging::Log("Loading file sound %s", file->valuestring);

        soundPackage.addItem(EngineSetup::get()->SOUNDS_FOLDER + file->valuestring, label->valuestring, selectedType);
    }

    cJSON_Delete(myDataJSON);
    free(contentFile);
}

int ComponentSound::playSound(Mix_Chunk *chunk, int channel, int times)
{
    if (chunk == nullptr) {
        Logging::Log("Error loading chunk sound");
        return -1;
    }

    const int resultPlaying = Mix_PlayChannel(channel, chunk, times);

    if (resultPlaying < 0) {
        Logging::Log("No channel available for sound...");
    }

    return resultPlaying;
}

int ComponentSound::sound(const std::string& sound, int channel, int times)
{
    return ComponentSound::playSound(
        soundPackage.getByLabel(sound),
        channel,
        times
    );
}

void ComponentSound::playMusic(Mix_Music *music, int loops = -1)
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

void ComponentSound::stopChannel(int channel) {
    Mix_HaltChannel(channel);
}

SoundPackage &ComponentSound::getSoundPackage() {
    return soundPackage;
}

Mix_Chunk *ComponentSound::soundByLabel(const std::string &label) {
    return soundPackage.getByLabel(label);
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
