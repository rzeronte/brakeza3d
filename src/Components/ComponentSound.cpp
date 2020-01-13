//
// Created by darkhead on 9/1/20.
//

#include <SDL_mixer.h>
#include "../../headers/Components/ComponentSound.h"
#include <iostream>

ComponentSound::ComponentSound() {

}

void ComponentSound::onStart() {
    std::cout << "ComponentSound onStart" << std::endl;
    this->initSoundSystem();
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

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
        printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
    }

    //Mix_Volume(EngineSetup::SoundChannels::SND_MENU, EngineSetup::getInstance()->SOUND_VOLUME_MENU);
    //Mix_Volume(EngineSetup::SoundChannels::SND_PLAYER, EngineSetup::getInstance()->SOUND_VOLUME_PLAYER);
    //Mix_Volume(EngineSetup::SoundChannels::SND_ENVIRONMENT, EngineSetup::getInstance()->SOUND_VOLUME_ENVIRONMENT);
    //Mix_VolumeMusic(EngineSetup::getInstance()->SOUND_VOLUME_MUSIC);
}


