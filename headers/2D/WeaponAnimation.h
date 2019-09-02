#ifndef BRAKEDA3D_WEAPONANIMATION_H
#define BRAKEDA3D_WEAPONANIMATION_H

#include <string>
#include <SDL.h>
#include "../Render/Timer.h"

#define WEAPON_ANIMATION_MAX_FRAMES 30

class WeaponAnimation {

public:
    WeaponAnimation();

    std::string baseFile;

    int fps = 25;
    int numFrames;
    int currentFrame = 0;

    SDL_Surface *frames[WEAPON_ANIMATION_MAX_FRAMES];

    Timer *timer;
    float last_ticks;
    float timerCurrent = 0;

    int offsetX = 0;
    int offsetY = 0;

    void setup(std::string file, int num_frames, int fps, int offsetX, int offsetY);
    void loadImages();

    int getNumFrames() const;

    SDL_Surface *getCurrentSurface();

    void nextFrame();
    void resetAnimation();

    void draw(SDL_Surface *dst, int globalOffsetX, int headBobOffsetY);

    void updateFrame();

};


#endif //BRAKEDA3D_WEAPONANIMATION_H
