
#include "Weapon.h"

#include "../../include/Brakeza3D.h"
#include "AmmoProjectileBody.h"
#include "../Items/ItemBombGhost.h"
#include "../Items/ItemShieldGhost.h"
#include "../Common/ShockWave.h"

Weapon::Weapon(WeaponAttributes attr)
        :
        enabled(false),
        parent(attr.parent),
        selectable(attr.selectable),
        cadenceTime(attr.cadenceTime),
        rayLight(attr.rayLight),
        model(new Mesh3D()),
        modelProjectile(new Mesh3D()),
        counterCadence(new Counter(attr.cadenceTime)),
        soundChannel(EngineSetup::SoundChannels::SND_GLOBAL),
        status(WeaponStatus::RELEASED),
        color(attr.projectileColor)
{
    setLabel(attr.name);

    counterCadence->setEnabled(false);

    getModel()->AssimpLoadGeometryFromFile(attr.weaponModel);

    getModelProjectile()->setFlatTextureColor(attr.projectileFlatTexture);
    getModelProjectile()->setEnableLights(attr.projectileEnableLights);
    getModelProjectile()->AssimpLoadGeometryFromFile(attr.projectileModel);
    getModelProjectile()->setLabel(Brakeza3D::uniqueObjectLabel("projectile_weapon_template"));
    getModelProjectile()->setFlatColor(attr.projectileColor);

    setIconImage(attr.icon);

    setAmmoAmount(attr.ammoAmount);
    setStartAmmoAmount(attr.startAmmoAmount);
    setSpeed(attr.speed);
    setDamage(attr.damage);
    setDispersion(attr.dispersion);
    setAccuracy(attr.accuracy);
    setCadenceTime(attr.cadenceTime);

    setStop(attr.stop);
    setAvailable(attr.available);

    if (attr.stop) {
        setStopDuration(attr.stopDuration);
        setStopEvery(attr.stopEver);
    }
}

Weapon::Weapon(WeaponAttributes attr, Mesh3D *externalModelProjectile)
:
    enabled(false),
    parent(attr.parent),
    selectable(attr.selectable),
    cadenceTime(attr.cadenceTime),
    rayLight(attr.rayLight),
    model(new Mesh3D()),
    modelProjectile(new Mesh3D()),
    counterCadence(new Counter(attr.cadenceTime)),
    soundChannel(EngineSetup::SoundChannels::SND_GLOBAL),
    status(WeaponStatus::RELEASED),
    color(attr.projectileColor)
{
    setLabel(attr.name);

    counterCadence->setEnabled(false);

    getModel()->AssimpLoadGeometryFromFile(attr.weaponModel);

    getModelProjectile()->clone(externalModelProjectile);
    getModelProjectile()->setFlatTextureColor(attr.projectileFlatTexture);
    getModelProjectile()->setEnableLights(attr.projectileEnableLights);


    setIconImage(attr.icon);

    setAmmoAmount(attr.ammoAmount);
    setStartAmmoAmount(attr.startAmmoAmount);
    setSpeed(attr.speed);
    setDamage(attr.damage);
    setDispersion(attr.dispersion);
    setAccuracy(attr.accuracy);
    setCadenceTime(attr.cadenceTime);

    setStop(attr.stop);
    setAvailable(attr.available);

    if (attr.stop) {
        setStopDuration(attr.stopDuration);
        setStopEvery(attr.stopEver);
    }
}

bool Weapon::shoot(WeaponShootAttributes attributes) {
    return false;
}

void Weapon::onUpdate()
{
    counterCadence->update();

    if (isStop()) {
        counterStopEvery.update();
        counterStopDuration.update();

        if (counterStopEvery.isFinished()) {
            counterStopDuration.setEnabled(true);
            counterStopEvery.setEnabled(false);
        }

        if (counterStopDuration.isFinished()) {
            counterStopEvery.setEnabled(true);
            counterStopDuration.setEnabled(false);
        }
    }

    if (status == PRESSED) {
        setStatus(SUSTAINED);
    }
}

void Weapon::stopSoundChannel() const {
    ComponentSound::stopChannel(getSoundChannel());
}

void Weapon::setSpeed(float value) {
    this->speed = value;
}

float Weapon::getDamage() const {
    return this->damage;
}

void Weapon::setDamage(float value) {
    this->damage = value;
}

float Weapon::getAccuracy() const {
    return accuracy;
}

void Weapon::setAccuracy(float value) {
    Weapon::accuracy = value;
}

int Weapon::getDispersion() const {
    return dispersion;
}

void Weapon::setDispersion(int value) {
    Weapon::dispersion = value;
}

float Weapon::getSpeed() const {
    return speed;
}

bool Weapon::isAvailable() const {
    return available;
}

void Weapon::setAvailable(bool value) {
    Weapon::available = value;
}

float Weapon::getDamageRadius() const {
    return damageRadius;
}

void Weapon::setDamageRadius(float value) {
    Weapon::damageRadius = value;
}

Mesh3D *Weapon::getModel() const {
    return model;
}

const std::string &Weapon::getLabel() const {
    return label;
}

void Weapon::setCadenceTime(float value) {
    Weapon::cadenceTime = value;
    this->counterCadence->setStep(value);
    this->counterCadence->setEnabled(true);
}

Image *Weapon::getIcon() const {
    return icon;
}

void Weapon::setIconImage(const std::string& file) {
    this->icon = new Image(file);
}

int Weapon::getAmmoAmount() const {
    return ammoAmount;
}

void Weapon::setAmmoAmount(int value) {
    Weapon::ammoAmount = value;
}

void Weapon::addAmount(int addAmount) {
    this->ammoAmount += addAmount;
}

Mesh3D *Weapon::getModelProjectile(){
    return modelProjectile;
}

int Weapon::getStatus() const {
    return status;
}

void Weapon::setStatus(int value)
{
    Weapon::status = value;
}

int Weapon::getStartAmmoAmount() const {
    return startAmmoAmount;
}

void Weapon::setStartAmmoAmount(int value) {
    Weapon::startAmmoAmount = value;
}

bool Weapon::isStop() const {
    return stop;
}

void Weapon::setStop(bool value) {
    Weapon::stop = value;
}

float Weapon::getStopDuration() const {
    return stopDuration;
}

void Weapon::setStopDuration(float value)
{
    Weapon::stopDuration = value;
    this->counterStopDuration.setStep(value);
    this->counterStopDuration.setEnabled(false);
}

float Weapon::getStopEvery() const {
    return stopEvery;
}

void Weapon::setStopEvery(float stopEverySeconds) {
    Weapon::stopEvery = stopEverySeconds;
    this->counterStopEvery.setStep(stopEverySeconds);
    this->counterStopEvery.setEnabled(true);
}

int Weapon::getSoundChannel() const {
    return soundChannel;
}

void Weapon::setSoundChannel(int value) {
    Weapon::soundChannel = value;
}

Weapon::~Weapon()
{
    delete modelProjectile;
    delete model;
    delete icon;
    delete counterCadence;
}

void Weapon::setLabel(const std::string &value) {
    Weapon::label = value;
}

Counter *Weapon::getCounterCadence() const {
    return counterCadence;
}

bool Weapon::isSelectable() const {
    return selectable;
}

void Weapon::drawImGuiProperties()
{
    ImGui::Text("Living projectiles: %d", numLiveProjectiles);
    ImGui::Separator();

    if (ImGui::TreeNode("Weapon Model")) {
        model->drawImGuiProperties();
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Weapon Model Projectile")) {
        modelProjectile->drawImGuiProperties();
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Weapon RayLight")) {
        rayLight->drawImGuiProperties();
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Controls")) {
        auto ImGuiTextures = Brakeza3D::get()->getManagerGui()->getImGuiTextures();

        ImGui::Text("Pressed");
        ImGui::SameLine();
        if (getStatus() == WeaponStatus::PRESSED) {
            ImGui::Image((ImTextureID)ImGuiTextures->getTextureByLabel("lockIcon")->getOGLTextureID(), ImVec2(18, 18));
        } else {
            ImGui::Image((ImTextureID)ImGuiTextures->getTextureByLabel("unlockIcon")->getOGLTextureID(), ImVec2(18, 18));
        }

        ImGui::Text("Sustained");
        ImGui::SameLine();
        if (getStatus() == WeaponStatus::SUSTAINED) {
            ImGui::Image((ImTextureID)ImGuiTextures->getTextureByLabel("lockIcon")->getOGLTextureID(), ImVec2(18, 18));
        } else {
            ImGui::Image((ImTextureID)ImGuiTextures->getTextureByLabel("unlockIcon")->getOGLTextureID(), ImVec2(18, 18));
        }

        ImGui::Text("Released");
        ImGui::SameLine();
        if (getStatus() == WeaponStatus::RELEASED) {
            ImGui::Image((ImTextureID)ImGuiTextures->getTextureByLabel("lockIcon")->getOGLTextureID(), ImVec2(18, 18));
        } else {
            ImGui::Image((ImTextureID)ImGuiTextures->getTextureByLabel("unlockIcon")->getOGLTextureID(), ImVec2(18, 18));
        }
        ImGui::TreePop();
    }
}

bool Weapon::isEnabled() const {
    return enabled;
}

void Weapon::setEnabled(bool enabled) {
    Weapon::enabled = enabled;
}

void Weapon::increaseNumberProjectiles() {
    numLiveProjectiles++;
}

void Weapon::decreaseNumberProjectiles() {
    numLiveProjectiles--;
    if (numLiveProjectiles < 0) {
        numLiveProjectiles = 0;
    }
}

const Color &Weapon::getColor() const {
    return color;
}

void Weapon::setColor(const Color &color) {
    Weapon::color = color;
}
