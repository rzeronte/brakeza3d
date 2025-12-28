//
// Created by darkhead on 9/1/20.
//

#ifndef BRAKEDA3D_COMPONENTSOUND_H
#define BRAKEDA3D_COMPONENTSOUND_H


#include <SDL2/SDL_events.h>
#include "Component.h"
#include "../include/Misc/SoundPackage.h"

class ComponentSound : public Component
{
    SoundPackage soundPackage;
    static int PlayChunk(Mix_Chunk *chunk, int channel, int times);
public:
    ComponentSound();

    void onStart() override;
    void preUpdate() override;
    void onUpdate() override;
    void postUpdate() override;
    void onEnd() override;
    void onSDLPollEvent(SDL_Event *event, bool &finish) override;
    void InitSoundSystem() const;
    void LoadSoundsConfigFile();
    void AddSound(const std::string& soundFile, const std::string& label);
    void AddMusic(const std::string& soundFile, const std::string& label);
    void PlaySound(const std::string& sound, int channel, int times);
    void PlayMusic(const std::string& sound);
    float getSoundDuration(const std::string &sound);
    static void playMusicMix(Mix_Music *music, int loops);
    static void fadeInMusic(Mix_Music *music, int loops, int ms);
    static void StopMusic();
    static void StopChannel(int channel);
    static void setMusicVolume(int v);
    static void setSoundsVolume(int v);
};


#endif //BRAKEDA3D_COMPONENTSOUND_H
