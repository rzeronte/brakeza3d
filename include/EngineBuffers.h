#ifndef SDL2_3D_ENGINE_ZBUFFER_H
#define SDL2_3D_ENGINE_ZBUFFER_H

#include <iostream>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include "Misc/Timer.h"
#include "Objects/Sprite3D.h"
#include "Game/NPCEnemyBody.h"
#include "Misc/SoundPackage.h"

// Singleton
class EngineBuffers {

private:
    static EngineBuffers *instance;

    EngineBuffers();

public:
    static EngineBuffers *getInstance();

    float *depthBuffer;
    Uint32 *videoBuffer;
    int sizeBuffers;

    Uint32 *HUDbuffer;

    int widthVideoBuffer = EngineSetup::get()->screenWidth;

    SoundPackage *soundPackage;

    std::vector<NPCEnemyBody *> enemyTemplates;

    NPCEnemyBody *getEnemyTemplateForClassname(const std::string& classname);

    void clearDepthBuffer() const;

    float getDepthBuffer(int x, int y) const;

    float getDepthBuffer(int i) const;

    void setDepthBuffer(const int x, const int y, const float value) const;

    void setDepthBuffer(const int i, const float value) const;

    float getVideoBuffer(int x, int y) const;

    void clearVideoBuffer() const;

    void setVideoBuffer(const int x, const int y, Uint32 value) const;

    void setVideoBuffer(const int i, Uint32 value) const;

    void flipVideoBufferToSurface(SDL_Surface *);
};

#endif