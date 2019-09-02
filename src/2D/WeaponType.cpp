
#include "../../headers/2D/WeaponType.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Billboard.h"


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

    markTemplate = new Sprite3D();

}

void WeaponType::addAnimation(std::string animation_folder, int numFrames, int fps, int offsetX, int offsetY)
{
    std::string full_animation_folder = EngineSetup::getInstance()->SPRITES_FOLDER + animation_folder;

    Logging::getInstance()->Log("Loading weapon animation: " + animation_folder + " ("+ std::to_string(numFrames)+" animations)", "WeaponType");

    this->animations[this->numAnimations]->setup(full_animation_folder, numFrames, fps, offsetX, offsetY);

    this->animations[this->numAnimations]->loadImages();

    this->numAnimations++;
}

void WeaponType::onUpdate()
{
    updateCadenceTimer();
    getCurrentWeaponAnimation()->updateFrame();
}

void WeaponType::setWeaponAnimation(int animationIndex)
{
    this->currentAnimationIndex = animationIndex;
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

void WeaponType::startAction()
{
    this->cadenceTimer.start();
}

void WeaponType::endAction()
{
    this->cadenceTimer.stop();
    this->acumulatedTime = 0;
    this->lastTicks = 0;
    this->getCurrentWeaponAnimation()->resetAnimation();
}

void WeaponType::updateCadenceTimer()
{
    if (!this->cadenceTimer.isStarted()) return;

    float deltatime = this->cadenceTimer.getTicks() - this->lastTicks;
    this->lastTicks = this->cadenceTimer.getTicks();

    this->acumulatedTime += deltatime / 1000;

    if  (this->acumulatedTime >= this->cadence ) {
        this->endAction();
    }
}

bool WeaponType::isCadenceInProgress()
{
    return this->cadenceTimer.isStarted();
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
        this->projectileTemplate = NULL;
    }
}

void WeaponType::makeProjectileTemplate()
{
    projectileTemplate = new SpriteDirectional3D();

    projectileTemplate->setPosition(Vertex3D(5, 0, -10));
    std::string spritePath = this->label + "/bullet/idle";
    projectileTemplate->addAnimationDirectional2D(spritePath, 1, 20, false, -1);
    projectileTemplate->setAnimation(0);
    projectileTemplate->width = 5;
    projectileTemplate->height = 5;
}

SpriteDirectional3D* WeaponType::getProjectileTemplate()
{
    return this->projectileTemplate;
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

Sprite3D *WeaponType::getMarkTemplate()
{
    return markTemplate;
}

void WeaponType::setupMarkTemplate(std::string path, int numFrames, int fps, float w, float h)
{
    markTemplate->setAutoRemoveAfterAnimation(true);
    markTemplate->setEnabled(true);
    markTemplate->addAnimation(path, numFrames, fps);
    markTemplate->setAnimation(0);
    markTemplate->getBillboard()->setDimensions(w, h);
}

void WeaponType::loadFireSound(std::string file)
{
    Logging::getInstance()->Log("loadFireSound: " + EngineSetup::getInstance()->SOUNDS_FOLDER + file, "WeaponType");

    soundFire = Mix_LoadWAV( (EngineSetup::getInstance()->SOUNDS_FOLDER + file).c_str() );
}

void WeaponType::loadMarkSound(std::string file)
{
    Logging::getInstance()->Log("loadMarkSound: " + EngineSetup::getInstance()->SOUNDS_FOLDER + file, "WeaponType");
    soundMark = Mix_LoadWAV( (EngineSetup::getInstance()->SOUNDS_FOLDER + file).c_str() );
}