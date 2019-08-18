
#include "../../headers/2D/WeaponType.h"
#include "../../headers/Render/Logging.h"


WeaponType::WeaponType(std::string label)
{
    this->label = label;

    for (int i = 0; i< WEAPON_MAX_ANIMATIONSS; i++) {
        this->animations[i] = new WeaponAnimation();
    }

    this->hitType = EngineSetup::getInstance()->WeaponsHitTypes::WEAPON_HIT_MELEE;
    this->cadenceTimer.stop();
    this->acumulatedTime = 0;
    this->lastTicks = 0;

}

void WeaponType::addAnimation(std::string animation_folder, int numFrames, int fps, int offsetX, int offsetY)
{
    std::string full_animation_folder = EngineSetup::getInstance()->SPRITES_FOLDER + animation_folder;

    Logging::getInstance()->Log("Loading weapon animation: " + animation_folder + " ("+ std::to_string(numFrames)+" animations)", "BILLBOARD");

    this->animations[this->numAnimations]->setup(full_animation_folder, numFrames, fps, offsetX, offsetY);

    this->animations[this->numAnimations]->loadImages();

    this->numAnimations++;
}

WeaponAnimation * WeaponType::getCurrentWeaponAnimation()
{
    return this->animations[currentAnimationIndex];
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

int WeaponType::getHitType()
{
    return this->hitType;
}

void WeaponType::setHitType(int hitType)
{
    this->hitType = hitType;
    if (this->hitType != EngineSetup::WeaponsHitTypes::WEAPON_HIT_MELEE) {
        this->makeProjectileTemplate();
    } else {
        this->projectile = NULL;
    }
}

void WeaponType::makeProjectileTemplate()
{
    projectile = new SpriteDirectional3D();

    projectile->setPosition(Vertex3D(5, 0, -10));
    std::string spritePath = this->label + "/bullet/idle";
    projectile->addAnimationDirectional2D(spritePath, 1, false, -1);
    projectile->setAnimation(0);
    projectile->width = 5;
    projectile->height = 5;
}

SpriteDirectional3D* WeaponType::getProjectileTemplate()
{
    return this->projectile;
}

void WeaponType::setProjectileSize(float w, float h)
{
    this->projectileWidth = w;
    this->projectileHeight = h;
}

int WeaponType::getDamage()
{
    return this->damage;
}

void WeaponType::setDamage(int damage)
{
    this->damage = damage;
}