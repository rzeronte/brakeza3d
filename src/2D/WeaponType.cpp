
#include "../../headers/2D/WeaponType.h"
#include "../../headers/Render/Logging.h"


WeaponType::WeaponType(std::string label)
{
    this->label = label;

    for (int i = 0; i< WEAPON_MAX_ANIMATIONSS; i++) {
        this->animations[i] = new WeaponAnimation();
    }

    this->cadenceTimer.stop();
    this->acumulatedTime = 0;
    this->lastTicks = 0;
}

void WeaponType::addAnimation(std::string animation_folder, int numFrames, int offsetX, int offsetY)
{
    std::string full_animation_folder = EngineSetup::getInstance()->SPRITES_FOLDER + animation_folder;

    Logging::getInstance()->Log("Loading weapon animation: " + animation_folder + " ("+ std::to_string(numFrames)+" animations)", "BILLBOARD");

    this->animations[this->num_animations]->setup(full_animation_folder, numFrames, offsetX, offsetY);

    this->animations[this->num_animations]->loadImages();

    this->num_animations++;
}

WeaponAnimation * WeaponType::getCurrentWeaponAnimation()
{
    return this->animations[current_animation];
}

void WeaponType::setCadence(float cadence)
{
    this->cadence = cadence;
}

void WeaponType::setSpeed(float speed)
{
    this->speed = speed;
}

void WeaponType::startFireAction()
{
    this->cadenceTimer.start();
}

void WeaponType::updateCadenceTimer()
{
    if (!this->cadenceTimer.isStarted()) return;
    float deltatime = this->cadenceTimer.getTicks() - this->lastTicks;
    this->lastTicks = this->cadenceTimer.getTicks();

    this->acumulatedTime += deltatime / 1000;

    Logging::getInstance()->Log(std::to_string(this->acumulatedTime));

    if  (this->acumulatedTime >= this->cadence ) {
        this->cadenceTimer.stop();
        this->acumulatedTime = 0;
        this->lastTicks = 0;
    }
}

bool WeaponType::cadenceTimerTest()
{
    return !this->cadenceTimer.isStarted();
}