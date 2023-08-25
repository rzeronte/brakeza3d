#include "ItemWeaponGhost.h"
#include "../../include/ComponentsManager.h"

ItemWeaponGhost::ItemWeaponGhost(Weapon *weaponType, bool hasFrame)
:
    frame(nullptr),
    hasFrame(hasFrame),
    weaponType(weaponType)
{
    if (hasFrame) {
        frame = new Mesh3D();
        frame->clone(ComponentsManager::get()->getComponentGame()->getItemBoxFrame());
        frame->setRotationFrameEnabled(true);
        frame->setRotationFrame(Tools::randomVertex().getScaled(0.5));
    }
}

Weapon *ItemWeaponGhost::getWeaponType() const {
    return weaponType;
}

void ItemWeaponGhost::postUpdate()
{
    Mesh3D::postUpdate();
    if (hasFrame) {
        frame->postUpdate();
    }
}

void ItemWeaponGhost::onUpdate()
{
    Mesh3D::onUpdate();

    if (!isEnabled()) return;

    if (hasFrame) {
        frame->onUpdate();
        frame->setPosition(getPosition());
    }

    if (ComponentsManager::get()->getComponentGame()->getGameState() == EngineSetup::GAMING) {
        this->magnetizableTo(ComponentsManager::get()->getComponentGame()->getPlayer());
    }
}

void ItemWeaponGhost::onDrawHostBuffer()
{
    Mesh3D::onDrawHostBuffer();

    if (hasFrame) {
        frame->onDrawHostBuffer();
    }
}

void ItemWeaponGhost::drawOnUpdateSecondPass()
{
    Object3D::drawOnUpdateSecondPass();

    if (hasFrame) {
        frame->drawOnUpdateSecondPass();
    }
}

ItemWeaponGhost::~ItemWeaponGhost()
{
    delete frame;
}
