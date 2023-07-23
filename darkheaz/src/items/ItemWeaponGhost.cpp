#include "ItemWeaponGhost.h"
#include "../../../include/ComponentsManager.h"

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

    setScale(0.5);
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

void ItemWeaponGhost::onDraw()
{
    Mesh3D::onDraw();

    if (hasFrame) {
        frame->onDraw();
    }
}

void ItemWeaponGhost::drawCall()
{
    Object3D::drawCall();

    if (hasFrame) {
        frame->drawCall();
    }
}

ItemWeaponGhost::~ItemWeaponGhost()
{
    delete frame;
}
