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
}

void WeaponAnimation::setup(std::string file, int num_frames, int offsetX, int offsetY)
{
    this->base_file = file;
    this->n_frames = num_frames;
    this->current = 0;
    this->offsetX = offsetX;
    this->offsetY = offsetY;
}

void WeaponAnimation::loadImages()
{
    for (int i = 0; i < this->getNumFrames() ; i++) {
        std::string file = this->base_file + "/" + std::to_string(i) + ".png";
        Logging::getInstance()->Log("Loading weapon image: " + file);
        this->frames[i] = IMG_Load(file.c_str());
    }
}

int WeaponAnimation::getNumFrames() const
{
    return n_frames;
}

SDL_Surface *WeaponAnimation::getCurrentSurface()
{
    return this->frames[current];
}

void WeaponAnimation::nextFrame()
{
    current++;

    if (current >= this->getNumFrames()) {
        current = 0;
    }
}

void WeaponAnimation::draw(SDL_Surface *dst)
{
    this->updateFrame();
    SDL_Rect destPos;
    destPos.x = offsetX;
    destPos.y = offsetY;

    SDL_BlitSurface(this->frames[current], NULL, dst, &destPos);
}

void WeaponAnimation::updateFrame()
{
    // Frame secuence control
    float deltatime = this->timer->getTicks() - this->last_ticks;
    this->last_ticks = this->timer->getTicks();
    timerCurrent += (deltatime/1000.f);

    float step = (float) 1 / this->fps;

    if (timerCurrent > step) {
        this->nextFrame();
        timerCurrent = 0;
    }
}