//
// Created by darkhead on 2019-08-09.
//

#include <SDL_image.h>
#include "../../headers/2D/WeaponAnimation.h"
#include "../../headers/Render/EngineSetup.h"
#include "../../headers/Render/Logging.h"


WeaponAnimation::WeaponAnimation()
{
    timer = new Timer();
    timer->start();
    currentFrame = 0;
}

void WeaponAnimation::setup(std::string file, int num_frames, int fps, int offsetX, int offsetY)
{
    this->baseFile = file;
    this->fps = fps;
    this->numFrames = num_frames;
    this->currentFrame = 0;
    this->offsetX = offsetX;
    this->offsetY = offsetY;
}

void WeaponAnimation::loadImages()
{
    for (int i = 0; i < this->getNumFrames() ; i++) {
        std::string file = this->baseFile + "/" + std::to_string(i) + ".png";
        Logging::getInstance()->Log("Loading weapon image: " + file);
        this->frames[i] = IMG_Load(file.c_str());
    }
}

int WeaponAnimation::getNumFrames() const
{
    return numFrames;
}

SDL_Surface *WeaponAnimation::getCurrentSurface()
{
    return this->frames[currentFrame];
}

void WeaponAnimation::nextFrame()
{
    currentFrame++;

    if (currentFrame >= this->getNumFrames()) {
        currentFrame = 0;
    }
}

void WeaponAnimation::draw(SDL_Surface *dst, int globalOffsetX, int globalOffsetY)
{
    SDL_Rect destPos;
    destPos.x = offsetX + globalOffsetX;
    destPos.y = offsetY + globalOffsetY;

    SDL_BlitSurface(this->frames[currentFrame], NULL, dst, &destPos);
}

void WeaponAnimation::updateFrame()
{
    // Frame secuence control
    float deltatime = this->timer->getTicks() - this->last_ticks;
    this->last_ticks = this->timer->getTicks();
    timerCurrent += (deltatime/1000.f);

    float step = (float) 1 / this->fps;

    if (timerCurrent > step) {
        timerCurrent = 0;
        this->nextFrame();
    }
}