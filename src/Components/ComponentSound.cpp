//
// Created by darkhead on 9/1/20.
//

#include <SDL_mixer.h>
#include "../../headers/Components/ComponentSound.h"
#include "../../headers/Misc/cJSON.h"
#include <iostream>

ComponentSound::ComponentSound() {
    initSoundSystem();
}

void ComponentSound::onStart() {
    std::cout << "ComponentSound onStart" << std::endl;
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
    Logging::getInstance()->Log("Init Sound System");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }

    Mix_Volume(EngineSetup::SoundChannels::SND_MENU, SETUP->SOUND_VOLUME_MENU);
    Mix_Volume(EngineSetup::SoundChannels::SND_PLAYER, SETUP->SOUND_VOLUME_PLAYER);
    Mix_Volume(EngineSetup::SoundChannels::SND_ENVIRONMENT, SETUP->SOUND_VOLUME_ENVIRONMENT);
    Mix_VolumeMusic(SETUP->SOUND_VOLUME_MUSIC);
}

void ComponentSound::loadSoundsJSON() {
    Logging::getInstance()->Log("Loading Sounds in package...", "SOUNDS");

    std::string sndPath = EngineSetup::getInstance()->SOUNDS_FOLDER;
    size_t file_size;
    std::string filePath = EngineSetup::getInstance()->CONFIG_FOLDER + EngineSetup::getInstance()->CFG_SOUNDS;
    const char *mapsFile = Tools::readFile(filePath, file_size);
    cJSON *myDataJSON = cJSON_Parse(mapsFile);

    if (myDataJSON == NULL) {
        Logging::getInstance()->Log(filePath + " can't be loaded", "ERROR");
        return;
    }

    cJSON *soundsJSONList = cJSON_GetObjectItemCaseSensitive(myDataJSON, "sounds");

    cJSON *currentSound;
    cJSON_ArrayForEach(currentSound, soundsJSONList) {
        cJSON *file = cJSON_GetObjectItemCaseSensitive(currentSound, "file");
        cJSON *label = cJSON_GetObjectItemCaseSensitive(currentSound, "label");
        cJSON *type = cJSON_GetObjectItemCaseSensitive(currentSound, "type");

        SoundPackageItemType selectedType;
        if (strcmp(type->valuestring, "music") == 0) selectedType = SoundPackageItemType::MUSIC;
        if (strcmp(type->valuestring, "sound") == 0) selectedType = SoundPackageItemType::SOUND;
        Logging::getInstance()->Log("Loading file sound " + std::string(file->valuestring), "SOUNDS");

        BUFFERS->soundPackage->addItem(sndPath + file->valuestring, label->valuestring, selectedType);
    }
}

