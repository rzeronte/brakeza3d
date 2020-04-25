
#include <SDL_image.h>
#include "../../headers/Game/WeaponType.h"
#include "../../headers/Render/Logging.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Brakeza3D.h"

WeaponType::WeaponType(): available(true)
{
}

WeaponType::WeaponType(std::string label)
{
    this->label = label;
    this->iconHUD = SDL_CreateRGBSurface(0, 100, 100, 32, 0, 0, 0, 0);

    makeProjectileTemplate();
    markTemplate = new Sprite3D();

    animations = new Mesh3DAnimatedCollection();
    animations->setFollowCamera( true );
    animations->setScale(0.05);
    animations->setLabel("weapon_" + label);
    animations->setEnabled( false );
    animations->rotationFixed = M3::getMatrixRotationForEulerAngles(90, 0, 0);

    Brakeza3D::get()->addObject3D(animations, animations->getLabel() );
}

void WeaponType::addAnimation(std::string label, std::string model, float scale, bool stopEnd)
{
    std::string full_animation_folder = EngineSetup::getInstance()->MODELS_FOLDER + model;

    this->animations->addAnimation( label, model, scale, stopEnd );

    this->numAnimations++;
}

void WeaponType::onUpdate(Camera3D *cam)
{
    if (status != 0) {

        // Light FX for Repeater
        if (Brakeza3D::get()->getComponentsManager()->getComponentWeapons()->currentWeaponIndex == EngineSetup::getInstance()->WeaponsTypes::REPEATER) {
            Vertex3D dir = cam->getRotation().getTranspose() * EngineSetup::getInstance()->forward;
            dir.getNormalize();

            Vertex3D A = cam->getPosition() + dir.getScaled(1);
            Vertex3D B = cam->getPosition() + dir.getScaled(50);

            Drawable::drawLightning( cam, A, B);
        }
    }
}

void WeaponType::setWeaponAnimation(int animationIndex)
{
    this->currentAnimationIndex = animationIndex;

    this->animations->setAnimation( animationIndex );

    if (EngineSetup::getInstance()->LOG_WEAPONS_SYSTEM)  {
        Logging::getInstance()->Log("setWeaponAnimation: " + std::to_string(animationIndex));
    }
}

Mesh3DAnimated * WeaponType::getCurrentWeaponAnimation()
{
    return this->animations->getCurrentMesh3DAnimated();
}

void WeaponType::setSpeed(float speed)
{
    this->speed = speed;
}

void WeaponType::makeProjectileTemplate()
{
    projectileTemplate = new Mesh3D();

    projectileTemplate->setPosition(Vertex3D(5, 0, -10));
}

Mesh3D* WeaponType::getProjectileTemplate()
{
    return this->projectileTemplate;
}

float WeaponType::getDamage()
{
    return this->damage;
}

void WeaponType::setDamage(float damage)
{
    this->damage = damage;
}

Sprite3D *WeaponType::getMarkTemplate()
{
    return markTemplate;
}

void WeaponType::setupMarkTemplate(std::string path, int numFrames, int fps, float w, float h)
{
    Logging::getInstance()->Log("setup Mark Template: " + std::string(path));
    markTemplate->setAutoRemoveAfterAnimation(true);
    markTemplate->setEnabled(true);
    markTemplate->addAnimation(path, numFrames, fps);
    markTemplate->setAnimation(0);
    markTemplate->getBillboard()->setDimensions(w, h);
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
    std::string path = EngineSetup::getInstance()->WEAPONS_FOLDER + file;

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
    if (EngineSetup::getInstance()->LOG_WEAPONS_SYSTEM)  {
        Logging::getInstance()->Log("setFiring: " + std::to_string((int) firing));
    }

    WeaponType::firing = firing;

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

void WeaponType::loadSniperHUD(std::string file) {
    std::string path = EngineSetup::getInstance()->WEAPONS_FOLDER + this->label + "/" + file;

    Logging::getInstance()->Log("Loading weapon Sniper HUD img:" + path);

    this->sniperHUD = IMG_Load(path.c_str());

    if (this->sniperHUD == NULL) {
        Logging::getInstance()->Log("Error loading Sniper HUD img:" + path);
    }
}

bool WeaponType::isSniperEnabled() const {
    return sniperEnabled;
}

void WeaponType::setSniperEnabled(bool sniperEnabled) {
    WeaponType::sniperEnabled = sniperEnabled;
}

const std::string &WeaponType::getClassname() const {
    return classname;
}

void WeaponType::setClassname(const std::string &classname) {
    WeaponType::classname = classname;
}

void WeaponType::setBillboardDimensions(int w, int h)
{
    this->billboardWidth  = w;
    this->billboardHeight = h;
}

const std::string &WeaponType::getBillboardTextureFile() const {
    return billboardTextureFile;
}

void WeaponType::setBillboardTextureFile(const std::string &billboardTexturePath) {
    WeaponType::billboardTextureFile = billboardTexturePath;
}

bool WeaponType::isAvailable() const {
    return available;
}

void WeaponType::setAvailable(bool available) {
    WeaponType::available = available;
}

int WeaponType::getIndex() const {
    return index;
}

void WeaponType::setIndex(int index) {
    WeaponType::index = index;
}

AmmoType *WeaponType::getAmmoType() const {
    return ammo;
}

void WeaponType::setAmmoType(AmmoType *ammo) {
    WeaponType::ammo = ammo;
}

float WeaponType::getDamageRadius() const {
    return damageRadius;
}

void WeaponType::setDamageRadius(float damageRadius) {
    WeaponType::damageRadius = damageRadius;
}

