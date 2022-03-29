#include <SDL2/SDL_mixer.h>
#include "../../include/Components/ComponentSound.h"
#include "../../include/Misc/cJSON.h"

ComponentSound::ComponentSound() {
    initSoundSystem();
}

void ComponentSound::onStart() {
    Logging::Log("ComponentSound onStart", "ComponentSound");
    loadSoundsJSON();
}

void ComponentSound::preUpdate() {

}

void ComponentSound::onUpdate() {

}

void ComponentSound::onEnd() {

}

void ComponentSound::postUpdate() {

}

void ComponentSound::onSDLPollEvent(SDL_Event *e, bool &finish) {
}

void ComponentSound::initSoundSystem() {
    Logging::Log("Init Sound System", "Sound");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }

    Mix_Volume(EngineSetup::SoundChannels::SND_MENU, (int) SETUP->SOUND_VOLUME_MENU);
    Mix_Volume(EngineSetup::SoundChannels::SND_PLAYER, (int) SETUP->SOUND_VOLUME_PLAYER);
    Mix_Volume(EngineSetup::SoundChannels::SND_ENVIRONMENT, (int) SETUP->SOUND_VOLUME_ENVIRONMENT);
    Mix_VolumeMusic((int) SETUP->SOUND_VOLUME_MUSIC);
}

void ComponentSound::loadSoundsJSON() {
    Logging::Log("Loading Sounds in package...", "SOUNDS");

    std::string sndPath = EngineSetup::get()->SOUNDS_FOLDER;
    size_t file_size;
    std::string filePath = EngineSetup::get()->CONFIG_FOLDER + EngineSetup::get()->CFG_SOUNDS;
    const char *mapsFile = Tools::readFile(filePath, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);

    if (myDataJSON == nullptr) {
        Logging::Log(filePath + " can't be loaded", "ERROR");
        return;
    }

    cJSON *soundsJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "sounds");

    cJSON *currentSound;
    cJSON_ArrayForEach(currentSound, soundsJSONList) {
        cJSON *file = cJSON_GetObjectItemCaseSensitive(currentSound, "file");
        cJSON *label = cJSON_GetObjectItemCaseSensitive(currentSound, "label");
        cJSON *type = cJSON_GetObjectItemCaseSensitive(currentSound, "type");

        SoundPackageItemType selectedType = SoundPackageItemType::SOUND;

        if (strcmp(type->valuestring, "music") == 0) selectedType = SoundPackageItemType::MUSIC;
        if (strcmp(type->valuestring, "sound") == 0) selectedType = SoundPackageItemType::SOUND;

        Logging::Log("Loading file sound " + std::string(file->valuestring), "SOUNDS");

        BUFFERS->soundPackage->addItem(sndPath + file->valuestring, label->valuestring, selectedType);
    }
}

void ComponentSound::playSound(Mix_Chunk *chunk, int channel, int times) {
    if (chunk == nullptr) {
        Logging::Log("Error loading chunk sound", "Sound");
        return;
    }

    Mix_PlayChannel(channel, chunk, times);
}

void ComponentSound::playMusic(Mix_Music *music, int loops = -1)
{
    Mix_PlayMusic(music, loops);

}

void ComponentSound::stopMusic()
{
    Mix_HaltMusic();
}

void ComponentSound::stopChannel(int channel) {
    Mix_HaltChannel(channel);
}

