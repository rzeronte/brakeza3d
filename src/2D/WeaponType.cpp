
#include "../../headers/2D/WeaponType.h"
#include "../../headers/Render/Logging.h"


WeaponType::WeaponType(std::string label)
{
    this->label = label;

    for (int i = 0; i< WEAPON_MAX_ANIMATIONSS; i++) {
        this->animations[i] = new WeaponAnimation();
    }
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