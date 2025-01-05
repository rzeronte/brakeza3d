//
// Created by darkhead on 9/1/20.
//

#ifndef BRAKEDA3D_COMPONENTSOUND_H
#define BRAKEDA3D_COMPONENTSOUND_H


#include <SDL2/SDL_events.h>
#include "Component.h"
#include "../include/Misc/SoundPackage.h"

class ComponentSound : public Component {
private:
    SoundPackage soundPackage;
    int playSound(Mix_Chunk *chunk, int channel, int times);
public:
    ComponentSound();

    void onStart() override;

    void preUpdate() override;

    void onUpdate() override;

    void postUpdate() override;

    void onEnd() override;

    void onSDLPollEvent(SDL_Event *event, bool &finish) override;

    void initSoundSystem();

    void loadSoundsJSON();

    int sound(const std::string& sound, int channel, int times);

    static void playMusic(Mix_Music *music, int loops);

    static void fadeInMusic(Mix_Music *music, int loops, int ms);

    void stopMusic();

    static void stopChannel(int channel);

    [[nodiscard]] SoundPackage &getSoundPackage();

    Mix_Chunk *soundByLabel(const std::string &label);

    float soundDuration(const std::string &sound);
};


#endif //BRAKEDA3D_COMPONENTSOUND_H
