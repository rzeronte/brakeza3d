
#include <SDL2/SDL_image.h>

#include <utility>
#include "../../include/2D/WeaponAnimation.h"
#include "../../include/EngineSetup.h"
#include "../../include/Render/Logging.h"
#include "../../include/Brakeza3D.h"

WeaponAnimation::WeaponAnimation() {
    timer = new Timer();
    timer->start();
    currentFrame = 0;
}

void
WeaponAnimation::setup(std::string file, int num_frames, int fps, int offsetX, int offsetY, bool right, bool stopEnd,
                       int nextAnimationIndex, bool looping, bool projectile) {
    this->baseFile = std::move(file);
    this->fps = fps;
    this->numFrames = num_frames;
    this->currentFrame = 0;
    this->offsetX = offsetX;
    this->offsetY = offsetY;
    this->right = right;
    this->stopEnd = stopEnd;
    this->nextAnimationIndex = nextAnimationIndex;
    this->looping = looping;
    this->projectile = projectile;
}

void WeaponAnimation::loadImages() {
    for (int i = 0; i < this->getNumFrames(); i++) {
        std::string file = this->baseFile + "/" + std::to_string(i) + ".png";
        Logging::Log("Loading weapon image: " + file, "WeaponAnimation");
        this->frames[i] = IMG_Load(file.c_str());
    }
}

int WeaponAnimation::getNumFrames() const {
    return numFrames;
}

SDL_Surface *WeaponAnimation::getCurrentSurface() {
    return this->frames[currentFrame];
}

void WeaponAnimation::resetAnimation() {
    this->currentFrame = 0;
    this->timerCurrent = 0;
}

void WeaponAnimation::draw(SDL_Surface *dst, int globalOffsetX, int globalOffsetY) {
    SDL_Rect destPos;

    if (this->right) {
        destPos.x = EngineSetup::get()->screenWidth - this->getCurrentSurface()->w;
    } else {
        destPos.x = (EngineSetup::get()->screenWidth / 2) - (this->getCurrentSurface()->w / 2);
    }
    destPos.x += globalOffsetX;
    destPos.x += this->offsetX;

    destPos.y = EngineSetup::get()->screenHeight - this->getCurrentSurface()->h + globalOffsetY;

    if (EngineSetup::get()->DRAW_HUD) {
        destPos.y -= Brakeza3D::get()->getComponentsManager()->getComponentHUD()->HUDTextures->getTextureByLabel(
                "hud")->getImage()->getSurface()->h;
    }

    SDL_BlitSurface(this->frames[currentFrame], nullptr, dst, &destPos);
}

void WeaponAnimation::updateFrame() {
    // Frame secuence control
    float deltatime = this->timer->getTicks() - this->last_ticks;
    this->last_ticks = this->timer->getTicks();
    timerCurrent += (deltatime / 1000.f);

    float step = (float) 1 / this->fps;

    if (timerCurrent > step) {
        timerCurrent = 0;
        currentFrame++;

        if (currentFrame >= this->getNumFrames()) {
            if (isStopEnd()) {
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

bool WeaponAnimation::isProjectile() const {
    return projectile;
}

void WeaponAnimation::setProjectile(bool projectile) {
    WeaponAnimation::projectile = projectile;
}
