#ifndef BRAKEDA3D_WEAPONANIMATION_H
#define BRAKEDA3D_WEAPONANIMATION_H

#include <string>
#include <SDL2/SDL.h>
#include "../Misc/Timer.h"

#define WEAPON_ANIMATION_MAX_FRAMES 30

class WeaponAnimation {

public:
    WeaponAnimation();

    std::string baseFile;

    int fps = 25;
    int numFrames;
    int currentFrame = 0;

    int nextAnimationIndex;

    SDL_Surface *frames[WEAPON_ANIMATION_MAX_FRAMES];

    Timer *timer;
    float last_ticks;
    float timerCurrent = 0;

    bool stopEnd    = false;
    bool looping    = false;
    bool projectile = false;

    int  offsetX = 0;
    int  offsetY = 0;

    bool right   = false;

    void setup(std::string file, int num_frames, int fps, int offsetX, int offsetY, bool right, bool stopEnd, int nextAnimationIndex, bool looping, bool projectile);
    void loadImages();

    int getNumFrames() const;

    SDL_Surface *getCurrentSurface();

    void resetAnimation();

    void draw(SDL_Surface *dst, int globalOffsetX, int headBobOffsetY);

    void updateFrame();

    bool isStopEnd() const;
    void setStopEnd(bool stopEnd);

    int  getNextAnimationIndex() const;
    void setNextAnimationIndex(int nextAnimationIndex);

    bool isLooping() const;
    void setLooping(bool looping);

    bool isProjectile() const;
    void setProjectile(bool projectile);
};


#endif //BRAKEDA3D_WEAPONANIMATION_H
