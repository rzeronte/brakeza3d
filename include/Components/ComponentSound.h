//
// Created by darkhead on 9/1/20.
//

#ifndef BRAKEDA3D_COMPONENTSOUND_H
#define BRAKEDA3D_COMPONENTSOUND_H


#include <SDL2/SDL_events.h>
#include "Component.h"

class ComponentSound : public Component {
public:
    ComponentSound();

    void onStart();

    void preUpdate();

    void onUpdate();

    void postUpdate();

    void onEnd();

    void onSDLPollEvent(SDL_Event *event, bool &finish);

    void initSoundSystem();

    void loadSoundsJSON();

    void playSound(Mix_Chunk *chunk, int channel, int times);

    void playMusic(Mix_Music *music, int loops);

    void stopMusic();
};


#endif //BRAKEDA3D_COMPONENTSOUND_H
