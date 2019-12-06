
#include <SDL_image.h>
#include "../../headers/2D/WeaponType.h"
#include "../../headers/Render/Logging.h"

WeaponType::WeaponType() {
}

WeaponType::WeaponType(std::string label)
{
    this->label = label;
    this->iconHUD = SDL_CreateRGBSurface(0, 100, 100, 32, 0, 0, 0, 0);

    for (int i = 0; i < WEAPON_MAX_ANIMATIONS; i++) {
        this->animations[i] = new WeaponAnimation();
    }

    markTemplate = new Sprite3D();
}

void WeaponType::addAnimation(std::string animation_folder, int numFrames, int fps, int offsetX, int offsetY, bool right, bool stopEnd, int next, bool looping)
{
    std::string full_animation_folder = EngineSetup::getInstance()->SPRITES_FOLDER + animation_folder;

    Logging::getInstance()->Log("Loading weapon animation: " + animation_folder + " ("+ std::to_string(numFrames)+" frames)", "WeaponType");

    this->animations[this->numAnimations]->setup(full_animation_folder, numFrames, fps, offsetX, offsetY, right, stopEnd, next, looping);

    this->animations[this->numAnimations]->loadImages();

    this->numAnimations++;
}

void WeaponType::onUpdate()
{
    if (status != 0) {
        Logging::getInstance()->Log("WeaponType status: " + std::to_string(status) + ", time: " + std::to_string(fireCounters[status].getStep()) + ", acumulated: " + std::to_string(fireCounters[status].getAcumulatedTime()));

        if (fireCounters[status].isFinished()) {
            int nextAnimationIndex = getCurrentWeaponAnimation()->getNextAnimationIndex();

            Logging::getInstance()->Log(getCurrentWeaponAnimation()->baseFile +  ": Finishing state... (next: " + std::to_string(nextAnimationIndex) + ")");

            fireCounters[nextAnimationIndex].reset();
            fireCounters[nextAnimationIndex].setEnabled( true );
            status = nextAnimationIndex;

            if (this->animations[ nextAnimationIndex ]->isLooping()) {
                if (!Mix_Playing(EngineSetup::SoundChannels::SND_WEAPON_LOOP)) {
                    Tools::playMixedSound( fireSounds[ nextAnimationIndex ], EngineSetup::SoundChannels::SND_WEAPON_LOOP, -1);
                    Logging::getInstance()->Log("Init sound looping mode");
                }
            } else {
                Mix_HaltChannel(EngineSetup::SoundChannels::SND_WEAPON_LOOP);
                Logging::getInstance()->Log("Init sound fire phase");
                Tools::playMixedSound( fireSounds[ nextAnimationIndex ], EngineSetup::SoundChannels::SND_WEAPON, 0);
            }

            setWeaponAnimation(nextAnimationIndex );

            if (status == EngineSetup::WeaponsActions::WALKING) {
                setFiring( false );
            }
        }

        std::vector<Counter>::iterator it;
        for ( it = fireCounters.begin(); it != fireCounters.end(); it++) {
            (it)->update();
        }
    }

    getCurrentWeaponAnimation()->updateFrame();
}

void WeaponType::setWeaponAnimation(int animationIndex)
{
    if (this->animations[animationIndex]->getNumFrames() == 0) {
        printf("Error animation with 0 frames");
        exit(-1);
    }

    this->animations[animationIndex]->currentFrame = 0;

    this->currentAnimationIndex = animationIndex;

    Logging::getInstance()->Log("setWeaponAnimation: " + std::to_string(animationIndex));
}

WeaponAnimation * WeaponType::getCurrentWeaponAnimation()
{
    return this->animations[currentAnimationIndex];
}

void WeaponType::setSpeed(float speed)
{
    this->speed = speed;
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

int WeaponType::getAmmo() const {
    return ammo;
}

void WeaponType::setAmmo(int ammo) {
    WeaponType::ammo = ammo;
}

float WeaponType::getAccuracy() const {
    return accuracy;
}

void WeaponType::setAccuracy(float accuracy) {
    WeaponType::accuracy = accuracy;
}

int WeaponType::getDispersion() const {
    return dispersion;
}

void WeaponType::setDispersion(float dispersion) {
    WeaponType::dispersion = dispersion;
}

int WeaponType::getSpeed() const {
    return speed;
}

void WeaponType::loadIconHUD(std::string file)
{
    std::string path = EngineSetup::getInstance()->SPRITES_FOLDER + file;

    Logging::getInstance()->Log("Loading weapon icon:" + path);

    this->iconHUD = IMG_Load(path.c_str());

    if (this->iconHUD == NULL) {
        Logging::getInstance()->Log("Error loading weapon icon:" + path);
    }
}

bool WeaponType::isFiring() const
{
    return firing;
}

void WeaponType::setFiring(bool firing)
{
    Logging::getInstance()->Log("setFiring: " + std::to_string((int) firing));

    WeaponType::firing = firing;

    if ( !firing ) {
        std::vector<Counter>::iterator it;
        for ( it = fireCounters.begin(); it != fireCounters.end(); it++) {
            (it)->setEnabled(false);
            (it)->reset();
        }
    }
}

bool WeaponType::isKeyDownHandle() const {
    return keyDownHandle;
}

void WeaponType::setKeyDownHandle(bool keyDownHandle) {
    WeaponType::keyDownHandle = keyDownHandle;
}

bool WeaponType::isKeyUpHandle() const {
    return keyUpHandle;
}

void WeaponType::setKeyUpHandle(bool keyUpHandle) {
    WeaponType::keyUpHandle = keyUpHandle;
}

int WeaponType::getKeyDownAnimationStatus() const {
    return keyDownAnimationStatus;
}

void WeaponType::setKeyDownAnimationStatus(int keyDownAnimationStatus) {
    WeaponType::keyDownAnimationStatus = keyDownAnimationStatus;
}

int WeaponType::getKeyUpAnimationStatus() const {
    return keyUpAnimationStatus;
}

void WeaponType::setKeyUpAnimationStatus(int keyUpAnimationStatus) {
    WeaponType::keyUpAnimationStatus = keyUpAnimationStatus;
}

bool WeaponType::isSniper() const {
    return sniper;
}

void WeaponType::setSniper(bool sniper) {
    WeaponType::sniper = sniper;
}

