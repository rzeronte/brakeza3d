//
// Created by darkhead on 9/1/20.
//

#ifndef BRAKEDA3D_COMPONENTSOUND_H
#define BRAKEDA3D_COMPONENTSOUND_H


#include <SDL2/SDL_events.h>
#include "Component.h"
#include "../include/Misc/SoundPackage.h"

// Undefine Windows API macro that conflicts with our PlaySound method
#ifdef PlaySound
#undef PlaySound
#endif

class ComponentSound : public Component
{
public:
    static constexpr int CHANNEL_UI_HOVER = 0;
    static constexpr int MAX_CHANNELS     = 32;

private:
    // Voz de un canal logico del pool. Se reinicializa (ma_sound_uninit + nuevo
    // ma_audio_buffer_ref) cada vez que PlaySound() reutiliza el canal para una
    // etiqueta distinta -- miniaudio no permite "recargar" un ma_sound existente
    // con otra fuente de datos.
    struct Voice {
        ma_sound             sound{};
        ma_audio_buffer_ref  ref{};
        bool                 soundInitialized = false;
        bool                 refInitialized   = false;
        SoundPackageItem*    item             = nullptr;
    };

    SoundPackage soundPackage;
    ma_engine    engine{};
    bool         engineInitialized = false;
    Voice        voices[MAX_CHANNELS];

    // Musica: voz dedicada fuera del pool de 32 canales (nunca la roba stealChannel
    // porque no pasa por PlaySound/voices[]).
    ma_sound             musicVoice{};
    ma_audio_buffer_ref  musicRef{};
    bool                 musicSoundInitialized = false;
    bool                 musicRefInitialized   = false;
    bool                 musicPausedFlag       = false;

    // Escalas de volumen globales -- sustituyen a Mix_Volume(SND_GLOBAL,..)/Mix_VolumeMusic(..),
    // que en SDL_mixer aplicaban instantaneamente a todos los canales/la musica. Aqui se
    // guardan y se multiplican en cada setChannelVolume()/al fijar el volumen de la musica.
    float fxVolumeScale    = 1.0f;
    float musicVolumeScale = 1.0f;

    void PlayBuffer(SoundPackageItem* item, int channel, int times);

public:
    ComponentSound();

    void onStart() override;
    void preUpdate() override;
    void onUpdate() override;
    void postUpdate() override;
    void onEnd() override;
    void onSDLPollEvent(SDL_Event *event, bool &finish) override;
    void InitSoundSystem();
    void LoadSoundsConfigFile();
    void AddSound(const std::string& soundFile, const std::string& label);
    void AddMusic(const std::string& soundFile, const std::string& label);
    void LoadSoundsFromFile(const std::string& filePath);
    void PlaySound(const std::string& sound, int channel, int times);
    void PlayMusic(const std::string& sound);
    float getSoundDuration(const std::string &sound);
    void StopMusic();
    void PauseMusic();
    void ResumeMusic();
    bool isMusicPaused();
    void StopChannel(int channel);
    void setMusicVolume(int v);
    void setSoundsVolume(int v);

    void setChannelFrequency(int channel, int freq);
    void setChannelPitch(int channel, float pitch);
    void setChannelVolume(int channel, int vol);
    void setChannelPosition(int channel, int angle, int distance);
    bool isChannelPlaying(int channel);
    bool isSoundPlaying(const std::string& label);

    // Usado por Sound3D (voz propia fuera del pool, mismo dispositivo/engine que el resto).
    ma_engine* getEngine() { return &engine; }
};


#endif //BRAKEDA3D_COMPONENTSOUND_H
