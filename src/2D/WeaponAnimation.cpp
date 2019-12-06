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

void WeaponAnimation::setup(std::string file, int num_frames, int fps, int offsetX, int offsetY, bool right, bool stopEnd, int nextAnimationIndex, bool looping)
{
    this->baseFile = file;
    this->fps = fps;
    this->numFrames = num_frames;
    this->currentFrame = 0;
    this->offsetX = offsetX;
    this->offsetY = offsetY;
    this->right = right;
    this->stopEnd = stopEnd;
    this->nextAnimationIndex = nextAnimationIndex;
    this->looping = looping;
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

void WeaponAnimation::resetAnimation()
{
    this->currentFrame = 0;
    this->timerCurrent = 0;
}

void WeaponAnimation::draw(SDL_Surface *dst, int globalOffsetX, int globalOffsetY)
{
    SDL_Rect destPos;

    if (this->right) {
        destPos.x = EngineSetup::getInstance()->screenWidth - this->getCurrentSurface()->w;
    } else {
        destPos.x = (EngineSetup::getInstance()->screenWidth / 2) - (this->getCurrentSurface()->w/2);
    }

    destPos.y = EngineSetup::getInstance()->screenHeight - this->getCurrentSurface()->h + globalOffsetY;

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
        currentFrame++;

        if (currentFrame >= this->getNumFrames()) {
            if ( isStopEnd() ) {
                currentFrame--;
                return;
            }

            currentFrame = 0;
        }
    }
}

bool WeaponAnimation::isStopEnd() const {
    return stopEnd;
}

void WeaponAnimation::setStopEnd(bool stopEnd) {
    WeaponAnimation::stopEnd = stopEnd;
}

int WeaponAnimation::getNextAnimationIndex() const {
    return nextAnimationIndex;
}

void WeaponAnimation::setNextAnimationIndex(int nextAnimationIndex) {
    WeaponAnimation::nextAnimationIndex = nextAnimationIndex;
}

bool WeaponAnimation::isLooping() const {
    return looping;
}

void WeaponAnimation::setLooping(bool looping) {
    WeaponAnimation::looping = looping;
}
